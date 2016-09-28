#include "OverlayRenderer.hpp"

#include "Engine.hpp"
#include "MaterialCache.hpp"
#include "SamplerCache.hpp"
#include "ShaderCache.hpp"

#include "BorderPanelOverlay.hpp"
#include "Overlay.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/MatrixFrameVariable.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>

#include <GlslSource.hpp>

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	static const int32_t C3D_MAX_CHARS_PER_BUFFER = 6000;

	namespace
	{
		uint32_t FillBuffers( OverlayCategory::VertexArray::const_iterator p_begin, uint32_t p_count, VertexBuffer & p_buffers )
		{
			OverlayCategory::Vertex const & l_vertex = *p_begin;
			p_buffers.Fill( reinterpret_cast< uint8_t const * >( &l_vertex ), p_count * sizeof( OverlayCategory::Vertex ), BufferAccessType::Dynamic, BufferAccessNature::Draw );
			return p_count;
		}

		uint32_t FillBuffers( TextOverlay::VertexArray::const_iterator p_begin, uint32_t p_count, VertexBuffer & p_buffers )
		{
			TextOverlay::Vertex const & l_vertex = *p_begin;
			p_buffers.Fill( reinterpret_cast< uint8_t const * >( &l_vertex ), p_count * sizeof( TextOverlay::Vertex ), BufferAccessType::Dynamic, BufferAccessNature::Draw );
			return p_count;
		}
	}

	OverlayRenderer::OverlayRenderer( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_declaration{ 
		{
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::Position ), ElementType::IVec2 ),
				BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::TexCoords ), ElementType::Vec2 )
			}
		} }
		, m_textDeclaration{
		{
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::Position ), ElementType::IVec2 ),
				BufferElementDeclaration( ShaderProgram::Text, uint32_t( ElementUsage::TexCoords ), ElementType::Vec2, 0 ),
				BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::TexCoords ), ElementType::Vec2, 1 )
			}
		} }
	{
	}

	OverlayRenderer::~OverlayRenderer()
	{
		for ( auto & l_vertex : m_borderVertex )
		{
			l_vertex.reset();
		}

		for ( auto & l_vertex : m_panelVertex )
		{
			l_vertex.reset();
		}
	}

	void OverlayRenderer::Initialise()
	{
		if ( !m_panelVertexBuffer )
		{
			// Panel Overlays buffers
			m_panelVertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			uint32_t l_stride = m_declaration.GetStride();
			m_panelVertexBuffer->Resize( uint32_t( m_panelVertex.size() * l_stride ) );
			uint8_t * l_buffer = m_panelVertexBuffer->data();

			for ( auto & l_vertex : m_panelVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_stride;
			}

			m_panelVertexBuffer->Create();
			m_panelVertexBuffer->Initialise( BufferAccessType::Dynamic, BufferAccessNature::Draw );

			{
				auto & l_pipeline = DoGetPanelPipeline( 0 );
				m_panelGeometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, l_pipeline.GetProgram() );
				m_panelGeometryBuffers.m_noTexture->Initialise( m_panelVertexBuffer, nullptr, nullptr, nullptr, nullptr );
			}
			{
				auto & l_pipeline = DoGetPanelPipeline( uint32_t( TextureChannel::Colour ) );
				m_panelGeometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, l_pipeline.GetProgram() );
				m_panelGeometryBuffers.m_textured->Initialise( m_panelVertexBuffer, nullptr, nullptr, nullptr, nullptr );
			}
		}

		if ( !m_borderVertexBuffer )
		{
			// Border Overlays buffers
			m_borderVertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			uint32_t l_stride = m_declaration.GetStride();
			m_borderVertexBuffer->Resize( uint32_t( m_borderVertex.size() * l_stride ) );
			uint8_t * l_buffer = m_borderVertexBuffer->data();

			for ( auto & l_vertex : m_borderVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_stride;
			}

			m_borderVertexBuffer->Create();
			m_borderVertexBuffer->Initialise( BufferAccessType::Dynamic, BufferAccessNature::Draw );

			{
				auto & l_pipeline = DoGetPanelPipeline( 0 );
				m_borderGeometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, l_pipeline.GetProgram() );
				m_borderGeometryBuffers.m_noTexture->Initialise( m_borderVertexBuffer, nullptr, nullptr, nullptr, nullptr );
			}
			{
				auto & l_pipeline = DoGetPanelPipeline( uint32_t( TextureChannel::Colour ) );
				m_borderGeometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, l_pipeline.GetProgram() );
				m_borderGeometryBuffers.m_textured->Initialise( m_borderVertexBuffer, nullptr, nullptr, nullptr, nullptr );
			}
		}

		// Create one text overlays buffer
		DoCreateTextGeometryBuffers();
	}

	void OverlayRenderer::Cleanup()
	{
		for ( auto & l_vertex : m_borderVertex )
		{
			l_vertex.reset();
		}

		for ( auto & l_vertex : m_panelVertex )
		{
			l_vertex.reset();
		}

		for ( auto & l_pair : m_pipelines )
		{
			l_pair.second->Cleanup();
		}

		m_pipelines.clear();
		m_mapPanelNodes.clear();
		m_mapTextNodes.clear();

		if ( m_panelVertexBuffer )
		{
			m_panelGeometryBuffers.m_noTexture->Cleanup();
			m_panelGeometryBuffers.m_textured->Cleanup();
			m_panelGeometryBuffers.m_noTexture.reset();
			m_panelGeometryBuffers.m_textured.reset();
			m_panelVertexBuffer->Cleanup();
			m_panelVertexBuffer->Destroy();
			m_panelVertexBuffer.reset();
		}

		if ( m_borderVertexBuffer )
		{
			m_borderGeometryBuffers.m_noTexture->Cleanup();
			m_borderGeometryBuffers.m_textured->Cleanup();
			m_borderGeometryBuffers.m_noTexture.reset();
			m_borderGeometryBuffers.m_textured.reset();
			m_borderVertexBuffer->Cleanup();
			m_borderVertexBuffer->Destroy();
			m_borderVertexBuffer.reset();
		}

		for ( auto & l_buffer : m_textsGeometryBuffers )
		{
			l_buffer.m_noTexture->Cleanup();
			l_buffer.m_textured->Cleanup();
		}

		m_textsGeometryBuffers.clear();

		for ( auto l_buffer : m_textsVertexBuffers )
		{
			l_buffer->Cleanup();
			l_buffer->Destroy();
		}

		m_textsVertexBuffers.clear();
	}

	void OverlayRenderer::DrawPanel( PanelOverlay & p_overlay )
	{
		MaterialSPtr l_material = p_overlay.GetMaterial();

		if ( l_material )
		{
			DoDrawItem( *l_material, m_panelGeometryBuffers, FillBuffers( p_overlay.GetPanelVertex().begin(), uint32_t( p_overlay.GetPanelVertex().size() ), *m_panelVertexBuffer ) );
		}
	}

	void OverlayRenderer::DrawBorderPanel( BorderPanelOverlay & p_overlay )
	{
		MaterialSPtr l_material = p_overlay.GetMaterial();

		if ( l_material )
		{
			DoDrawItem( *l_material, m_panelGeometryBuffers, FillBuffers( p_overlay.GetPanelVertex().begin(), uint32_t( p_overlay.GetPanelVertex().size() ), *m_panelVertexBuffer ) );
		}

		l_material = p_overlay.GetBorderMaterial();

		if ( l_material )
		{
			DoDrawItem( *l_material, m_borderGeometryBuffers, FillBuffers( p_overlay.GetBorderVertex().begin(), uint32_t( p_overlay.GetBorderVertex().size() ), *m_borderVertexBuffer ) );
		}
	}

	void OverlayRenderer::DrawText( TextOverlay & p_overlay )
	{
		FontSPtr l_pFont = p_overlay.GetFontTexture()->GetFont();

		if ( l_pFont )
		{
			MaterialSPtr l_material = p_overlay.GetMaterial();

			if ( l_material )
			{
				TextOverlay::VertexArray l_arrayVtx = p_overlay.GetTextVertex();
				int32_t l_count = uint32_t( l_arrayVtx.size() );
				uint32_t l_index = 0;
				std::vector< OverlayGeometryBuffers > l_geometryBuffers;
				TextOverlay::VertexArray::const_iterator l_it = l_arrayVtx.begin();

				while ( l_count > C3D_MAX_CHARS_PER_BUFFER )
				{
					l_geometryBuffers.push_back( DoFillTextPart( l_count, l_it, l_index ) );
					l_count -= C3D_MAX_CHARS_PER_BUFFER;
				}

				if ( l_count > 0 )
				{
					l_geometryBuffers.push_back( DoFillTextPart( l_count, l_it, l_index ) );
				}

				auto l_texture = p_overlay.GetFontTexture()->GetTexture();
				auto l_sampler = p_overlay.GetFontTexture()->GetSampler();
				l_count = uint32_t( l_arrayVtx.size() );

				for ( auto l_pass : *l_material )
				{
					if ( CheckFlag( l_pass->GetTextureFlags(), TextureChannel::Colour ) )
					{
						for ( auto l_geoBuffers : l_geometryBuffers )
						{
							DoDrawItem( *l_pass, *l_geoBuffers.m_textured, *l_texture, *l_sampler, std::min( l_count, C3D_MAX_CHARS_PER_BUFFER ) );
							l_count -= C3D_MAX_CHARS_PER_BUFFER;
						}
					}
					else
					{
						for ( auto l_geoBuffers : l_geometryBuffers )
						{
							DoDrawItem( *l_pass, *l_geoBuffers.m_noTexture, *l_texture, *l_sampler, std::min( l_count, C3D_MAX_CHARS_PER_BUFFER ) );
							l_count -= C3D_MAX_CHARS_PER_BUFFER;
						}
					}
				}
			}
		}
	}

	void OverlayRenderer::BeginRender( Viewport const & p_viewport )
	{
		auto l_size = p_viewport.GetSize();

		if ( m_size != l_size )
		{
			m_sizeChanged = true;
			m_size = l_size;
		}

		for ( auto & l_it : m_pipelines )
		{
			l_it.second->SetProjectionMatrix( p_viewport.GetProjection() );
		}
	}

	void OverlayRenderer::EndRender()
	{
		m_sizeChanged = false;
	}

	RenderNode & OverlayRenderer::DoGetPanelNode( Pass & p_pass )
	{
		auto l_it = m_mapPanelNodes.find( &p_pass );

		if ( l_it == m_mapPanelNodes.end() )
		{
			auto & l_pipeline = DoGetPanelPipeline( p_pass.GetTextureFlags() );

			auto l_sceneBuffer = l_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferScene );
			auto l_passBuffer = l_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferPass );
			Point4rFrameVariableSPtr l_pt4r;
			OneFloatFrameVariableSPtr l_1f;

			l_it = m_mapPanelNodes.insert( { &p_pass, RenderNode
				{
					p_pass,
					l_pipeline,
					*l_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix ),
					*l_passBuffer,
					*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
					*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
				}
			} ).first;
			p_pass.FillRenderNode( l_it->second );
		}

		return l_it->second;
	}

	RenderNode & OverlayRenderer::DoGetTextNode( Pass & p_pass )
	{
		auto l_it = m_mapTextNodes.find( &p_pass );

		if ( l_it == m_mapTextNodes.end() )
		{
			auto & l_pipeline = DoGetTextPipeline( p_pass.GetTextureFlags() );

			auto l_sceneBuffer = l_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferScene );
			auto l_passBuffer = l_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferPass );
			Point4rFrameVariableSPtr l_pt4r;
			OneFloatFrameVariableSPtr l_1f;

			l_it = m_mapTextNodes.insert( { &p_pass, RenderNode
				{
					p_pass,
					l_pipeline,
					*l_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix ),
					*l_passBuffer,
					*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
					*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
					*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
				}
			} ).first;
			p_pass.FillRenderNode( l_it->second );
		}

		return l_it->second;
	}

	Pipeline & OverlayRenderer::DoGetPanelPipeline( uint16_t p_textureFlags )
	{
		// Remove unwanted flags
		RemFlag( p_textureFlags, TextureChannel::Ambient );
		RemFlag( p_textureFlags, TextureChannel::Diffuse );
		RemFlag( p_textureFlags, TextureChannel::Normal );
		RemFlag( p_textureFlags, TextureChannel::Specular );
		RemFlag( p_textureFlags, TextureChannel::Gloss );
		RemFlag( p_textureFlags, TextureChannel::Height );
		RemFlag( p_textureFlags, TextureChannel::Emissive );

		// Get shader
		return DoGetPipeline( p_textureFlags );
	}

	Pipeline & OverlayRenderer::DoGetTextPipeline( uint16_t p_textureFlags )
	{
		// Remove unwanted flags
		RemFlag( p_textureFlags, TextureChannel::Ambient );
		RemFlag( p_textureFlags, TextureChannel::Diffuse );
		RemFlag( p_textureFlags, TextureChannel::Normal );
		RemFlag( p_textureFlags, TextureChannel::Specular );
		RemFlag( p_textureFlags, TextureChannel::Gloss );
		RemFlag( p_textureFlags, TextureChannel::Height );
		RemFlag( p_textureFlags, TextureChannel::Emissive );
		AddFlag( p_textureFlags, TextureChannel::Text );

		// Get shader
		return DoGetPipeline( p_textureFlags );
	}

	Pipeline & OverlayRenderer::DoGetPipeline( uint16_t p_textureFlags )
	{
		auto l_it = m_pipelines.find( p_textureFlags );

		if ( l_it == m_pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			auto l_program = DoCreateOverlayProgram( p_textureFlags );

			if ( l_program )
			{
				l_program->Initialise();
				BlendState l_blState;
				l_blState.SetAlphaSrcBlend( BlendOperand::SrcAlpha );
				l_blState.SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
				l_blState.SetRgbSrcBlend( BlendOperand::SrcAlpha );
				l_blState.SetRgbDstBlend( BlendOperand::InvSrcAlpha );
				l_blState.EnableBlend( true );

				MultisampleState l_msState;
				l_msState.EnableAlphaToCoverage( false );

				DepthStencilState l_dsState;
				l_dsState.SetDepthTest( false );

				RasteriserState l_rsState;
				l_rsState.SetCulledFaces( Culling::Back );

				auto l_pipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsState ), std::move( l_rsState ), std::move( l_blState ), std::move( l_msState ), *l_program );
				l_it = m_pipelines.emplace( p_textureFlags, std::move( l_pipeline ) ).first;
			}
			else
			{
				CASTOR_EXCEPTION( "Couldn't create an overlay shader program" );
			}
		}

		return *l_it->second;
	}

	OverlayRenderer::OverlayGeometryBuffers OverlayRenderer::DoCreateTextGeometryBuffers()
	{
		auto l_vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_textDeclaration );
		l_vertexBuffer->Resize( C3D_MAX_CHARS_PER_BUFFER * m_textDeclaration.GetStride() );
		l_vertexBuffer->Create();
		l_vertexBuffer->Initialise( BufferAccessType::Dynamic, BufferAccessNature::Draw );

		OverlayGeometryBuffers l_geometryBuffers;
		{
			auto & l_pipeline = DoGetTextPipeline( 0 );
			l_geometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, l_pipeline.GetProgram() );
			l_geometryBuffers.m_noTexture->Initialise( l_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
		}
		{
			auto & l_pipeline = DoGetTextPipeline( uint32_t( TextureChannel::Colour ) );
			l_geometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( Topology::Triangles, l_pipeline.GetProgram() );
			l_geometryBuffers.m_textured->Initialise( l_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
		}

		m_textsVertexBuffers.push_back( std::move( l_vertexBuffer ) );
		m_textsGeometryBuffers.push_back( l_geometryBuffers );
		return l_geometryBuffers;
	}

	void OverlayRenderer::DoDrawItem( Pass & p_pass, GeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		RenderNode & l_node = DoGetPanelNode( p_pass );
		l_node.m_pipeline.ApplyProjection( l_node.m_matrixUbo );
		p_pass.FillShaderVariables( l_node );
		l_node.m_pipeline.Apply();
		p_pass.Render2D();
		l_node.m_pipeline.GetProgram().BindUbos();
		p_geometryBuffers.Draw( p_count, 0 );
		l_node.m_pipeline.GetProgram().UnbindUbos();
		p_pass.EndRender();
	}

	void OverlayRenderer::DoDrawItem( Pass & p_pass, GeometryBuffers const & p_geometryBuffers, TextureLayout const & p_texture, Sampler const & p_sampler , uint32_t p_count )
	{
		RenderNode & l_node = DoGetTextNode( p_pass );
		l_node.m_pipeline.ApplyProjection( l_node.m_matrixUbo );

		OneIntFrameVariableSPtr l_textureVariable = l_node.m_pipeline.GetProgram().FindFrameVariable< OneIntFrameVariable >( ShaderProgram::MapText, ShaderType::Pixel );

		if ( l_textureVariable )
		{
			l_textureVariable->SetValue( 0 );
		}

		p_pass.FillShaderVariables( l_node );
		l_node.m_pipeline.Apply();
		l_node.m_pipeline.GetProgram().BindUbos();
		p_pass.Render2D();
		p_texture.Bind( 0 );
		p_sampler.Bind( 0 );
		p_geometryBuffers.Draw( p_count, 0 );
		p_sampler.Unbind( 0 );
		p_texture.Unbind( 0 );

		p_pass.EndRender();
		l_node.m_pipeline.GetProgram().UnbindUbos();
	}

	void OverlayRenderer::DoDrawItem( Material & p_material, OverlayRenderer::OverlayGeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		for ( auto l_pass : p_material )
		{
			if ( CheckFlag( l_pass->GetTextureFlags(), TextureChannel::Colour ) )
			{
				DoDrawItem( *l_pass, *p_geometryBuffers.m_textured, p_count );
			}
			else
			{
				DoDrawItem( *l_pass, *p_geometryBuffers.m_noTexture, p_count );
			}
		}
	}

	OverlayRenderer::OverlayGeometryBuffers OverlayRenderer::DoFillTextPart( int32_t p_count, TextOverlay::VertexArray::const_iterator & p_it, uint32_t & p_index )
	{
		OverlayGeometryBuffers l_geometryBuffers;

		if ( m_textsGeometryBuffers.size() <= p_index )
		{
			DoCreateTextGeometryBuffers();
		}

		if ( m_textsGeometryBuffers.size() > p_index )
		{
			l_geometryBuffers = m_textsGeometryBuffers[p_index];
			auto & l_vertexBuffer = m_textsVertexBuffers[p_index];
			p_count = std::min( p_count, C3D_MAX_CHARS_PER_BUFFER );
			FillBuffers( p_it, p_count, *l_vertexBuffer );
			p_it += p_count;
			p_index++;
		}
		else
		{
			CASTOR_EXCEPTION( "Bad index for geometry buffers" );
		}

		return l_geometryBuffers;
	}

	ShaderProgramSPtr OverlayRenderer::DoCreateOverlayProgram( uint16_t p_textureFlags )
	{
		using namespace GLSL;

		// Shader program
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderProgramSPtr l_program = l_cache.GetNewProgram();
		l_cache.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
		l_cache.CreatePassBuffer( *l_program, MASK_SHADER_TYPE_PIXEL );

		// Vertex shader
		String l_strVs;
		{
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			auto position = l_writer.GetAttribute< IVec2 >( ShaderProgram::Position );
			auto text = l_writer.GetAttribute< Vec2 >( ShaderProgram::Text, CheckFlag( p_textureFlags, TextureChannel::Text ) );
			auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture, CheckFlag( p_textureFlags, TextureChannel::Colour ) );

			// Shader outputs
			auto vtx_text = l_writer.GetOutput< Vec2 >( cuT( "vtx_text" ), CheckFlag( p_textureFlags, TextureChannel::Text ) );
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ), CheckFlag( p_textureFlags, TextureChannel::Colour ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				if ( CheckFlag( p_textureFlags, TextureChannel::Text ) )
				{
					vtx_text = text;
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::Colour ) )
				{
					vtx_texture = texture;
				}

				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );

			l_strVs = l_writer.Finalise();
		}

		// Pixel shader
		String l_strPs;
		{
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			UBO_PASS( l_writer );

			// Shader inputs
			auto vtx_text = l_writer.GetInput< Vec2 >( cuT( "vtx_text" ), CheckFlag( p_textureFlags, TextureChannel::Text ) );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ), CheckFlag( p_textureFlags, TextureChannel::Colour ) );
			auto c3d_mapText = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapText, CheckFlag( p_textureFlags, TextureChannel::Text ) );
			auto c3d_mapColour = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::Colour ) );
			auto c3d_mapOpacity = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::Opacity ) );

			// Shader outputs
			auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Ambient, c3d_v4MatAmbient );
				LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );

				if ( CheckFlag( p_textureFlags, TextureChannel::Text ) )
				{
					l_fAlpha *= texture( c3d_mapText, vec2( vtx_text.x(), vtx_text.y() ) ).SWIZZLE_R;
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::Colour ) )
				{
					l_v4Ambient = texture( c3d_mapColour, vec2( vtx_texture.x(), vtx_texture.y() ) );
				}

				if ( CheckFlag( p_textureFlags, TextureChannel::Opacity ) )
				{
					l_fAlpha *= texture( c3d_mapOpacity, vec2( vtx_texture.x(), vtx_texture.y() ) ).SWIZZLE_R;
				}

				pxl_v4FragColor = vec4( l_v4Ambient.xyz(), l_fAlpha );
			} );

			l_strPs = l_writer.Finalise();
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Text ) )
		{
			l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapText, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Colour ) )
		{
			l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapColour, ShaderType::Pixel );
		}

		if ( CheckFlag( p_textureFlags, TextureChannel::Opacity ) )
		{
			l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapOpacity, ShaderType::Pixel );
		}

		eSHADER_MODEL l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		l_program->SetSource( ShaderType::Vertex, l_model, l_strVs );
		l_program->SetSource( ShaderType::Pixel, l_model, l_strPs );

		return l_program;
	}
}

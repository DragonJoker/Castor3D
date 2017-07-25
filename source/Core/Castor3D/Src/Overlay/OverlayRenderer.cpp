#include "OverlayRenderer.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "PanelOverlay.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	static const int32_t C3D_MAX_CHARS_PER_BUFFER = 6000;

	namespace
	{
		uint32_t FillBuffers( OverlayCategory::VertexArray::const_iterator begin
			, uint32_t count
			, VertexBuffer & vbo )
		{
			OverlayCategory::Vertex const & vertex = *begin;
			auto size = count * sizeof( OverlayCategory::Vertex );

			vbo.Bind();
			auto buffer = vbo.Lock( 0, uint32_t( size ), AccessType::eWrite );

			if ( buffer )
			{
				std::memcpy( buffer, reinterpret_cast< uint8_t const * >( &vertex ), size );
				vbo.Unlock();
			}

			vbo.Unbind();
			return count;
		}

		uint32_t FillBuffers( TextOverlay::VertexArray::const_iterator begin
			, uint32_t count
			, VertexBuffer & vbo )
		{
			TextOverlay::Vertex const & vertex = *begin;
			auto size = count * sizeof( TextOverlay::Vertex );

			vbo.Bind();
			auto buffer = vbo.Lock( 0, uint32_t( size ), AccessType::eWrite );

			if ( buffer )
			{
				std::memcpy( buffer, reinterpret_cast< uint8_t const * >( &vertex ), size );
				vbo.Unlock();
			}

			vbo.Unbind();
			return count;
		}
	}

	OverlayRenderer::OverlayRenderer( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >( renderSystem )
		, m_declaration{
			{
				{
					BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eIVec2 ),
					BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 )
				}
			} }
		, m_textDeclaration{
			{
				{
					BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eIVec2 ),
					BufferElementDeclaration( ShaderProgram::Text, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2, 0 ),
					BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2, 1 )
				}
			} }
		, m_matrixUbo{ *renderSystem.GetEngine() }
		, m_overlayUbo{ *renderSystem.GetEngine() }
	{
	}

	OverlayRenderer::~OverlayRenderer()
	{
		for ( auto & vertex : m_borderVertex )
		{
			vertex.reset();
		}

		for ( auto & vertex : m_panelVertex )
		{
			vertex.reset();
		}
	}

	void OverlayRenderer::Initialise()
	{
		if ( !m_panelVertexBuffer )
		{
			// Panel Overlays buffers
			m_panelVertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			uint32_t stride = m_declaration.stride();
			m_panelVertexBuffer->Resize( uint32_t( m_panelVertex.size() * stride ) );
			uint8_t * buffer = m_panelVertexBuffer->GetData();

			for ( auto & vertex : m_panelVertex )
			{
				vertex = std::make_shared< BufferElementGroup >( buffer );
				buffer += stride;
			}

			m_panelVertexBuffer->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			{
				auto & pipeline = DoGetPanelPipeline( 0u );
				m_panelGeometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, pipeline.GetProgram() );
				m_panelGeometryBuffers.m_noTexture->Initialise( { *m_panelVertexBuffer }, nullptr );
			}
			{
				auto & pipeline = DoGetPanelPipeline( uint32_t( TextureChannel::eDiffuse ) );
				m_panelGeometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, pipeline.GetProgram() );
				m_panelGeometryBuffers.m_textured->Initialise( { *m_panelVertexBuffer }, nullptr );
			}
		}

		if ( !m_borderVertexBuffer )
		{
			// Border Overlays buffers
			m_borderVertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			uint32_t stride = m_declaration.stride();
			m_borderVertexBuffer->Resize( uint32_t( m_borderVertex.size() * stride ) );
			uint8_t * buffer = m_borderVertexBuffer->GetData();

			for ( auto & vertex : m_borderVertex )
			{
				vertex = std::make_shared< BufferElementGroup >( buffer );
				buffer += stride;
			}

			m_borderVertexBuffer->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			{
				auto & pipeline = DoGetPanelPipeline( 0 );
				m_borderGeometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, pipeline.GetProgram() );
				m_borderGeometryBuffers.m_noTexture->Initialise( { *m_borderVertexBuffer }, nullptr );
			}
			{
				auto & pipeline = DoGetPanelPipeline( uint32_t( TextureChannel::eDiffuse ) );
				m_borderGeometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, pipeline.GetProgram() );
				m_borderGeometryBuffers.m_textured->Initialise( { *m_borderVertexBuffer }, nullptr );
			}
		}

		// Create one text overlays buffer
		DoCreateTextGeometryBuffers();
	}

	void OverlayRenderer::Cleanup()
	{
		for ( auto & vertex : m_borderVertex )
		{
			vertex.reset();
		}

		for ( auto & vertex : m_panelVertex )
		{
			vertex.reset();
		}

		for ( auto & pair : m_pipelines )
		{
			pair.second->Cleanup();
		}

		m_overlayUbo.GetUbo().Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
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
			m_panelVertexBuffer.reset();
		}

		if ( m_borderVertexBuffer )
		{
			m_borderGeometryBuffers.m_noTexture->Cleanup();
			m_borderGeometryBuffers.m_textured->Cleanup();
			m_borderGeometryBuffers.m_noTexture.reset();
			m_borderGeometryBuffers.m_textured.reset();
			m_borderVertexBuffer->Cleanup();
			m_borderVertexBuffer.reset();
		}

		for ( auto & buffer : m_textsGeometryBuffers )
		{
			buffer.m_noTexture->Cleanup();
			buffer.m_textured->Cleanup();
		}

		m_textsGeometryBuffers.clear();

		for ( auto buffer : m_textsVertexBuffers )
		{
			buffer->Cleanup();
		}

		m_textsVertexBuffers.clear();
	}

	void OverlayRenderer::DrawPanel( PanelOverlay & overlay )
	{
		MaterialSPtr material = overlay.GetMaterial();

		if ( material )
		{
			m_overlayUbo.SetPosition( overlay.GetAbsolutePosition( m_size ) );
			DoDrawItem( *material
				, m_panelGeometryBuffers
				, FillBuffers( overlay.GetPanelVertex().begin()
					, uint32_t( overlay.GetPanelVertex().size() )
					, *m_panelVertexBuffer ) );
		}
	}

	void OverlayRenderer::DrawBorderPanel( BorderPanelOverlay & overlay )
	{
		{
			auto material = overlay.GetMaterial();

			if ( material )
			{
				m_overlayUbo.SetPosition( overlay.GetAbsolutePosition( m_size ) );
				DoDrawItem( *material
					, m_panelGeometryBuffers
					, FillBuffers( overlay.GetPanelVertex().begin()
						, uint32_t( overlay.GetPanelVertex().size() )
						, *m_panelVertexBuffer ) );
			}
		}
		{
			auto material = overlay.GetBorderMaterial();

			if ( material )
			{
				m_overlayUbo.SetPosition( overlay.GetAbsolutePosition( m_size ) );
				DoDrawItem( *material
					, m_borderGeometryBuffers
					, FillBuffers( overlay.GetBorderVertex().begin()
						, uint32_t( overlay.GetBorderVertex().size() )
						, *m_borderVertexBuffer ) );
			}
		}
	}

	void OverlayRenderer::DrawText( TextOverlay & overlay )
	{
		FontSPtr pFont = overlay.GetFontTexture()->GetFont();

		if ( pFont )
		{
			MaterialSPtr material = overlay.GetMaterial();

			if ( material )
			{
				TextOverlay::VertexArray arrayVtx = overlay.GetTextVertex();
				int32_t count = uint32_t( arrayVtx.size() );
				uint32_t index = 0;
				std::vector< OverlayGeometryBuffers > geometryBuffers;
				TextOverlay::VertexArray::const_iterator it = arrayVtx.begin();

				while ( count > C3D_MAX_CHARS_PER_BUFFER )
				{
					geometryBuffers.push_back( DoFillTextPart( count, it, index ) );
					count -= C3D_MAX_CHARS_PER_BUFFER;
				}

				if ( count > 0 )
				{
					geometryBuffers.push_back( DoFillTextPart( count, it, index ) );
				}

				auto texture = overlay.GetFontTexture()->GetTexture();
				auto sampler = overlay.GetFontTexture()->GetSampler();
				count = uint32_t( arrayVtx.size() );
				m_overlayUbo.SetPosition( overlay.GetAbsolutePosition( m_size ) );

				for ( auto pass : *material )
				{
					if ( CheckFlag( pass->GetTextureFlags(), TextureChannel::eDiffuse ) )
					{
						for ( auto geoBuffers : geometryBuffers )
						{
							DoDrawItem( *pass
								, *geoBuffers.m_textured
								, *texture
								, *sampler
								, std::min( count, C3D_MAX_CHARS_PER_BUFFER ) );
							count -= C3D_MAX_CHARS_PER_BUFFER;
						}
					}
					else
					{
						for ( auto geoBuffers : geometryBuffers )
						{
							DoDrawItem( *pass
								, *geoBuffers.m_noTexture
								, *texture
								, *sampler
								, std::min( count, C3D_MAX_CHARS_PER_BUFFER ) );
							count -= C3D_MAX_CHARS_PER_BUFFER;
						}
					}
				}
			}
		}
	}

	void OverlayRenderer::BeginRender( Viewport const & viewport )
	{
		auto size = viewport.GetSize();

		if ( m_size != size )
		{
			m_sizeChanged = true;
			m_size = size;
		}

		m_matrixUbo.Update( viewport.GetProjection() );
	}

	void OverlayRenderer::EndRender()
	{
		m_sizeChanged = false;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::DoGetPanelNode( Pass & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto & pipeline = DoGetPanelPipeline( pass.GetTextureFlags() );
			it = m_mapPanelNodes.insert( { &pass, OverlayRenderNode
				{
					pipeline,
					{
						pass,
						pipeline.GetProgram(),
					},
					m_overlayUbo,
				}
			} ).first;
		}

		return it->second;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::DoGetTextNode( Pass & pass )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto & pipeline = DoGetTextPipeline( pass.GetTextureFlags() );
			it = m_mapTextNodes.insert( { &pass, OverlayRenderNode
				{
					pipeline,
					{
						pass,
						pipeline.GetProgram(),
					},
					m_overlayUbo,
				}
			} ).first;
		}

		return it->second;
	}

	RenderPipeline & OverlayRenderer::DoGetPanelPipeline( TextureChannels textureFlags )
	{
		// Remove unwanted flags
		RemFlag( textureFlags, TextureChannel::eNormal );
		RemFlag( textureFlags, TextureChannel::eSpecular );
		RemFlag( textureFlags, TextureChannel::eGloss );
		RemFlag( textureFlags, TextureChannel::eHeight );
		RemFlag( textureFlags, TextureChannel::eEmissive );

		// Get shader
		return DoGetPipeline( textureFlags );
	}

	RenderPipeline & OverlayRenderer::DoGetTextPipeline( TextureChannels textureFlags )
	{
		// Remove unwanted flags
		RemFlag( textureFlags, TextureChannel::eNormal );
		RemFlag( textureFlags, TextureChannel::eSpecular );
		RemFlag( textureFlags, TextureChannel::eGloss );
		RemFlag( textureFlags, TextureChannel::eHeight );
		RemFlag( textureFlags, TextureChannel::eEmissive );
		AddFlag( textureFlags, TextureChannel::eText );

		// Get shader
		return DoGetPipeline( textureFlags );
	}

	RenderPipeline & OverlayRenderer::DoGetPipeline( TextureChannels const & textureFlags )
	{
		auto it = m_pipelines.find( textureFlags );

		if ( it == m_pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			auto program = DoCreateOverlayProgram( textureFlags );

			if ( program )
			{
				program->Initialise();
				BlendState blState;
				blState.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
				blState.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				blState.SetRgbSrcBlend( BlendOperand::eSrcAlpha );
				blState.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				blState.EnableBlend( true );

				MultisampleState msState;
				msState.EnableAlphaToCoverage( false );

				DepthStencilState dsState;
				dsState.SetDepthMask( WritingMask::eZero );
				dsState.SetDepthTest( false );

				RasteriserState rsState;
				rsState.SetCulledFaces( Culling::eBack );

				auto pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( dsState ), std::move( rsState ), std::move( blState ), std::move( msState ), *program, PipelineFlags{} );
				pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
				pipeline->AddUniformBuffer( m_overlayUbo.GetUbo() );
				it = m_pipelines.emplace( textureFlags, std::move( pipeline ) ).first;
			}
			else
			{
				CASTOR_EXCEPTION( "Couldn't create an overlay shader program" );
			}
		}

		return *it->second;
	}

	OverlayRenderer::OverlayGeometryBuffers OverlayRenderer::DoCreateTextGeometryBuffers()
	{
		auto vertexBuffer = std::make_shared< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_textDeclaration );
		vertexBuffer->Resize( C3D_MAX_CHARS_PER_BUFFER * m_textDeclaration.stride() );
		vertexBuffer->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

		OverlayGeometryBuffers geometryBuffers;
		{
			auto & pipeline = DoGetTextPipeline( 0 );
			geometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, pipeline.GetProgram() );
			geometryBuffers.m_noTexture->Initialise( { *vertexBuffer }, nullptr );
		}
		{
			auto & pipeline = DoGetTextPipeline( uint32_t( TextureChannel::eDiffuse ) );
			geometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, pipeline.GetProgram() );
			geometryBuffers.m_textured->Initialise( { *vertexBuffer }, nullptr );
		}

		m_textsVertexBuffers.push_back( std::move( vertexBuffer ) );
		m_textsGeometryBuffers.push_back( geometryBuffers );
		return geometryBuffers;
	}

	void OverlayRenderer::DoDrawItem( Pass & pass
		, GeometryBuffers const & geometryBuffers
		, uint32_t count )
	{
		auto & node = DoGetPanelNode( pass );
		m_overlayUbo.Update( pass.GetId() );
		node.m_pipeline.Apply();
		pass.BindTextures();
		geometryBuffers.Draw( count, 0 );
		pass.UnbindTextures();
	}

	void OverlayRenderer::DoDrawItem( Pass & pass
		, GeometryBuffers const & geometryBuffers
		, TextureLayout const & texture
		, Sampler const & sampler
		, uint32_t count )
	{
		auto & node = DoGetTextNode( pass );

		auto textureVariable = node.m_pipeline.GetProgram().FindUniform< UniformType::eSampler >( ShaderProgram::MapText, ShaderType::ePixel );

		if ( textureVariable )
		{
			textureVariable->SetValue( Pass::LightBufferIndex );
		}

		m_overlayUbo.Update( pass.GetId() );
		node.m_pipeline.Apply();
		pass.BindTextures();
		texture.Bind( Pass::LightBufferIndex );
		sampler.Bind( Pass::LightBufferIndex );
		geometryBuffers.Draw( count, 0 );
		sampler.Unbind( Pass::LightBufferIndex );
		texture.Unbind( Pass::LightBufferIndex );
		pass.UnbindTextures();
	}

	void OverlayRenderer::DoDrawItem( Material & material
		, OverlayRenderer::OverlayGeometryBuffers const & geometryBuffers
		, uint32_t count )
	{
		for ( auto pass : material )
		{
			if ( CheckFlag( pass->GetTextureFlags(), TextureChannel::eDiffuse ) )
			{
				DoDrawItem( *pass, *geometryBuffers.m_textured, count );
			}
			else
			{
				DoDrawItem( *pass, *geometryBuffers.m_noTexture, count );
			}
		}
	}

	OverlayRenderer::OverlayGeometryBuffers OverlayRenderer::DoFillTextPart( int32_t count
		, TextOverlay::VertexArray::const_iterator & it
		, uint32_t & index )
	{
		OverlayGeometryBuffers geometryBuffers;

		if ( m_textsGeometryBuffers.size() <= index )
		{
			DoCreateTextGeometryBuffers();
		}

		if ( m_textsGeometryBuffers.size() > index )
		{
			geometryBuffers = m_textsGeometryBuffers[index];
			auto & vertexBuffer = m_textsVertexBuffers[index];
			count = std::min( count, C3D_MAX_CHARS_PER_BUFFER );
			FillBuffers( it, count, *vertexBuffer );
			it += count;
			index++;
		}
		else
		{
			CASTOR_EXCEPTION( "Bad index for geometry buffers" );
		}

		return geometryBuffers;
	}

	ShaderProgramSPtr OverlayRenderer::DoCreateOverlayProgram( TextureChannels const & textureFlags )
	{
		using namespace GLSL;

		// Shader program
		auto & cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );

		// Vertex shader
		GLSL::Shader strVs;
		{
			auto writer = GetRenderSystem()->CreateGlslWriter();

			UBO_MATRIX( writer );
			UBO_OVERLAY( writer );

			// Shader inputs
			auto position = writer.DeclAttribute< IVec2 >( ShaderProgram::Position );
			auto text = writer.DeclAttribute< Vec2 >( ShaderProgram::Text, CheckFlag( textureFlags, TextureChannel::eText ) );
			auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture, CheckFlag( textureFlags, TextureChannel::eDiffuse ) );

			// Shader outputs
			auto vtx_text = writer.DeclOutput< Vec2 >( cuT( "vtx_text" ), CheckFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ), CheckFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				if ( CheckFlag( textureFlags, TextureChannel::eText ) )
				{
					vtx_text = text;
				}

				if ( CheckFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					vtx_texture = texture;
				}

				gl_Position = c3d_mtxProjection * vec4( c3d_position.x() + position.x(), c3d_position.y() + position.y(), 0.0, 1.0 );
			} );

			strVs = writer.Finalise();
		}

		// Pixel shader
		GLSL::Shader strPs;
		{
			auto writer = GetRenderSystem()->CreateGlslWriter();

			std::unique_ptr< Materials > materials;

			switch ( GetRenderSystem()->GetEngine()->GetMaterialsType() )
			{
			case MaterialType::eLegacy:
				materials = std::make_unique< LegacyMaterials >( writer );
				break;

			case MaterialType::ePbrMetallicRoughness:
				materials = std::make_unique< PbrMRMaterials >( writer );
				break;

			case MaterialType::ePbrSpecularGlossiness:
				materials = std::make_unique< PbrSGMaterials >( writer );
				break;
			}

			materials->Declare();
			UBO_OVERLAY( writer );

			// Shader inputs
			auto vtx_text = writer.DeclInput< Vec2 >( cuT( "vtx_text" ), CheckFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ), CheckFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapText = writer.DeclUniform< Sampler2D >( ShaderProgram::MapText, CheckFlag( textureFlags, TextureChannel::eText ) );
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapOpacity = writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( textureFlags, TextureChannel::eOpacity ) );

			// Shader outputs
			auto pxl_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto diffuse = writer.DeclLocale( cuT( "diffuse" ), materials->GetDiffuse( c3d_materialIndex ) );
				auto alpha = writer.DeclLocale( cuT( "alpha" ), materials->GetOpacity( c3d_materialIndex ) );

				if ( CheckFlag( textureFlags, TextureChannel::eText ) )
				{
					alpha *= texture( c3d_mapText, vec2( vtx_text.x(), vtx_text.y() ) ).r();
				}

				if ( CheckFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					diffuse = texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz();
				}

				if ( CheckFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, vec2( vtx_texture.x(), vtx_texture.y() ) ).r();
				}

				pxl_v4FragColor = vec4( diffuse.xyz(), alpha );
			} );

			strPs = writer.Finalise();
		}

		if ( CheckFlag( textureFlags, TextureChannel::eText ) )
		{
			program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapText, ShaderType::ePixel )->SetValue( Pass::LightBufferIndex );
		}

		auto index = Pass::MinTextureIndex;

		if ( CheckFlag( textureFlags, TextureChannel::eDiffuse ) )
		{
			program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->SetValue( index++ );
		}

		if ( CheckFlag( textureFlags, TextureChannel::eOpacity ) )
		{
			program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapOpacity, ShaderType::ePixel )->SetValue( index++ );
		}

		program->SetSource( ShaderType::eVertex, strVs );
		program->SetSource( ShaderType::ePixel, strPs );
		program->Initialise();
		return program;
	}
}

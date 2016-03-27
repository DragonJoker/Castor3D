#include "OverlayRenderer.hpp"

#include "BlendStateManager.hpp"
#include "BorderPanelOverlay.hpp"
#include "Buffer.hpp"
#include "DepthStencilStateManager.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "MaterialManager.hpp"
#include "MatrixFrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "Overlay.hpp"
#include "PanelOverlay.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "PointFrameVariable.hpp"
#include "RenderSystem.hpp"
#include "Sampler.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "TextOverlay.hpp"
#include "Viewport.hpp"

#include <Font.hpp>

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
			p_buffers.Fill( reinterpret_cast< uint8_t const * >( &l_vertex ), p_count * sizeof( OverlayCategory::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			return p_count;
		}

		uint32_t FillBuffers( TextOverlay::VertexArray::const_iterator p_begin, uint32_t p_count, VertexBuffer & p_buffers )
		{
			TextOverlay::Vertex const & l_vertex = *p_begin;
			p_buffers.Fill( reinterpret_cast< uint8_t const * >( &l_vertex ), p_count * sizeof( TextOverlay::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			return p_count;
		}
	}

	OverlayRenderer::OverlayRenderer( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_previousPanelZIndex( 0 )
		, m_previousBorderZIndex( 0 )
	{
		m_declaration = BufferDeclaration{
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2INTS ),
			BufferElementDeclaration( ShaderProgram::Texture, eELEMENT_USAGE_TEXCOORDS, eELEMENT_TYPE_2FLOATS )
		} };
		m_textDeclaration = BufferDeclaration{
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2INTS ),
			BufferElementDeclaration( ShaderProgram::Text, eELEMENT_USAGE_TEXCOORDS, eELEMENT_TYPE_2FLOATS, 0 ),
			BufferElementDeclaration( ShaderProgram::Texture, eELEMENT_USAGE_TEXCOORDS, eELEMENT_TYPE_2FLOATS, 1 )
		} };
		m_wp2DBlendState = GetRenderSystem()->GetEngine()->GetBlendStateManager().Create( cuT( "OVERLAY_BLEND" ) );
		m_wp2DDepthStencilState = GetRenderSystem()->GetEngine()->GetDepthStencilStateManager().Create( cuT( "OVERLAY_DS" ) );
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
			m_panelVertexBuffer = std::make_unique< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			uint32_t l_stride = m_declaration.GetStride();
			m_panelVertexBuffer->Resize( uint32_t( m_panelVertex.size() * l_stride ) );
			uint8_t * l_buffer = m_panelVertexBuffer->data();

			for ( auto && l_vertex : m_panelVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_stride;
			}

			m_panelVertexBuffer->Create();
			m_panelVertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );

			auto l_program = DoGetPanelProgram( 0 );
			m_panelGeometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_panelVertexBuffer.get(), nullptr, nullptr, nullptr );
			l_program = DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR );
			m_panelGeometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_panelVertexBuffer.get(), nullptr, nullptr, nullptr );
		}

		if ( !m_borderVertexBuffer )
		{
			// Border Overlays buffers
			m_borderVertexBuffer = std::make_unique< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			uint32_t l_stride = m_declaration.GetStride();
			m_borderVertexBuffer->Resize( uint32_t( m_borderVertex.size() * l_stride ) );
			uint8_t * l_buffer = m_borderVertexBuffer->data();

			for ( auto && l_vertex : m_borderVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_stride;
			}

			m_borderVertexBuffer->Create();
			m_borderVertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );

			auto l_program = DoGetPanelProgram( 0 );
			m_borderGeometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_borderVertexBuffer.get(), nullptr, nullptr, nullptr );
			l_program = DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR );
			m_borderGeometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_borderVertexBuffer.get(), nullptr, nullptr, nullptr );
		}

		// Create one text overlays buffer
		DoCreateTextGeometryBuffers();

		BlendStateSPtr l_blendState = m_wp2DBlendState.lock();
		l_blendState->EnableAlphaToCoverage( false );
		l_blendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
		l_blendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
		l_blendState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
		l_blendState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
		l_blendState->EnableBlend( true );
		l_blendState->Initialise();

		DepthStencilStateSPtr l_dsState = m_wp2DDepthStencilState.lock();
		l_dsState->SetDepthTest( false );
		l_dsState->Initialise();
	}

	void OverlayRenderer::Cleanup()
	{
		BlendStateSPtr l_blendState = m_wp2DBlendState.lock();
		l_blendState->Cleanup();

		DepthStencilStateSPtr l_dsState = m_wp2DDepthStencilState.lock();
		l_dsState->Cleanup();

		for ( auto && l_vertex : m_borderVertex )
		{
			l_vertex.reset();
		}

		for ( auto && l_vertex : m_panelVertex )
		{
			l_vertex.reset();
		}

		for ( auto && l_pair : m_programs )
		{
			l_pair.second->Cleanup();
		}

		m_programs.clear();
		m_mapPanelPrograms.clear();
		m_mapTextPrograms.clear();

		if ( m_panelVertexBuffer )
		{
			m_panelVertexBuffer->Cleanup();
			m_panelVertexBuffer->Destroy();
			m_panelVertexBuffer.reset();
			m_panelGeometryBuffers.m_noTexture.reset();
			m_panelGeometryBuffers.m_textured.reset();
		}

		if ( m_borderVertexBuffer )
		{
			m_borderVertexBuffer->Cleanup();
			m_borderVertexBuffer->Destroy();
			m_borderVertexBuffer.reset();
			m_borderGeometryBuffers.m_noTexture.reset();
			m_borderGeometryBuffers.m_textured.reset();
		}

		for ( auto && l_buffer : m_textsVertexBuffers )
		{
			l_buffer->Cleanup();
			l_buffer->Destroy();
			l_buffer.reset();
		}

		m_textsVertexBuffers.clear();
		m_textsGeometryBuffers.clear();
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
				auto l_it = l_arrayVtx.begin();

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
					if ( ( l_pass->GetTextureFlags() & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
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

	void OverlayRenderer::BeginRender( Size const & p_size )
	{
		if ( m_size != p_size )
		{
			m_sizeChanged = true;
			m_size = p_size;
		}

		m_wp2DBlendState.lock()->Apply();
		m_wp2DDepthStencilState.lock()->Apply();
	}

	void OverlayRenderer::EndRender()
	{
		m_sizeChanged = false;
	}

	RenderNode & OverlayRenderer::DoGetPanelProgram( Pass & p_pass )
	{
		auto && l_it = m_mapPanelPrograms.find( &p_pass );

		if ( l_it == m_mapPanelPrograms.end() )
		{
			auto l_program = DoGetPanelProgram( p_pass.GetTextureFlags() );

			auto l_sceneBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferScene );
			auto l_passBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferPass );
			Point4rFrameVariableSPtr l_pt4r;
			OneFloatFrameVariableSPtr l_1f;

			l_it = m_mapPanelPrograms.insert( { &p_pass, RenderNode{
									  p_pass,
									  *l_program,
									  *l_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix ),
									  *l_passBuffer,
									  *l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
									} } ).first;
			p_pass.BindToNode( l_it->second );
		}

		return l_it->second;
	}

	RenderNode & OverlayRenderer::DoGetTextProgram( Pass & p_pass )
	{
		auto && l_it = m_mapTextPrograms.find( &p_pass );

		if ( l_it == m_mapTextPrograms.end() )
		{
			auto l_program = DoGetTextProgram( p_pass.GetTextureFlags() );

			auto l_sceneBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferScene );
			auto l_passBuffer = l_program->FindFrameVariableBuffer( ShaderProgram::BufferPass );
			Point4rFrameVariableSPtr l_pt4r;
			OneFloatFrameVariableSPtr l_1f;

			l_it = m_mapTextPrograms.insert( { &p_pass, RenderNode{
									  p_pass,
									  *l_program,
									  *l_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix ),
									  *l_passBuffer,
									  *l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
									  *l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
									} } ).first;
			p_pass.BindToNode( l_it->second );
		}

		return l_it->second;
	}

	ShaderProgramSPtr OverlayRenderer::DoGetPanelProgram( uint32_t p_flags )
	{
		// Remove unwanted flags
		p_flags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_flags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_flags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_flags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_flags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_flags &= ~eTEXTURE_CHANNEL_HEIGHT;
		p_flags &= ~eTEXTURE_CHANNEL_EMISSIVE;

		// Get shader
		return DoGetProgram( p_flags );
	}

	ShaderProgramSPtr OverlayRenderer::DoGetTextProgram( uint32_t p_flags )
	{
		// Remove unwanted flags
		p_flags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_flags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_flags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_flags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_flags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_flags &= ~eTEXTURE_CHANNEL_HEIGHT;
		p_flags &= ~eTEXTURE_CHANNEL_EMISSIVE;
		p_flags |= eTEXTURE_CHANNEL_TEXT;

		// Get shader
		return DoGetProgram( p_flags );
	}

	ShaderProgramSPtr OverlayRenderer::DoGetProgram( uint32_t p_flags )
	{
		auto && l_it = m_programs.find( p_flags );

		if ( l_it == m_programs.end() )
		{
			// Since it does not exist yet, create it and initialise it
			auto l_program = DoCreateOverlayProgram( p_flags );

			if ( l_program )
			{
				l_program->Initialise();
				l_it = m_programs.insert( { p_flags, l_program } ).first;
			}
			else
			{
				CASTOR_EXCEPTION( "Couldn't create an overaly shader program" );
			}
		}

		return l_it->second;
	}

	OverlayRenderer::OverlayGeometryBuffers OverlayRenderer::DoCreateTextGeometryBuffers()
	{
		VertexBufferUPtr l_vertexBuffer = std::make_unique< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_textDeclaration );
		l_vertexBuffer->Resize( C3D_MAX_CHARS_PER_BUFFER * m_textDeclaration.GetStride() );
		l_vertexBuffer->Create();
		l_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );

		OverlayGeometryBuffers l_geometryBuffers;
		auto l_program = DoGetTextProgram( 0 );
		l_geometryBuffers.m_noTexture = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, l_vertexBuffer.get(), nullptr, nullptr, nullptr );
		l_program = DoGetTextProgram( eTEXTURE_CHANNEL_COLOUR );
		l_geometryBuffers.m_textured = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, l_vertexBuffer.get(), nullptr, nullptr, nullptr );

		m_textsVertexBuffers.push_back( std::move( l_vertexBuffer ) );
		m_textsGeometryBuffers.push_back( l_geometryBuffers );
		return l_geometryBuffers;
	}

	void OverlayRenderer::DoDrawItem( Pass & p_pass, GeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		RenderNode & l_node = DoGetPanelProgram( p_pass );
		p_pass.GetEngine()->GetRenderSystem()->GetPipeline().ApplyProjection( l_node.m_matrixUbo );
		p_pass.FillShaderVariables( l_node );
		l_node.m_program.Bind();
		p_pass.Render2D();
		p_geometryBuffers.Draw( p_count, 0 );
		p_pass.EndRender();
		l_node.m_program.Unbind();
	}

	void OverlayRenderer::DoDrawItem( Pass & p_pass, GeometryBuffers const & p_geometryBuffers, Texture const & p_texture, Sampler const & p_sampler , uint32_t p_count )
	{
		RenderNode & l_node = DoGetTextProgram( p_pass );
		p_pass.GetEngine()->GetRenderSystem()->GetPipeline().ApplyProjection( l_node.m_matrixUbo );

		OneIntFrameVariableSPtr l_textureVariable = l_node.m_program.FindFrameVariable( ShaderProgram::MapText, eSHADER_TYPE_PIXEL );

		if ( l_textureVariable )
		{
			l_textureVariable->SetValue( 0 );
		}

		p_pass.FillShaderVariables( l_node );
		l_node.m_program.Bind();
		p_pass.Render2D();
		p_texture.Bind( 0 );
		p_sampler.Bind( 0 );
		p_geometryBuffers.Draw( p_count, 0 );
		p_sampler.Unbind( 0 );
		p_texture.Unbind( 0 );

		p_pass.EndRender();
		l_node.m_program.Unbind();
	}

	void OverlayRenderer::DoDrawItem( Material & p_material, OverlayRenderer::OverlayGeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		for ( auto && l_pass : p_material )
		{
			if ( ( l_pass->GetTextureFlags() & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
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

	ShaderProgramSPtr OverlayRenderer::DoCreateOverlayProgram( uint32_t p_flags )
	{
#define CHECK_FLAG( flag ) ( ( p_flags & ( flag ) ) == ( flag ) )

		using namespace GLSL;

		// Shader program
		ShaderManager & l_manager = GetRenderSystem()->GetEngine()->GetShaderManager();
		ShaderProgramSPtr l_program = l_manager.GetNewProgram();
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
		l_manager.CreatePassBuffer( *l_program, MASK_SHADER_TYPE_PIXEL );

		// Vertex shader
		String l_strVs;
		{
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			auto position = l_writer.GetAttribute< IVec2 >( ShaderProgram::Position );
			auto text = l_writer.GetAttribute< Vec2 >( ShaderProgram::Text, CHECK_FLAG( eTEXTURE_CHANNEL_TEXT ) );
			auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture, CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) );

			// Shader outputs
			auto vtx_text = l_writer.GetOutput< Vec2 >( cuT( "vtx_text" ), CHECK_FLAG( eTEXTURE_CHANNEL_TEXT ) );
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ), CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				if ( CHECK_FLAG( eTEXTURE_CHANNEL_TEXT ) )
				{
					vtx_text = text;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) )
				{
					vtx_texture = texture;
				}

				gl_Position = c3d_mtxProjection * vec4( position.X, position.Y, 0.0, 1.0 );
			} );

			l_strVs = l_writer.Finalise();
		}

		// Pixel shader
		String l_strPs;
		{
			auto l_writer = GetRenderSystem()->CreateGlslWriter();

			UBO_PASS( l_writer );

			// Shader inputs
			auto vtx_text = l_writer.GetInput< Vec2 >( cuT( "vtx_text" ), CHECK_FLAG( eTEXTURE_CHANNEL_TEXT ) );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ), CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) );
			auto c3d_mapText = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapText, CHECK_FLAG( eTEXTURE_CHANNEL_TEXT ) );
			auto c3d_mapColour = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) );
			auto c3d_mapOpacity = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) );

			// Shader outputs
			auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Ambient, c3d_v4MatAmbient );
				LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_TEXT ) )
				{
					l_fAlpha *= texture2D( c3d_mapText, vec2( vtx_text.X, vtx_text.Y ) ).R;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) )
				{
					l_v4Ambient = texture2D( c3d_mapColour, vec2( vtx_texture.X, vtx_texture.Y ) );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) )
				{
					l_fAlpha *= texture2D( c3d_mapOpacity, vec2( vtx_texture.X, vtx_texture.Y ) ).R;
				}

				pxl_v4FragColor = vec4( l_v4Ambient.XYZ, l_fAlpha );
			} );

			l_strPs = l_writer.Finalise();
		}

		if ( CHECK_FLAG( eTEXTURE_CHANNEL_TEXT ) )
		{
			l_program->CreateFrameVariable( ShaderProgram::MapText, eSHADER_TYPE_PIXEL );
		}

		if ( CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) )
		{
			l_program->CreateFrameVariable( ShaderProgram::MapColour, eSHADER_TYPE_PIXEL );
		}

		if ( CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) )
		{
			l_program->CreateFrameVariable( ShaderProgram::MapOpacity, eSHADER_TYPE_PIXEL );
		}

		eSHADER_MODEL l_model = GetRenderSystem()->GetMaxShaderModel();
		l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_strVs );
		l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_strPs );

		return l_program;

#undef CHECK_FLAG
	}
}

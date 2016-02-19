#include "OverlayRenderer.hpp"

#include "BlendStateManager.hpp"
#include "BorderPanelOverlay.hpp"
#include "Buffer.hpp"
#include "DepthStencilStateManager.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "Material.hpp"
#include "MatrixFrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "Overlay.hpp"
#include "PanelOverlay.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "PointFrameVariable.hpp"
#include "RenderSystem.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "TextOverlay.hpp"
#include "Viewport.hpp"

#include <Font.hpp>

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	static const int32_t C3D_MAX_CHARS_PER_BUFFER = 6000;

	namespace
	{
		template< typename T >
		size_t count_of( T * p_buffer )
		{
			return sizeof( p_buffer ) / sizeof( p_buffer[0] );
		}
	}

	OverlayRenderer::OverlayRenderer( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_previousPanelZIndex( 0 )
		, m_previousBorderZIndex( 0 )
	{
		m_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2INTS ),
			BufferElementDeclaration( ShaderProgram::Texture, eELEMENT_USAGE_TEXCOORDS, eELEMENT_TYPE_2FLOATS )
		} );
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
			m_panelVertexBuffer->Resize( m_panelVertex.size() * l_stride );
			uint8_t * l_buffer = m_panelVertexBuffer->data();

			for ( auto && l_vertex : m_panelVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_stride;
			}

			auto l_program = DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR );
			m_panelVertexBuffer->Create();
			m_panelVertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			m_panelGeometryBuffers = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_panelVertexBuffer.get(), nullptr, nullptr, nullptr );
		}

		if ( !m_borderVertexBuffer )
		{
			// Border Overlays buffers
			m_borderVertexBuffer = std::make_unique< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			uint32_t l_stride = m_declaration.GetStride();
			m_borderVertexBuffer->Resize( m_borderVertex.size() * l_stride );
			uint8_t * l_buffer = m_borderVertexBuffer->data();

			for ( auto && l_vertex : m_borderVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_stride;
			}

			auto l_program = DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR );
			m_borderVertexBuffer->Create();
			m_borderVertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			m_borderGeometryBuffers = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_borderVertexBuffer.get(), nullptr, nullptr, nullptr );
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

		for ( auto && l_pair : m_mapPanelPrograms )
		{
			l_pair.second->Cleanup();
		}

		for ( auto && l_pair : m_mapTextPrograms )
		{
			l_pair.second->Cleanup();
		}

		m_mapPanelPrograms.clear();
		m_mapTextPrograms.clear();

		if ( m_panelVertexBuffer )
		{
			m_panelVertexBuffer->Cleanup();
			m_panelVertexBuffer->Destroy();
			m_panelVertexBuffer.reset();
			m_panelGeometryBuffers.reset();
		}

		if ( m_borderVertexBuffer )
		{
			m_borderVertexBuffer->Cleanup();
			m_borderVertexBuffer->Destroy();
			m_borderVertexBuffer.reset();
			m_borderGeometryBuffers.reset();
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

	uint32_t FillBuffers( OverlayCategory::VertexArray::const_iterator p_begin, size_t p_count, VertexBuffer & p_buffers )
	{
		OverlayCategory::Vertex const & l_vertex = *p_begin;
		p_buffers.Fill( reinterpret_cast< uint8_t const * >( &l_vertex ), p_count * sizeof( OverlayCategory::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		return p_count;
	}

	void OverlayRenderer::DrawPanel( PanelOverlay & p_overlay )
	{
		MaterialSPtr l_pMaterial = p_overlay.GetMaterial();

		if ( l_pMaterial )
		{
			DoDrawItem( *l_pMaterial, m_panelGeometryBuffers, nullptr, FillBuffers( p_overlay.GetPanelVertex().begin(), p_overlay.GetPanelVertex().size(), *m_panelVertexBuffer ) );
		}
	}

	void OverlayRenderer::DrawBorderPanel( BorderPanelOverlay & p_overlay )
	{
		MaterialSPtr l_pMaterial = p_overlay.GetMaterial();

		if ( l_pMaterial )
		{
			DoDrawItem( *l_pMaterial, m_panelGeometryBuffers, nullptr, FillBuffers( p_overlay.GetPanelVertex().begin(), p_overlay.GetPanelVertex().size(), *m_panelVertexBuffer ) );
		}

		l_pMaterial = p_overlay.GetBorderMaterial();

		if ( l_pMaterial )
		{
			DoDrawItem( *l_pMaterial, m_borderGeometryBuffers, nullptr, FillBuffers( p_overlay.GetBorderVertex().begin(), p_overlay.GetBorderVertex().size(), *m_borderVertexBuffer ) );
		}
	}

	void OverlayRenderer::DrawText( TextOverlay & p_overlay )
	{
		FontSPtr l_pFont = p_overlay.GetFontTexture()->GetFont();

		if ( l_pFont )
		{
			MaterialSPtr l_pMaterial = p_overlay.GetMaterial();

			if ( l_pMaterial )
			{
				OverlayCategory::VertexArray l_arrayVtx = p_overlay.GetTextVertex();
				int32_t l_count = l_arrayVtx.size();
				uint32_t l_index = 0;
				std::vector< GeometryBuffersSPtr > l_geometryBuffers;
				OverlayCategory::VertexArray::const_iterator l_it = l_arrayVtx.begin();

				while ( l_count > C3D_MAX_CHARS_PER_BUFFER )
				{
					l_geometryBuffers.push_back( DoFillTextPart( l_count, l_it, l_index ) );
					l_count -= C3D_MAX_CHARS_PER_BUFFER;
				}

				if ( l_count > 0 )
				{
					l_geometryBuffers.push_back( DoFillTextPart( l_count, l_it, l_index ) );
				}

				TextureSPtr l_texture = p_overlay.GetFontTexture()->GetTexture();
				l_count = l_arrayVtx.size();

				for ( auto l_geoBuffers : l_geometryBuffers )
				{
					DoDrawItem( *l_pMaterial, l_geoBuffers, l_texture, std::min( l_count, C3D_MAX_CHARS_PER_BUFFER ) );
					l_count -= C3D_MAX_CHARS_PER_BUFFER;
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
		return DoGetProgram( p_flags, m_mapPanelPrograms );
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
		return DoGetProgram( p_flags, m_mapTextPrograms );
	}

	ShaderProgramSPtr OverlayRenderer::DoGetProgram( uint32_t p_flags, std::map< uint32_t, ShaderProgramSPtr > & p_programs )
	{
		auto && l_it = p_programs.find( p_flags );
		ShaderProgramSPtr l_return;

		if ( l_it != p_programs.end() )
		{
			l_return = l_it->second;
		}
		else
		{
			// Since it does not exist yet, create it and initialise it
			l_return = GetRenderSystem()->CreateOverlayProgram( p_flags );

			if ( l_return )
			{
				l_return->Initialise();
				p_programs.insert( std::make_pair( p_flags, l_return ) );
			}
		}

		return l_return;
	}

	GeometryBuffersSPtr OverlayRenderer::DoCreateTextGeometryBuffers()
	{
		VertexBufferUPtr l_vertexBuffer = std::make_unique< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
		l_vertexBuffer->Resize( C3D_MAX_CHARS_PER_BUFFER * m_declaration.GetStride() );
		auto l_program = DoGetTextProgram( eTEXTURE_CHANNEL_COLOUR );

		l_vertexBuffer->Create();
		l_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		GeometryBuffersSPtr l_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, l_vertexBuffer.get(), nullptr, nullptr, nullptr );

		m_textsVertexBuffers.push_back( std::move( l_vertexBuffer ) );
		m_textsGeometryBuffers.push_back( l_geometryBuffers );
		return l_geometryBuffers;
	}

	void OverlayRenderer::DoDrawItem( Material & p_material, GeometryBuffersSPtr p_geometryBuffers, TextureSPtr p_texture, uint32_t p_count )
	{
		for ( auto && l_pass : p_material )
		{
			ShaderProgramSPtr l_program;

			// The texture can only contain the font data, so this test is valid. :P
			if ( p_texture )
			{
				l_program = DoGetTextProgram( l_pass->GetTextureFlags() );
			}
			else
			{
				l_program = DoGetPanelProgram( l_pass->GetTextureFlags() );
			}

			if ( l_program )
			{
				l_pass->BindToAutomaticProgram( l_program );
				auto l_matrixBuffer = l_pass->GetMatrixBuffer();

				if ( l_matrixBuffer )
				{
					p_material.GetEngine()->GetRenderSystem()->GetPipeline().ApplyProjection( *l_matrixBuffer );
				}

				if ( p_texture )
				{
					OneTextureFrameVariableSPtr l_textureVariable = l_program->FindFrameVariable( ShaderProgram::MapText, eSHADER_TYPE_PIXEL );

					if ( l_textureVariable )
					{
						l_textureVariable->SetValue( p_texture.get() );
					}

					l_pass->Render2D();
					p_texture->BindAt( 0 );
					p_geometryBuffers->Draw( p_count, 0 );
					p_texture->UnbindFrom( 0 );
				}
				else
				{
					l_pass->Render2D();
					p_geometryBuffers->Draw( p_count, 0 );
				}

				l_pass->EndRender();
			}
		}
	}

	GeometryBuffersSPtr OverlayRenderer::DoFillTextPart( int32_t p_count, OverlayCategory::VertexArray::const_iterator & p_it, uint32_t & p_index )
	{
		GeometryBuffersSPtr l_geometryBuffers;

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
}

#include "OverlayRenderer.hpp"

#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "MatrixFrameVariable.hpp"
#include "Overlay.hpp"
#include "PanelOverlay.hpp"
#include "BorderPanelOverlay.hpp"
#include "TextOverlay.hpp"
#include "ShaderProgram.hpp"
#include "ShaderManager.hpp"
#include "Material.hpp"
#include "Buffer.hpp"
#include "Viewport.hpp"
#include "Pass.hpp"
#include "RenderSystem.hpp"
#include "Pipeline.hpp"
#include "BlendState.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"
#include "FrameVariableBuffer.hpp"
#include "DynamicTexture.hpp"

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

	OverlayRenderer::OverlayRenderer( RenderSystem * p_pRenderSystem )
		: m_renderSystem( p_pRenderSystem )
		, m_previousPanelZIndex( 0 )
		, m_previousBorderZIndex( 0 )
	{
		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2INTS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};

		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
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

		m_pDeclaration.reset();
	}

	void OverlayRenderer::Initialise()
	{
		if ( !m_pPanelGeometryBuffer )
		{
			// Panel Overlays buffers
			VertexBufferUPtr l_pPanelVtxBuffer = std::make_unique< VertexBuffer >( m_renderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
			uint32_t l_uiStride = m_pDeclaration->GetStride();
			l_pPanelVtxBuffer->Resize( m_panelVertex.size() * l_uiStride );
			uint8_t * l_buffer = l_pPanelVtxBuffer->data();

			for ( auto && l_vertex : m_panelVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_uiStride;
			}

			m_pPanelGeometryBuffer = m_renderSystem->CreateGeometryBuffers( std::move( l_pPanelVtxBuffer ), nullptr, nullptr );
			m_pPanelGeometryBuffer->Create();
			m_pPanelGeometryBuffer->Initialise( DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}

		if ( !m_pBorderGeometryBuffer )
		{
			// Border Overlays buffers
			VertexBufferUPtr l_pBorderVtxBuffer = std::make_unique< VertexBuffer >( m_renderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
			uint32_t l_uiStride = m_pDeclaration->GetStride();
			l_pBorderVtxBuffer->Resize( m_borderVertex.size() * l_uiStride );
			uint8_t * l_buffer = l_pBorderVtxBuffer->data();

			for ( auto && l_vertex : m_borderVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_uiStride;
			}

			m_pBorderGeometryBuffer = m_renderSystem->CreateGeometryBuffers( std::move( l_pBorderVtxBuffer ), nullptr, nullptr );
			m_pBorderGeometryBuffer->Create();
			m_pBorderGeometryBuffer->Initialise( DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}

		// Create one text overlays buffer
		DoCreateTextGeometryBuffers();

		// Driver specific
		DoInitialise();
	}

	void OverlayRenderer::Cleanup()
	{
		DoCleanup();

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

		if ( m_pPanelGeometryBuffer )
		{
			m_pPanelGeometryBuffer->Cleanup();
			m_pPanelGeometryBuffer->Destroy();
			m_pPanelGeometryBuffer.reset();
		}

		if ( m_pBorderGeometryBuffer )
		{
			m_pBorderGeometryBuffer->Cleanup();
			m_pBorderGeometryBuffer->Destroy();
			m_pBorderGeometryBuffer.reset();
		}

		for ( auto && l_geometryBuffers : m_pTextsGeometryBuffers )
		{
			l_geometryBuffers->Cleanup();
			l_geometryBuffers->Destroy();
			l_geometryBuffers.reset();
		}

		m_pTextsGeometryBuffers.clear();
	}

	uint32_t FillBuffers( OverlayCategory::VertexArray::const_iterator p_begin, size_t p_count, GeometryBuffers & p_buffers )
	{
		OverlayCategory::Vertex const & l_vertex = *p_begin;
		p_buffers.GetVertexBuffer().Fill( reinterpret_cast< uint8_t const * >( &l_vertex ), p_count * sizeof( OverlayCategory::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		return p_count;
	}

	void OverlayRenderer::DrawPanel( PanelOverlay & p_overlay )
	{
		MaterialSPtr l_pMaterial = p_overlay.GetMaterial();

		if ( l_pMaterial )
		{
			DoDrawItem( *l_pMaterial, m_pPanelGeometryBuffer, nullptr, FillBuffers( p_overlay.GetPanelVertex().begin(), p_overlay.GetPanelVertex().size(), *m_pPanelGeometryBuffer ) );
		}
	}

	void OverlayRenderer::DrawBorderPanel( BorderPanelOverlay & p_overlay )
	{
		MaterialSPtr l_pMaterial = p_overlay.GetMaterial();

		if ( l_pMaterial )
		{
			DoDrawItem( *l_pMaterial, m_pPanelGeometryBuffer, nullptr, FillBuffers( p_overlay.GetPanelVertex().begin(), p_overlay.GetPanelVertex().size(), *m_pPanelGeometryBuffer ) );
		}

		l_pMaterial = p_overlay.GetBorderMaterial();

		if ( l_pMaterial )
		{
			DoDrawItem( *l_pMaterial, m_pBorderGeometryBuffer, nullptr, FillBuffers( p_overlay.GetBorderVertex().begin(), p_overlay.GetBorderVertex().size(), *m_pBorderGeometryBuffer ) );
		}
	}

	void OverlayRenderer::DrawText( TextOverlay & p_overlay )
	{
		FontSPtr l_pFont = p_overlay.GetFont();

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

				DynamicTextureSPtr l_texture = p_overlay.GetTexture();
				l_count = l_arrayVtx.size();

				for ( auto l_vtxBuffer : l_geometryBuffers )
				{
					DoDrawItem( *l_pMaterial, l_vtxBuffer, l_texture, std::min( l_count, C3D_MAX_CHARS_PER_BUFFER ) );
					l_count -= C3D_MAX_CHARS_PER_BUFFER;
				}
			}
		}
	}

	void OverlayRenderer::BeginRender( Size const & p_size, Matrix4x4r const & p_projection )
	{
		m_size = p_size;
		m_projection = p_projection;
	}

	void OverlayRenderer::EndRender()
	{
	}

	ShaderProgramBaseSPtr OverlayRenderer::DoGetPanelProgram( uint32_t p_uiFlags )
	{
		// Remove unwanted flags
		p_uiFlags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_uiFlags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_uiFlags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_uiFlags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_uiFlags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_uiFlags &= ~eTEXTURE_CHANNEL_HEIGHT;

		// Get shader
		return DoGetProgram( p_uiFlags, m_mapPanelPrograms );
	}

	ShaderProgramBaseSPtr OverlayRenderer::DoGetTextProgram( uint32_t p_uiFlags )
	{
		// Remove unwanted flags
		p_uiFlags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_uiFlags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_uiFlags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_uiFlags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_uiFlags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_uiFlags &= ~eTEXTURE_CHANNEL_HEIGHT;
		p_uiFlags |= eTEXTURE_CHANNEL_TEXT;

		// Get shader
		return DoGetProgram( p_uiFlags, m_mapTextPrograms );
	}

	ShaderProgramBaseSPtr OverlayRenderer::DoGetProgram( uint32_t p_uiFlags, std::map< uint32_t, ShaderProgramBaseSPtr > & p_programs )
	{
		auto && l_it = p_programs.find( p_uiFlags );
		ShaderProgramBaseSPtr l_pReturn;

		if ( l_it != p_programs.end() )
		{
			l_pReturn = l_it->second;
		}
		else
		{
			// Since it does not exist yet, create it and initialise it
			l_pReturn = DoCreateProgram( p_uiFlags );

			if ( l_pReturn )
			{
				l_pReturn->Initialise();
				p_programs.insert( std::make_pair( p_uiFlags, l_pReturn ) );
			}
		}

		return l_pReturn;
	}

	GeometryBuffersSPtr OverlayRenderer::DoCreateTextGeometryBuffers()
	{
		VertexBufferUPtr l_pTextVtxBuffer = std::make_unique< VertexBuffer >( m_renderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		l_pTextVtxBuffer->Resize( C3D_MAX_CHARS_PER_BUFFER * m_pDeclaration->GetStride() );

		GeometryBuffersSPtr l_geometryBuffers = m_renderSystem->CreateGeometryBuffers( std::move( l_pTextVtxBuffer ), nullptr, nullptr );
		l_geometryBuffers->Create();
		l_geometryBuffers->Initialise( DoGetTextProgram( eTEXTURE_CHANNEL_COLOUR ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );

		m_pTextsGeometryBuffers.push_back( l_geometryBuffers );
		return l_geometryBuffers;
	}

	void OverlayRenderer::DoDrawItem( Material & p_material, GeometryBuffersSPtr p_geometryBuffers, DynamicTextureSPtr p_texture, uint32_t p_count )
	{
		uint8_t l_byIndex = 0;
		uint8_t l_byCount = uint8_t( p_material.GetPassCount() );

		for ( auto && l_pass : p_material )
		{
			ShaderProgramBaseSPtr l_program;

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
					OneTextureFrameVariableSPtr l_textureVariable = l_program->FindFrameVariable( ShaderProgramBase::MapText, eSHADER_TYPE_PIXEL );

					if ( l_textureVariable )
					{
						l_textureVariable->SetValue( p_texture.get() );
					}

					l_pass->Render2D( l_byIndex++, l_byCount );
					p_texture->BindAt( 0 );
					p_geometryBuffers->Draw( eTOPOLOGY_TRIANGLES, l_program, p_count, 0 );
					p_texture->UnbindFrom( 0 );
				}
				else
				{
					l_pass->Render2D( l_byIndex++, l_byCount );
					p_geometryBuffers->Draw( eTOPOLOGY_TRIANGLES, l_program, p_count, 0 );
				}

				l_pass->EndRender();
			}
		}
	}

	GeometryBuffersSPtr OverlayRenderer::DoFillTextPart( int32_t p_count, OverlayCategory::VertexArray::const_iterator & p_it, uint32_t & p_index )
	{
		GeometryBuffersSPtr l_geometryBuffers;

		if ( m_pTextsGeometryBuffers.size() <= p_index )
		{
			l_geometryBuffers = DoCreateTextGeometryBuffers();
		}
		else
		{
			l_geometryBuffers = m_pTextsGeometryBuffers[p_index++];
		}

		p_count = std::min( p_count, C3D_MAX_CHARS_PER_BUFFER );
		FillBuffers( p_it, p_count, *l_geometryBuffers );
		p_it += p_count;
		return l_geometryBuffers;
	}
}

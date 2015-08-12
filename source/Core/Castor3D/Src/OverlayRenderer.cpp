#include "OverlayRenderer.hpp"
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
	namespace
	{
		template< typename T >
		size_t count_of( T * p_buffer )
		{
			return sizeof( p_buffer ) / sizeof( p_buffer[0] );
		}
	}

	OverlayRenderer::OverlayRenderer( RenderSystem * p_pRenderSystem )
		: m_pRenderSystem( p_pRenderSystem )
		, m_previousPanelZIndex( 0 )
		, m_previousBorderZIndex( 0 )
	{
		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_3INTS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};

		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
		
		std::memset( m_panelBuffer, 0, 60 );
		uint32_t i = 0;
		std::for_each( m_panelVertex.begin(), m_panelVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
		{
			p_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_panelBuffer )[i++ * m_pDeclaration->GetStride()] );
		} );
	}

	OverlayRenderer::~OverlayRenderer()
	{
		std::for_each( m_panelVertex.begin(), m_panelVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
		{
			p_vertex.reset();
		} );
		m_pDeclaration.reset();
	}

	void OverlayRenderer::Initialise()
	{
		if ( !m_pPanelGeometryBuffer )
		{
			// Panel Overlays
			VertexBufferUPtr l_pPanelVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
			uint32_t l_uiStride = m_pDeclaration->GetStride();
			l_pPanelVtxBuffer->Resize( 6 * l_uiStride );
			m_panelVertex[0]->LinkCoords( &l_pPanelVtxBuffer->data()[0 * l_uiStride], l_uiStride );
			m_panelVertex[1]->LinkCoords( &l_pPanelVtxBuffer->data()[1 * l_uiStride], l_uiStride );
			m_panelVertex[2]->LinkCoords( &l_pPanelVtxBuffer->data()[2 * l_uiStride], l_uiStride );
			m_panelVertex[3]->LinkCoords( &l_pPanelVtxBuffer->data()[3 * l_uiStride], l_uiStride );
			m_panelVertex[4]->LinkCoords( &l_pPanelVtxBuffer->data()[4 * l_uiStride], l_uiStride );
			m_panelVertex[5]->LinkCoords( &l_pPanelVtxBuffer->data()[5 * l_uiStride], l_uiStride );
			l_pPanelVtxBuffer->Create();
			l_pPanelVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, GetPanelProgram( eTEXTURE_CHANNEL_COLOUR ) );
			m_pPanelGeometryBuffer = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pPanelVtxBuffer ), nullptr, nullptr );
			m_pPanelGeometryBuffer->Initialise();
		}

		if ( !m_pBorderGeometryBuffer )
		{
			// Border Overlays
			VertexBufferUPtr l_pBorderVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
			m_pBorderGeometryBuffer = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pBorderVtxBuffer ), nullptr, nullptr );
			m_pBorderGeometryBuffer->GetVertexBuffer().Create();
			m_pBorderGeometryBuffer->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, GetPanelProgram( eTEXTURE_CHANNEL_COLOUR ) );
			m_pBorderGeometryBuffer->Initialise();
		}

		if ( !m_pTextGeometryBuffers )
		{
			// Text Overlays
			VertexBufferUPtr l_pTextVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
			m_pTextGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pTextVtxBuffer ), nullptr, nullptr );
			m_pTextGeometryBuffers->GetVertexBuffer().Create();
			m_pTextGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, GetTextProgram( eTEXTURE_CHANNEL_COLOUR ) );
			m_pTextGeometryBuffers->Initialise();

			// Driver specific
			DoInitialise();
		}
	}

	void OverlayRenderer::Cleanup()
	{
		DoCleanup();

		std::for_each( m_mapPanelPrograms.begin(), m_mapPanelPrograms.end(), [&]( std::pair< uint32_t, ShaderProgramBaseSPtr > p_pair )
		{
			if ( p_pair.second )
			{
				p_pair.second->Cleanup();
			}
		} );

		std::for_each( m_mapTextPrograms.begin(), m_mapTextPrograms.end(), [&]( std::pair< uint32_t, ShaderProgramBaseSPtr > p_pair )
		{
			if ( p_pair.second )
			{
				p_pair.second->Cleanup();
			}
		} );

		m_mapPanelPrograms.clear();
		m_mapTextPrograms.clear();

		if ( m_pPanelGeometryBuffer )
		{
			m_pPanelGeometryBuffer->GetVertexBuffer().Cleanup();
			m_pPanelGeometryBuffer->Cleanup();
			m_pPanelGeometryBuffer.reset();
		}

		if ( m_pBorderGeometryBuffer )
		{
			m_pBorderGeometryBuffer->GetVertexBuffer().Cleanup();
			m_pBorderGeometryBuffer->Cleanup();
			m_pBorderGeometryBuffer.reset();
		}

		if ( m_pTextGeometryBuffers )
		{
			m_pTextGeometryBuffers->GetVertexBuffer().Cleanup();
			m_pTextGeometryBuffers->Cleanup();
			m_pTextGeometryBuffers.reset();
		}
	}

	void OverlayRenderer::DrawPanel( PanelOverlay & p_panelOverlay )
	{
		MaterialSPtr l_pMaterial = p_panelOverlay.GetMaterial();
		int iZIndex = 1000 - p_panelOverlay.GetOverlay().GetZIndex();

		if ( l_pMaterial )
		{
			std::vector< OverlayCategory::Vertex > const & l_arrayVtx = p_panelOverlay.GetVertexBuffer();
			uint32_t l_count = uint32_t( l_arrayVtx.size() );

			if ( m_previousPanelZIndex != iZIndex )
			{
				m_previousPanelZIndex = iZIndex;
				VertexBuffer & l_vtxBuffer = m_pPanelGeometryBuffer->GetVertexBuffer();

				if ( l_vtxBuffer.Bind() )
				{
					l_vtxBuffer.Fill( reinterpret_cast< uint8_t const * >( l_arrayVtx.data() ), l_count * sizeof( OverlayCategory::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
					l_vtxBuffer.Unbind();
				}
			}

			uint8_t l_byIndex = 0;
			uint8_t l_byCount = uint8_t( l_pMaterial->GetPassCount() );

			std::for_each( l_pMaterial->Begin(), l_pMaterial->End(), [&]( PassSPtr p_pPass )
			{
				ShaderProgramBaseSPtr l_pProgram = GetPanelProgram( p_pPass->GetTextureFlags() );

				if ( l_pProgram )
				{
					p_pPass->BindToProgram( l_pProgram );
					auto l_matrixBuffer = p_pPass->GetMatrixBuffer();

					if ( l_matrixBuffer )
					{
						m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_matrixBuffer );
					}

					p_pPass->Render2D( l_byIndex++, l_byCount );
					m_pPanelGeometryBuffer->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, l_count, 0 );
					p_pPass->EndRender();
				}
			} );
		}
	}

	void OverlayRenderer::DrawBorderPanel( BorderPanelOverlay & p_borderPanelOverlay )
	{
		MaterialSPtr l_pMaterial = p_borderPanelOverlay.GetMaterial();
		int iZIndex = 1000 - p_borderPanelOverlay.GetOverlay().GetZIndex();

		if ( l_pMaterial )
		{
			std::vector< OverlayCategory::Vertex > const & l_arrayVtx = p_borderPanelOverlay.GetVertexBuffer();
			uint32_t l_count = uint32_t( l_arrayVtx.size() );

			if ( m_previousPanelZIndex != iZIndex )
			{
				m_previousPanelZIndex = iZIndex;
				VertexBuffer & l_vtxBuffer = m_pPanelGeometryBuffer->GetVertexBuffer();

				if ( l_vtxBuffer.Bind() )
				{
					l_vtxBuffer.Fill( reinterpret_cast< uint8_t const * >( l_arrayVtx.data() ), l_count * sizeof( OverlayCategory::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
					l_vtxBuffer.Unbind();
				}
			}

			uint8_t l_byIndex = 0;
			uint8_t l_byCount = uint8_t( l_pMaterial->GetPassCount() );

			std::for_each( l_pMaterial->Begin(), l_pMaterial->End(), [&]( PassSPtr p_pPass )
			{
				ShaderProgramBaseSPtr l_pProgram = GetPanelProgram( p_pPass->GetTextureFlags() );

				if ( l_pProgram )
				{
					p_pPass->BindToProgram( l_pProgram );
					auto l_matrixBuffer = p_pPass->GetMatrixBuffer();

					if ( l_matrixBuffer )
					{
						m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_matrixBuffer );
					}

					p_pPass->Render2D( l_byIndex++, l_byCount );
					m_pPanelGeometryBuffer->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, l_count, 0 );
					p_pPass->EndRender();
				}
			} );
		}

		l_pMaterial = p_borderPanelOverlay.GetBorderMaterial();

		if ( l_pMaterial )
		{
			std::vector< OverlayCategory::Vertex > const & l_arrayVtx = p_borderPanelOverlay.GetBorderVertexBuffer();
			uint32_t l_count = uint32_t( l_arrayVtx.size() );

			if ( m_previousBorderZIndex != iZIndex )
			{
				m_previousBorderZIndex = iZIndex;
				VertexBuffer & l_vtxBuffer = m_pBorderGeometryBuffer->GetVertexBuffer();

				if ( l_vtxBuffer.Bind() )
				{
					l_vtxBuffer.Fill( reinterpret_cast< uint8_t const * >( l_arrayVtx.data() ), l_count * sizeof( OverlayCategory::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
					l_vtxBuffer.Unbind();
				}
			}

			uint8_t l_byIndex = 0;
			uint8_t l_byCount = uint8_t( l_pMaterial->GetPassCount() );

			std::for_each( l_pMaterial->Begin(), l_pMaterial->End(), [&]( PassSPtr p_pPass )
			{
				ShaderProgramBaseSPtr l_pProgram = GetPanelProgram( p_pPass->GetTextureFlags() );

				if ( l_pProgram )
				{
					p_pPass->BindToProgram( l_pProgram );
					auto l_matrixBuffer = p_pPass->GetMatrixBuffer();

					if ( l_matrixBuffer )
					{
						m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_matrixBuffer );
					}

					p_pPass->Render2D( l_byIndex++, l_byCount );
					m_pBorderGeometryBuffer->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, l_count, 0 );
					p_pPass->EndRender();
				}
			} );
		}
	}

	void OverlayRenderer::DrawText( TextOverlay & p_textOverlay )
	{
		FontSPtr l_pFont = p_textOverlay.GetFont();
		int iZIndex = 1000 - p_textOverlay.GetOverlay().GetZIndex();

		if ( l_pFont )
		{
			MaterialSPtr l_pMaterial = p_textOverlay.GetMaterial();
			VertexBuffer & l_vtxBuffer = m_pTextGeometryBuffers->GetVertexBuffer();
			DynamicTextureSPtr l_texture = p_textOverlay.GetTexture();

			if ( l_pMaterial )
			{
				std::vector< OverlayCategory::Vertex > const & l_arrayVtx = p_textOverlay.GetVertexBuffer();
				uint32_t l_count = uint32_t( l_arrayVtx.size() );

				if ( m_previousCaption != p_textOverlay.GetCaption() || m_previousTextZIndex != iZIndex )
				{
					m_previousTextZIndex = iZIndex;
					m_previousCaption = p_textOverlay.GetCaption();

					if ( l_vtxBuffer.Bind() )
					{
						l_vtxBuffer.Fill( reinterpret_cast< uint8_t const * >( l_arrayVtx.data() ), l_count * sizeof( OverlayCategory::Vertex ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
						l_vtxBuffer.Unbind();
					}
				}

				uint8_t l_byIndex = 0;
				uint8_t l_byCount = uint8_t( l_pMaterial->GetPassCount() );

				std::for_each( l_pMaterial->Begin(), l_pMaterial->End(), [&]( PassSPtr p_pPass )
				{
					ShaderProgramBaseSPtr l_pProgram = GetTextProgram( p_pPass->GetTextureFlags() );

					if ( l_pProgram )
					{
						if ( l_vtxBuffer.Bind() )
						{
							l_vtxBuffer.Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
							l_vtxBuffer.Unbind();
							p_pPass->BindToProgram( l_pProgram );
							auto l_matrixBuffer = p_pPass->GetMatrixBuffer();

							if ( l_matrixBuffer )
							{
								m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_matrixBuffer );
							}

							p_pPass->Render2D( l_byIndex++, l_byCount );
							l_texture->Bind();
							m_pTextGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, l_count, 0 );
							l_texture->Unbind();
							p_pPass->EndRender();
						}
					}
				} );
			}
		}
	}

	void OverlayRenderer::BeginRender( Size const & p_size )
	{
		m_size = p_size;
	}

	void OverlayRenderer::EndRender()
	{
	}

	ShaderProgramBaseSPtr OverlayRenderer::GetPanelProgram( uint32_t p_uiFlags )
	{
		p_uiFlags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_uiFlags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_uiFlags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_uiFlags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_uiFlags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_uiFlags &= ~eTEXTURE_CHANNEL_HEIGHT;
		p_uiFlags &= ~eTEXTURE_CHANNEL_LGHTPASS;
		std::map< uint32_t, ShaderProgramBaseSPtr >::const_iterator l_it = m_mapPanelPrograms.find( p_uiFlags );
		ShaderProgramBaseSPtr l_pReturn;

		if ( l_it != m_mapPanelPrograms.end() )
		{
			l_pReturn = l_it->second;
		}
		else
		{
			l_pReturn = DoGetProgram( p_uiFlags );

			if ( l_pReturn )
			{
				l_pReturn->Initialise();
				m_mapPanelPrograms.insert( std::make_pair( p_uiFlags, l_pReturn ) );
			}
		}

		return l_pReturn;
	}

	ShaderProgramBaseSPtr OverlayRenderer::GetTextProgram( uint32_t p_uiFlags )
	{
		p_uiFlags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_uiFlags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_uiFlags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_uiFlags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_uiFlags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_uiFlags &= ~eTEXTURE_CHANNEL_HEIGHT;
		p_uiFlags &= ~eTEXTURE_CHANNEL_LGHTPASS;
		p_uiFlags |= eTEXTURE_CHANNEL_TEXT;
		std::map< uint32_t, ShaderProgramBaseSPtr >::const_iterator l_it = m_mapTextPrograms.find( p_uiFlags );
		ShaderProgramBaseSPtr l_pReturn;

		if ( l_it != m_mapTextPrograms.end() )
		{
			l_pReturn = l_it->second;
		}
		else
		{
			l_pReturn = DoGetProgram( p_uiFlags );

			if ( l_pReturn )
			{
				l_pReturn->Initialise();
				m_mapTextPrograms.insert( std::make_pair( p_uiFlags, l_pReturn ) );
			}
		}

		return l_pReturn;
	}
}

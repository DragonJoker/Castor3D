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
		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2INTS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};

		m_declaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
		m_wp2DBlendState = GetOwner()->GetOwner()->GetBlendStateManager().Create( cuT( "OVERLAY_BLEND" ) );
		m_wp2DDepthStencilState = GetOwner()->GetOwner()->GetDepthStencilStateManager().Create( cuT( "OVERLAY_DS" ) );
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

		m_declaration.reset();
	}

	void OverlayRenderer::Initialise()
	{
		if ( !m_pPanelGeometryBuffer )
		{
			// Panel Overlays buffers
			VertexBufferUPtr l_pPanelVtxBuffer = std::make_unique< VertexBuffer >( *GetOwner()->GetOwner(), &( *m_declaration )[0], m_declaration->Size() );
			uint32_t l_uiStride = m_declaration->GetStride();
			l_pPanelVtxBuffer->Resize( m_panelVertex.size() * l_uiStride );
			uint8_t * l_buffer = l_pPanelVtxBuffer->data();

			for ( auto && l_vertex : m_panelVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_uiStride;
			}

			m_pPanelGeometryBuffer = GetOwner()->CreateGeometryBuffers( std::move( l_pPanelVtxBuffer ), nullptr, nullptr, eTOPOLOGY_TRIANGLES );
			m_pPanelGeometryBuffer->Create();
			m_pPanelGeometryBuffer->Initialise( DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}

		if ( !m_pBorderGeometryBuffer )
		{
			// Border Overlays buffers
			VertexBufferUPtr l_pBorderVtxBuffer = std::make_unique< VertexBuffer >( *GetOwner()->GetOwner(), &( *m_declaration )[0], m_declaration->Size() );
			uint32_t l_uiStride = m_declaration->GetStride();
			l_pBorderVtxBuffer->Resize( m_borderVertex.size() * l_uiStride );
			uint8_t * l_buffer = l_pBorderVtxBuffer->data();

			for ( auto && l_vertex : m_borderVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
				l_buffer += l_uiStride;
			}

			m_pBorderGeometryBuffer = GetOwner()->CreateGeometryBuffers( std::move( l_pBorderVtxBuffer ), nullptr, nullptr, eTOPOLOGY_TRIANGLES );
			m_pBorderGeometryBuffer->Create();
			m_pBorderGeometryBuffer->Initialise( DoGetPanelProgram( eTEXTURE_CHANNEL_COLOUR ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
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

				TextureBaseSPtr l_texture = p_overlay.GetFontTexture()->GetTexture();
				l_count = l_arrayVtx.size();

				for ( auto l_vtxBuffer : l_geometryBuffers )
				{
					DoDrawItem( *l_pMaterial, l_vtxBuffer, l_texture, std::min( l_count, C3D_MAX_CHARS_PER_BUFFER ) );
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

	ShaderProgramBaseSPtr OverlayRenderer::DoGetPanelProgram( uint32_t p_flags )
	{
		// Remove unwanted flags
		p_flags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_flags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_flags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_flags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_flags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_flags &= ~eTEXTURE_CHANNEL_HEIGHT;

		// Get shader
		return DoGetProgram( p_flags, m_mapPanelPrograms );
	}

	ShaderProgramBaseSPtr OverlayRenderer::DoGetTextProgram( uint32_t p_flags )
	{
		// Remove unwanted flags
		p_flags &= ~eTEXTURE_CHANNEL_AMBIENT;
		p_flags &= ~eTEXTURE_CHANNEL_DIFFUSE;
		p_flags &= ~eTEXTURE_CHANNEL_NORMAL;
		p_flags &= ~eTEXTURE_CHANNEL_SPECULAR;
		p_flags &= ~eTEXTURE_CHANNEL_GLOSS;
		p_flags &= ~eTEXTURE_CHANNEL_HEIGHT;
		p_flags |= eTEXTURE_CHANNEL_TEXT;

		// Get shader
		return DoGetProgram( p_flags, m_mapTextPrograms );
	}

	ShaderProgramBaseSPtr OverlayRenderer::DoGetProgram( uint32_t p_flags, std::map< uint32_t, ShaderProgramBaseSPtr > & p_programs )
	{
		auto && l_it = p_programs.find( p_flags );
		ShaderProgramBaseSPtr l_return;

		if ( l_it != p_programs.end() )
		{
			l_return = l_it->second;
		}
		else
		{
			// Since it does not exist yet, create it and initialise it
			l_return = GetOwner()->CreateOverlayProgram( p_flags );

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
		VertexBufferUPtr l_pTextVtxBuffer = std::make_unique< VertexBuffer >( *GetOwner()->GetOwner(), &( *m_declaration )[0], m_declaration->Size() );
		l_pTextVtxBuffer->Resize( C3D_MAX_CHARS_PER_BUFFER * m_declaration->GetStride() );

		GeometryBuffersSPtr l_geometryBuffers = GetOwner()->CreateGeometryBuffers( std::move( l_pTextVtxBuffer ), nullptr, nullptr, eTOPOLOGY_TRIANGLES );
		l_geometryBuffers->Create();
		l_geometryBuffers->Initialise( DoGetTextProgram( eTEXTURE_CHANNEL_COLOUR ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );

		m_pTextsGeometryBuffers.push_back( l_geometryBuffers );
		return l_geometryBuffers;
	}

	void OverlayRenderer::DoDrawItem( Material & p_material, GeometryBuffersSPtr p_geometryBuffers, TextureBaseSPtr p_texture, uint32_t p_count )
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
					p_material.GetOwner()->GetRenderSystem()->GetPipeline().ApplyProjection( *l_matrixBuffer );
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
					p_geometryBuffers->Draw( l_program, p_count, 0 );
					p_texture->UnbindFrom( 0 );
				}
				else
				{
					l_pass->Render2D( l_byIndex++, l_byCount );
					p_geometryBuffers->Draw( l_program, p_count, 0 );
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

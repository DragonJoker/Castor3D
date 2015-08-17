#include "FrameBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "RenderBuffer.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "RenderSystem.hpp"
#include "Context.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	FrameBuffer::FrameBuffer( Engine * p_pEngine )
		:	m_pEngine( p_pEngine )
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	bool FrameBuffer::Bind( eFRAMEBUFFER_MODE p_eMode, eFRAMEBUFFER_TARGET p_eTarget )
	{
		bool l_bReturn = DoBind( p_eTarget );

		if ( l_bReturn && m_arrayAttaches.size() && p_eMode == eFRAMEBUFFER_MODE_AUTOMATIC )
		{
			SetDrawBuffers( m_arrayAttaches );
		}

		return l_bReturn;
	}

	void FrameBuffer::Unbind()
	{
		DoUnbind();
	}

	void FrameBuffer::Attach( RenderBufferAttachmentRPtr p_pAttach )
	{
		DoAttach( p_pAttach );// the renderer virtual function
		DoRboAttach( p_pAttach->GetAttachmentPoint(), p_pAttach->GetRenderBuffer() );
	}

	void FrameBuffer::Detach( RenderBufferAttachmentRPtr p_pAttach )
	{
		DoDetach( p_pAttach->GetAttachmentPoint() );
		DoDetach( p_pAttach );// the renderer virtual function
	}

	void FrameBuffer::Attach( TextureAttachmentRPtr p_pAttach )
	{
		DoAttach( p_pAttach );// the renderer virtual function
		DoTexAttach( p_pAttach->GetAttachmentPoint(), p_pAttach->GetTexture() );
	}

	void FrameBuffer::Detach( TextureAttachmentRPtr p_pAttach )
	{
		DoDetach( p_pAttach->GetAttachmentPoint() );
		DoDetach( p_pAttach );// the renderer virtual function
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_eAttachment, DynamicTextureSPtr p_pTexture, eTEXTURE_TARGET p_eTarget, int p_iLayer )
	{
		bool l_bReturn = DoAttach( p_eAttachment, p_pTexture, p_eTarget, p_iLayer );

		if ( l_bReturn )
		{
			DoTexAttach( p_eAttachment, p_pTexture );
		}

		return l_bReturn;
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_eAttachment, RenderBufferSPtr p_pRenderBuffer )
	{
		bool l_bReturn = DoAttach( p_eAttachment, p_pRenderBuffer );

		if ( l_bReturn )
		{
			DoRboAttach( p_eAttachment, p_pRenderBuffer );
		}

		return l_bReturn;
	}

	void FrameBuffer::DetachAll()
	{
		DoDetachAll();
		m_arrayAttaches.clear();
		m_mapRbo.clear();
		m_mapTex.clear();
	}

	void FrameBuffer::Resize( Castor::Size const & p_size )
	{
		for ( auto && l_pair: m_mapRbo )
		{
			l_pair.second->Resize( p_size );
		}

		for ( auto && l_pair: m_mapTex )
		{
			l_pair.second->Resize( p_size );
		}
	}

	bool FrameBuffer::BlitInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrcDst, uint32_t p_uiComponents )
	{
		bool l_bReturn = p_pBuffer->Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_bReturn )
		{
			l_bReturn = Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_READ );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoStretchInto( p_pBuffer, p_rectSrcDst, p_rectSrcDst, p_uiComponents, eINTERPOLATION_MODE_NEAREST );
			p_pBuffer->Unbind();
		}

		return l_bReturn;
	}

	bool FrameBuffer::StretchInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE p_eInterpolation )
	{
		bool l_bReturn = p_pBuffer->Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_bReturn )
		{
			l_bReturn = Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_READ );

			if ( l_bReturn )
			{
				l_bReturn = DoStretchInto( p_pBuffer, p_rectSrc, p_rectDst, p_uiComponents, p_eInterpolation );
				Unbind();
			}

			p_pBuffer->Unbind();
		}

		return l_bReturn;
	}

	void FrameBuffer::RenderToBuffer( FrameBufferSPtr p_pBuffer, Size const & p_sizeDst, uint32_t p_uiComponents, DepthStencilStateSPtr p_pDepthStencilState, RasteriserStateSPtr p_pRasteriserState )
	{
		if ( m_mapTex.size() )
		{
			DynamicTextureSPtr l_pTexture = m_mapTex.begin()->second;

			if ( l_pTexture && p_pBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
			{
				p_pDepthStencilState->Apply();
				p_pRasteriserState->Apply();
				m_pEngine->GetRenderSystem()->GetCurrentContext()->BToBRender( p_sizeDst, l_pTexture, p_uiComponents );
				p_pBuffer->Unbind();

				if ( p_pBuffer->m_mapTex.size() )
				{
					l_pTexture = p_pBuffer->m_mapTex.begin()->second;
					l_pTexture->Bind();
					l_pTexture->GenerateMipmaps();
					l_pTexture->Unbind();
				}
			}
		}
		else
		{
			Logger::LogWarning( cuT( "Can't render a frame buffer that has no texture to bind" ) );
		}
	}

	void FrameBuffer::DoTexAttach( eATTACHMENT_POINT p_eAttach, DynamicTextureSPtr p_pTexture )
	{
		DoDetach( p_eAttach );
		m_arrayAttaches.push_back( p_eAttach );
		std::sort( m_arrayAttaches.begin(), m_arrayAttaches.end() );
		m_mapTex[p_eAttach] = p_pTexture;
	}

	void FrameBuffer::DoRboAttach( eATTACHMENT_POINT p_eAttach, RenderBufferSPtr p_pRenderBuffer )
	{
		DoDetach( p_eAttach );
		m_arrayAttaches.push_back( p_eAttach );
		std::sort( m_arrayAttaches.begin(), m_arrayAttaches.end() );
		m_mapRbo[p_eAttach] = p_pRenderBuffer;
	}

	void FrameBuffer::DoDetach( eATTACHMENT_POINT p_eAttach )
	{
		AttachArrayIt l_itAtt = std::find( m_arrayAttaches.begin(), m_arrayAttaches.end(), p_eAttach );

		if ( l_itAtt != m_arrayAttaches.end() )
		{
			m_arrayAttaches.erase( l_itAtt );
		}

		RboAttachMapIt l_itRbo = m_mapRbo.find( p_eAttach );

		if ( l_itRbo != m_mapRbo.end() )
		{
			m_mapRbo.erase( l_itRbo );
		}

		TexAttachMapIt l_itTex = m_mapTex.find( p_eAttach );

		if ( l_itTex != m_mapTex.end() )
		{
			m_mapTex.erase( l_itTex );
		}
	}
}

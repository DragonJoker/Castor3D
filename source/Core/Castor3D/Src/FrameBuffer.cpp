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
	FrameBuffer::FrameBuffer( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	bool FrameBuffer::Bind( eFRAMEBUFFER_MODE p_mode, eFRAMEBUFFER_TARGET p_eTarget )
	{
		bool l_return = DoBind( p_eTarget );

		if ( l_return && !m_attaches.empty() && p_mode == eFRAMEBUFFER_MODE_AUTOMATIC )
		{
			SetDrawBuffers( m_attaches );
		}

		return l_return;
	}

	void FrameBuffer::Unbind()
	{
		DoUnbind();
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, uint8_t p_index, TextureAttachmentSPtr p_texture, eTEXTURE_TARGET p_target, int p_layer )
	{
		DoDetach( p_attachment );
		bool l_return = DoAttach( p_attachment, p_index, p_texture, p_target, p_layer );

		if ( l_return )
		{
			p_texture->Attach( p_attachment, p_index, shared_from_this(), p_target, p_layer );
			m_attaches.push_back( p_texture );
		}

		return l_return;
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, TextureAttachmentSPtr p_texture, eTEXTURE_TARGET p_target, int p_layer )
	{
		DoDetach( p_attachment );
		bool l_return = DoAttach( p_attachment, 0, p_texture, p_target, p_layer );

		if ( l_return )
		{
			p_texture->Attach( p_attachment, 0, p_target, p_layer );
			m_attaches.push_back( p_texture );
		}

		return l_return;
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, uint8_t p_index, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		DoDetach( p_attachment );
		bool l_return = DoAttach( p_attachment, p_index, p_renderBuffer );

		if ( l_return )
		{
			p_renderBuffer->Attach( p_attachment, p_index, shared_from_this() );
			m_attaches.push_back( p_renderBuffer );
		}

		return l_return;
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		DoDetach( p_attachment );
		bool l_return = DoAttach( p_attachment, 0, p_renderBuffer );

		if ( l_return )
		{
			p_renderBuffer->Attach( p_attachment, 0 );
			m_attaches.push_back( p_renderBuffer );
		}

		return l_return;
	}

	void FrameBuffer::DetachAll()
	{
		for ( auto l_attach : m_attaches )
		{
			l_attach->Detach();
		}

		m_attaches.clear();
	}

	void FrameBuffer::Resize( Castor::Size const & p_size )
	{
		for ( auto && l_attach : m_attaches )
		{
			if ( l_attach->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
			{
				std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture()->Resize( p_size );
			}
			else
			{
				std::static_pointer_cast< RenderBufferAttachment >( l_attach )->GetRenderBuffer()->Resize( p_size );
			}
		}
	}

	bool FrameBuffer::BlitInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrcDst, uint32_t p_uiComponents )
	{
		bool l_return = p_pBuffer->Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			l_return = Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_READ );
		}

		if ( l_return )
		{
			l_return = DoBlitInto( p_pBuffer, p_rectSrcDst, p_uiComponents, eINTERPOLATION_MODE_NEAREST );
			p_pBuffer->Unbind();
		}

		return l_return;
	}

	bool FrameBuffer::SetDrawBuffers()
	{
		return SetDrawBuffers( m_attaches );
	}

	bool FrameBuffer::SetDrawBuffer( TextureAttachmentSPtr p_attach )
	{
		return SetDrawBuffers( BufAttachArray( 1, p_attach ) );
	}

	bool FrameBuffer::SetDrawBuffer( RenderBufferAttachmentSPtr p_attach )
	{
		return SetDrawBuffers( BufAttachArray( 1, p_attach ) );
	}

	void FrameBuffer::RenderToBuffer( FrameBufferSPtr p_pBuffer, Size const & p_sizeDst, uint32_t p_uiComponents, DepthStencilStateSPtr p_pDepthStencilState, RasteriserStateSPtr p_pRasteriserState )
	{
		if ( !m_attaches.empty() )
		{
			auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), []( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE;
			} );

			if ( l_it != m_attaches.end() )
			{
				DynamicTextureSPtr l_texture = std::static_pointer_cast< TextureAttachment >( *l_it )->GetTexture();

				if ( l_texture && p_pBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
				{
					p_pDepthStencilState->Apply();
					p_pRasteriserState->Apply();
					GetOwner()->GetRenderSystem()->GetCurrentContext()->BToBRender( p_sizeDst, l_texture, p_uiComponents );
					p_pBuffer->Unbind();

					if ( !p_pBuffer->m_attaches.empty() )
					{
						l_it = std::find_if( p_pBuffer->m_attaches.begin(), p_pBuffer->m_attaches.end(), []( FrameBufferAttachmentSPtr p_attach )
						{
							return p_attach->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE;
						} );

						l_texture = std::static_pointer_cast< TextureAttachment >( *l_it )->GetTexture();
						l_texture->Bind();
						l_texture->GenerateMipmaps();
						l_texture->Unbind();
					}
				}
			}
		}
		else
		{
			Logger::LogWarning( cuT( "Can't render a frame buffer that has no texture to bind" ) );
		}
	}

	void FrameBuffer::DoDetach( eATTACHMENT_POINT p_eAttach )
	{
		BufAttachArrayIt l_itAtt = std::find_if( m_attaches.begin(), m_attaches.end(), [p_eAttach]( FrameBufferAttachmentSPtr p_att )
		{
			return p_att->GetAttachmentPoint() == p_eAttach;
		} );

		if ( l_itAtt != m_attaches.end() )
		{
			( *l_itAtt )->Detach();
			m_attaches.erase( l_itAtt );
		}
	}
}

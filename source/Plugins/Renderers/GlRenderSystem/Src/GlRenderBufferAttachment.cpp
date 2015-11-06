#include "GlRenderBufferAttachment.hpp"
#include "OpenGl.hpp"
#include "GlFrameBuffer.hpp"
#include "GlColourRenderBuffer.hpp"
#include "GlDepthStencilRenderBuffer.hpp"

#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlRenderBufferAttachment::GlRenderBufferAttachment( OpenGl & p_gl, RenderBufferSPtr p_pRenderBuffer )
		: RenderBufferAttachment( p_pRenderBuffer )
		, m_eGlAttachmentPoint( eGL_RENDERBUFFER_ATTACHMENT_NONE )
		, m_eGlStatus( eGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
		, m_gl( p_gl )
	{
	}

	GlRenderBufferAttachment::~GlRenderBufferAttachment()
	{
	}

	bool GlRenderBufferAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, eINTERPOLATION_MODE p_interpolation )
	{
		bool l_return = false;

		if ( m_gl.HasFbo() )
		{
			l_return = m_eGlStatus == eGL_FRAMEBUFFER_COMPLETE;
			GlFrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlFrameBuffer >( p_buffer );

			if ( l_return )
			{
				l_return = m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, m_pGlFrameBuffer.lock()->GetGlName() );
			}

			if ( l_return )
			{
				l_return = m_gl.BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, l_pBuffer->GetGlName() );
			}

			if ( l_return )
			{
				l_return = m_gl.ReadBuffer( m_gl.Get( m_eGlAttachmentPoint ) );
			}

			if ( l_return )
			{
				if ( m_eGlAttachmentPoint == eGL_RENDERBUFFER_ATTACHMENT_DEPTH )
				{
					l_return = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_COMPONENT_DEPTH, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else if ( m_eGlAttachmentPoint == eGL_RENDERBUFFER_ATTACHMENT_STENCIL )
				{
					l_return = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_COMPONENT_STENCIL, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else
				{
					l_return = m_gl.BlitFramebuffer( p_rectSrc, p_rectDst, eGL_COMPONENT_COLOR, m_gl.Get( p_interpolation ) );
				}
			}
		}

		return l_return;
	}

	bool GlRenderBufferAttachment::DoAttach( FrameBufferSPtr p_pFrameBuffer )
	{
		bool l_return = false;

		if ( m_gl.HasFbo() )
		{
			m_eGlAttachmentPoint = eGL_RENDERBUFFER_ATTACHMENT( m_gl.GetRboAttachment( GetAttachmentPoint() ) + GetAttachmentIndex() );
			uint32_t l_uiGlName;

			switch ( GetRenderBuffer()->GetComponent() )
			{
			case eBUFFER_COMPONENT_COLOUR:
				l_uiGlName = std::static_pointer_cast< GlColourRenderBuffer >( GetRenderBuffer() )->GetGlName();
				break;

			case eBUFFER_COMPONENT_DEPTH:
				l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
				break;

			case eBUFFER_COMPONENT_STENCIL:
				l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
				break;

			default:
				l_uiGlName = uint32_t( eGL_INVALID_INDEX );
				break;
			}

			if ( l_uiGlName != eGL_INVALID_INDEX )
			{
				m_pGlFrameBuffer = std::static_pointer_cast< GlFrameBuffer >( p_pFrameBuffer );
				l_return = m_gl.FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, eGL_RENDERBUFFER_MODE_DEFAULT, l_uiGlName );
			}

			if ( l_return )
			{
				m_eGlStatus = eGL_FRAMEBUFFER_STATUS( m_gl.CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) );

				if ( m_eGlStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
				{
					m_eGlStatus = eGL_FRAMEBUFFER_COMPLETE;
				}
			}
			else
			{
				m_eGlStatus = eGL_FRAMEBUFFER_UNSUPPORTED;
			}
		}

		return l_return;
	}

	void GlRenderBufferAttachment::DoDetach()
	{
		if ( m_gl.HasFbo() )
		{
			if ( m_eGlStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
			{
				m_gl.FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_eGlAttachmentPoint, eGL_RENDERBUFFER_MODE_DEFAULT, 0 );
			}

			m_eGlAttachmentPoint = eGL_RENDERBUFFER_ATTACHMENT_NONE;
		}
	}
}

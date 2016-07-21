#include "FrameBuffer/GlRenderBufferAttachment.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "FrameBuffer/GlColourRenderBuffer.hpp"
#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlRenderBufferAttachment::GlRenderBufferAttachment( OpenGl & p_gl, RenderBufferSPtr p_renderBuffer )
		: RenderBufferAttachment( p_renderBuffer )
		, Holder( p_gl )
		, m_glAttachmentPoint( eGL_RENDERBUFFER_ATTACHMENT_NONE )
		, m_glStatus( eGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
	{
	}

	GlRenderBufferAttachment::~GlRenderBufferAttachment()
	{
	}

	bool GlRenderBufferAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		bool l_return = false;

		if ( GetOpenGl().HasFbo() )
		{
			l_return = m_glStatus == eGL_FRAMEBUFFER_COMPLETE;
			GlFrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlFrameBuffer >( p_buffer );

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( eGL_FRAMEBUFFER_MODE_READ, m_glFrameBuffer.lock()->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( eGL_FRAMEBUFFER_MODE_DRAW, l_pBuffer->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().ReadBuffer( GetOpenGl().Get( m_glAttachmentPoint ) );
			}

			if ( l_return )
			{
				if ( m_glAttachmentPoint == eGL_RENDERBUFFER_ATTACHMENT_DEPTH )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, eGL_COMPONENT_DEPTH, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else if ( m_glAttachmentPoint == eGL_RENDERBUFFER_ATTACHMENT_STENCIL )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, eGL_COMPONENT_STENCIL, eGL_INTERPOLATION_MODE_NEAREST );
				}
				else
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, eGL_COMPONENT_COLOR, GetOpenGl().Get( p_interpolation ) );
				}
			}
		}

		return l_return;
	}

	bool GlRenderBufferAttachment::DoAttach( FrameBufferSPtr p_frameBuffer )
	{
		bool l_return = false;

		if ( GetOpenGl().HasFbo() )
		{
			m_glAttachmentPoint = eGL_RENDERBUFFER_ATTACHMENT( GetOpenGl().GetRboAttachment( GetAttachmentPoint() ) + GetAttachmentIndex() );
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
				m_glFrameBuffer = std::static_pointer_cast< GlFrameBuffer >( p_frameBuffer );
				l_return = GetOpenGl().FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, eGL_RENDERBUFFER_MODE_DEFAULT, l_uiGlName );
			}

			if ( l_return )
			{
				m_glStatus = eGL_FRAMEBUFFER_STATUS( GetOpenGl().CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE_DEFAULT ) );

				if ( m_glStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
				{
					m_glStatus = eGL_FRAMEBUFFER_COMPLETE;
				}
			}
			else
			{
				m_glStatus = eGL_FRAMEBUFFER_UNSUPPORTED;
			}
		}

		return l_return;
	}

	void GlRenderBufferAttachment::DoDetach()
	{
		if ( GetOpenGl().HasFbo() )
		{
			if ( m_glStatus != eGL_FRAMEBUFFER_UNSUPPORTED )
			{
				GetOpenGl().FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE_DEFAULT, m_glAttachmentPoint, eGL_RENDERBUFFER_MODE_DEFAULT, 0 );
			}

			m_glAttachmentPoint = eGL_RENDERBUFFER_ATTACHMENT_NONE;
		}
	}
}

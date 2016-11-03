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
		, m_glAttachmentPoint( GlAttachmentPoint::None )
		, m_glStatus( GlFramebufferStatus::IncompleteMissingAttachment )
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
			l_return = m_glStatus == GlFramebufferStatus::Complete;
			GlFrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlFrameBuffer >( p_buffer );

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( GlFrameBufferMode::Read, std::static_pointer_cast< GlFrameBuffer >( GetFrameBuffer() )->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( GlFrameBufferMode::Draw, l_pBuffer->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().ReadBuffer( GetOpenGl().Get( m_glAttachmentPoint ) );
			}

			if ( l_return )
			{
				if ( m_glAttachmentPoint == GlAttachmentPoint::Depth )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, GlComponent::Depth, GlInterpolationMode::Nearest );
				}
				else if ( m_glAttachmentPoint == GlAttachmentPoint::Stencil )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, GlComponent::Stencil, GlInterpolationMode::Nearest );
				}
				else
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, GlComponent::Colour, GetOpenGl().Get( p_interpolation ) );
				}
			}
		}

		return l_return;
	}

	bool GlRenderBufferAttachment::DoAttach()
	{
		bool l_return = false;

		if ( GetOpenGl().HasFbo() )
		{
			m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
			uint32_t l_uiGlName;

			switch ( GetRenderBuffer()->GetComponent() )
			{
			case BufferComponent::Colour:
				l_uiGlName = std::static_pointer_cast< GlColourRenderBuffer >( GetRenderBuffer() )->GetGlName();
				break;

			case BufferComponent::Depth:
				l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
				break;

			case BufferComponent::Stencil:
				l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
				break;

			default:
				l_uiGlName = uint32_t( GlInvalidIndex );
				break;
			}

			if ( l_uiGlName != GlInvalidIndex )
			{
				l_return = GetOpenGl().FramebufferRenderbuffer( GlFrameBufferMode::Default, m_glAttachmentPoint, GlRenderBufferMode::Default, l_uiGlName );
			}

			if ( l_return )
			{
				m_glStatus = GlFramebufferStatus( GetOpenGl().CheckFramebufferStatus( GlFrameBufferMode::Default ) );

				if ( m_glStatus != GlFramebufferStatus::Unsupported )
				{
					m_glStatus = GlFramebufferStatus::Complete;
				}
			}
			else
			{
				m_glStatus = GlFramebufferStatus::Unsupported;
			}
		}

		return l_return;
	}

	void GlRenderBufferAttachment::DoDetach()
	{
		if ( GetOpenGl().HasFbo() )
		{
			if ( m_glStatus != GlFramebufferStatus::Unsupported )
			{
				GetOpenGl().FramebufferRenderbuffer( GlFrameBufferMode::Default, m_glAttachmentPoint, GlRenderBufferMode::Default, 0 );
			}

			m_glAttachmentPoint = GlAttachmentPoint::None;
		}
	}
}

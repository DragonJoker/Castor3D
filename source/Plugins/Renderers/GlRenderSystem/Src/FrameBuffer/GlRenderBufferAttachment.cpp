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
		, m_glAttachmentPoint( GlAttachmentPoint::eNone )
		, m_glStatus( GlFramebufferStatus::eIncompleteMissingAttachment )
	{
	}

	GlRenderBufferAttachment::~GlRenderBufferAttachment()
	{
	}

	void GlRenderBufferAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		uint32_t l_uiGlName;

		switch ( GetRenderBuffer()->GetComponent() )
		{
		case BufferComponent::eColour:
			l_uiGlName = std::static_pointer_cast< GlColourRenderBuffer >( GetRenderBuffer() )->GetGlName();
			break;

		case BufferComponent::eDepth:
			l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
			break;

		case BufferComponent::eStencil:
			l_uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
			break;

		default:
			l_uiGlName = uint32_t( GlInvalidIndex );
			break;
		}

		if ( l_uiGlName != GlInvalidIndex )
		{
			GetOpenGl().FramebufferRenderbuffer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GlRenderBufferMode::eDefault, l_uiGlName );
			m_glStatus = GlFramebufferStatus( GetOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) );

			if ( m_glStatus != GlFramebufferStatus::eUnsupported )
			{
				m_glStatus = GlFramebufferStatus::eComplete;
			}
		}
		else
		{
			m_glStatus = GlFramebufferStatus::eUnsupported;
		}
	}

	void GlRenderBufferAttachment::DoDetach()
	{
		if ( GetOpenGl().HasFbo() )
		{
			if ( m_glStatus != GlFramebufferStatus::eUnsupported )
			{
				GetOpenGl().FramebufferRenderbuffer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GlRenderBufferMode::eDefault, 0 );
			}

			m_glAttachmentPoint = GlAttachmentPoint::eNone;
		}
	}
}

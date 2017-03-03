#include "FrameBuffer/GlES3RenderBufferAttachment.hpp"

#include "Common/OpenGlES3.hpp"
#include "FrameBuffer/GlES3FrameBuffer.hpp"
#include "FrameBuffer/GlES3ColourRenderBuffer.hpp"
#include "FrameBuffer/GlES3DepthStencilRenderBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3RenderBufferAttachment::GlES3RenderBufferAttachment( OpenGlES3 & p_gl, RenderBufferSPtr p_renderBuffer )
		: RenderBufferAttachment( p_renderBuffer )
		, Holder( p_gl )
		, m_glAttachmentPoint( GlES3AttachmentPoint::eNone )
		, m_glStatus( GlES3FramebufferStatus::eIncompleteMissingAttachment )
	{
	}

	GlES3RenderBufferAttachment::~GlES3RenderBufferAttachment()
	{
	}

	void GlES3RenderBufferAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		REQUIRE( m_glStatus == GlES3FramebufferStatus::eComplete );
		GlES3FrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlES3FrameBuffer >( p_buffer );
		GetOpenGlES3().BindFramebuffer( GlES3FrameBufferMode::eRead, std::static_pointer_cast< GlES3FrameBuffer >( GetFrameBuffer() )->GetGlES3Name() );
		GetOpenGlES3().BindFramebuffer( GlES3FrameBufferMode::eDraw, l_pBuffer->GetGlES3Name() );
		GetOpenGlES3().ReadBuffer( GetOpenGlES3().Get( m_glAttachmentPoint ) );

		if ( m_glAttachmentPoint == GlES3AttachmentPoint::eDepth )
		{
			GetOpenGlES3().BlitFramebuffer( p_rectSrc, p_rectDst, GlES3Component::eDepth, GlES3InterpolationMode::eNearest );
		}
		else if ( m_glAttachmentPoint == GlES3AttachmentPoint::eStencil )
		{
			GetOpenGlES3().BlitFramebuffer( p_rectSrc, p_rectDst, GlES3Component::eStencil, GlES3InterpolationMode::eNearest );
		}
		else
		{
			GetOpenGlES3().BlitFramebuffer( p_rectSrc, p_rectDst, GlES3Component::eColour, GetOpenGlES3().Get( p_interpolation ) );
		}
	}

	void GlES3RenderBufferAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlES3AttachmentPoint( uint32_t( GetOpenGlES3().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		uint32_t l_uiGlES3Name;

		switch ( GetRenderBuffer()->GetComponent() )
		{
		case BufferComponent::eColour:
			l_uiGlES3Name = std::static_pointer_cast< GlES3ColourRenderBuffer >( GetRenderBuffer() )->GetGlES3Name();
			break;

		case BufferComponent::eDepth:
			l_uiGlES3Name = std::static_pointer_cast< GlES3DepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlES3Name();
			break;

		case BufferComponent::eStencil:
			l_uiGlES3Name = std::static_pointer_cast< GlES3DepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlES3Name();
			break;

		default:
			l_uiGlES3Name = uint32_t( GlES3InvalidIndex );
			break;
		}

		if ( l_uiGlES3Name != GlES3InvalidIndex )
		{
			GetOpenGlES3().FramebufferRenderbuffer( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, GlES3RenderBufferMode::eDefault, l_uiGlES3Name );
			m_glStatus = GlES3FramebufferStatus( GetOpenGlES3().CheckFramebufferStatus( GlES3FrameBufferMode::eDefault ) );

			if ( m_glStatus != GlES3FramebufferStatus::eUnsupported )
			{
				m_glStatus = GlES3FramebufferStatus::eComplete;
			}
		}
		else
		{
			m_glStatus = GlES3FramebufferStatus::eUnsupported;
		}
	}

	void GlES3RenderBufferAttachment::DoDetach()
	{
		if ( GetOpenGlES3().HasFbo() )
		{
			if ( m_glStatus != GlES3FramebufferStatus::eUnsupported )
			{
				GetOpenGlES3().FramebufferRenderbuffer( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, GlES3RenderBufferMode::eDefault, 0 );
			}

			m_glAttachmentPoint = GlES3AttachmentPoint::eNone;
		}
	}
}

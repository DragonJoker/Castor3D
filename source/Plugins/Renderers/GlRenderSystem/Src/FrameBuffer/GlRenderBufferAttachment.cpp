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

	void GlRenderBufferAttachment::DoDownload( Castor::Position const & p_offset
		, Castor::PxBufferBase & p_buffer )const
	{
		GetOpenGl().ReadBuffer( GlBufferBinding( int( GlBufferBinding::eColor0 ) + GetAttachmentIndex() ) );
		auto format = GetOpenGl().Get( p_buffer.format() );
		GetOpenGl().ReadPixels( p_offset
			, p_buffer.dimensions()
			, format.Format
			, format.Type
			, p_buffer.ptr() );
	}

	void GlRenderBufferAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		uint32_t uiGlName;

		switch ( GetRenderBuffer()->GetComponent() )
		{
		case BufferComponent::eColour:
			uiGlName = std::static_pointer_cast< GlColourRenderBuffer >( GetRenderBuffer() )->GetGlName();
			break;

		case BufferComponent::eDepth:
			uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
			break;

		case BufferComponent::eStencil:
			uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( GetRenderBuffer() )->GetGlName();
			break;

		default:
			uiGlName = uint32_t( GlInvalidIndex );
			break;
		}

		if ( uiGlName != GlInvalidIndex )
		{
			GetOpenGl().FramebufferRenderbuffer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GlRenderBufferMode::eDefault, uiGlName );
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

	void GlRenderBufferAttachment::DoClear( Colour const & p_colour )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eColour
			, GetAttachmentIndex()
			, p_colour.const_ptr() );
	}

	void GlRenderBufferAttachment::DoClear( float p_depth )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eDepth
			, GetAttachmentIndex()
			, &p_depth );
	}

	void GlRenderBufferAttachment::DoClear( int p_stencil )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eStencil
			, GetAttachmentIndex()
			, &p_stencil );
	}

	void GlRenderBufferAttachment::DoClear( float p_depth, int p_stencil )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eDepthStencil
			, GetAttachmentIndex()
			, p_depth
			, p_stencil );
	}
}

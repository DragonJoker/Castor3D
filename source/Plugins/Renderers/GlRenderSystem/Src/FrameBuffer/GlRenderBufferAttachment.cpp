#include "FrameBuffer/GlRenderBufferAttachment.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "FrameBuffer/GlColourRenderBuffer.hpp"
#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace castor3d;
using namespace castor;

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

	void GlRenderBufferAttachment::doDownload( castor::Position const & p_offset
		, castor::PxBufferBase & p_buffer )const
	{
		getOpenGl().ReadBuffer( GlBufferBinding( int( GlBufferBinding::eColor0 ) + getAttachmentIndex() ) );
		auto format = getOpenGl().get( p_buffer.format() );
		getOpenGl().ReadPixels( p_offset
			, p_buffer.dimensions()
			, format.Format
			, format.Type
			, p_buffer.ptr() );
	}

	void GlRenderBufferAttachment::doAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( getOpenGl().get( getAttachmentPoint() ) ) + getAttachmentIndex() );
		uint32_t uiGlName;

		switch ( getRenderBuffer()->getComponent() )
		{
		case BufferComponent::eColour:
			uiGlName = std::static_pointer_cast< GlColourRenderBuffer >( getRenderBuffer() )->getGlName();
			break;

		case BufferComponent::eDepth:
			uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( getRenderBuffer() )->getGlName();
			break;

		case BufferComponent::eStencil:
			uiGlName = std::static_pointer_cast< GlDepthStencilRenderBuffer >( getRenderBuffer() )->getGlName();
			break;

		default:
			uiGlName = uint32_t( GlInvalidIndex );
			break;
		}

		if ( uiGlName != GlInvalidIndex )
		{
			getOpenGl().FramebufferRenderbuffer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GlRenderBufferMode::eDefault, uiGlName );
			m_glStatus = GlFramebufferStatus( getOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) );

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

	void GlRenderBufferAttachment::doDetach()
	{
		if ( getOpenGl().hasFbo() )
		{
			if ( m_glStatus != GlFramebufferStatus::eUnsupported )
			{
				getOpenGl().FramebufferRenderbuffer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GlRenderBufferMode::eDefault, 0 );
			}

			m_glAttachmentPoint = GlAttachmentPoint::eNone;
		}
	}

	void GlRenderBufferAttachment::doClear( Colour const & p_colour )const
	{
		getOpenGl().ClearBuffer( GlComponent::eColour
			, getAttachmentIndex()
			, p_colour.constPtr() );
	}

	void GlRenderBufferAttachment::doClear( float p_depth )const
	{
		getOpenGl().ClearBuffer( GlComponent::eDepth
			, getAttachmentIndex()
			, &p_depth );
	}

	void GlRenderBufferAttachment::doClear( int p_stencil )const
	{
		getOpenGl().ClearBuffer( GlComponent::eStencil
			, getAttachmentIndex()
			, &p_stencil );
	}

	void GlRenderBufferAttachment::doClear( float p_depth, int p_stencil )const
	{
		getOpenGl().ClearBuffer( GlComponent::eDepthStencil
			, getAttachmentIndex()
			, p_depth
			, p_stencil );
	}
}

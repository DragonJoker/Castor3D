#include "FrameBufferAttachment.hpp"

using namespace castor;

namespace castor3d
{
	FrameBufferAttachment::FrameBufferAttachment( AttachmentType p_type )
		: m_type( p_type )
		, m_point( AttachmentPoint::eNone )
		, m_index( 0 )
	{
	}

	FrameBufferAttachment::~FrameBufferAttachment()
	{
	}

	void FrameBufferAttachment::download( castor::Position const & p_offset
		, castor::PxBufferBase & p_buffer )const
	{
		REQUIRE( getAttachmentPoint() == AttachmentPoint::eColour );
		REQUIRE( int32_t( getBuffer()->dimensions().getWidth() ) >= int32_t( p_offset.x() + p_buffer.dimensions().getWidth() ) );
		REQUIRE( int32_t( getBuffer()->dimensions().getHeight() ) >= int32_t( p_offset.y() + p_buffer.dimensions().getHeight() ) );
		doDownload( p_offset, p_buffer );
	}

	void FrameBufferAttachment::attach( AttachmentPoint p_eAttachment )
	{
		attach( p_eAttachment, 0 );
	}

	void FrameBufferAttachment::attach( AttachmentPoint p_eAttachment
		, uint8_t p_index )
	{
		m_index = p_index;
		m_point = p_eAttachment;
		doAttach();
	}

	void FrameBufferAttachment::detach()
	{
		doDetach();
		m_point = AttachmentPoint::eNone;
		m_index = 0;
	}

	void FrameBufferAttachment::clear( castor::Colour const & p_colour )const
	{
		REQUIRE( m_point == AttachmentPoint::eColour );
		doClear( p_colour );
	}

	void FrameBufferAttachment::clear( float p_depth )const
	{
		REQUIRE( m_point == AttachmentPoint::eDepth
			|| m_point == AttachmentPoint::eDepthStencil );
		doClear( p_depth );
	}

	void FrameBufferAttachment::clear( int p_stencil )const
	{
		REQUIRE( m_point == AttachmentPoint::eStencil
			|| m_point == AttachmentPoint::eDepthStencil );
		doClear( p_stencil );
	}

	void FrameBufferAttachment::clear( float p_depth, int p_stencil )const
	{
		REQUIRE( m_point == AttachmentPoint::eDepthStencil );
		doClear( p_depth, p_stencil );
	}
}

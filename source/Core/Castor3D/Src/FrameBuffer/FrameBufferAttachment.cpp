#include "FrameBufferAttachment.hpp"

using namespace Castor;

namespace Castor3D
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

	void FrameBufferAttachment::Download( Castor::Position const & p_offset
		, Castor::PxBufferBase & p_buffer )const
	{
		REQUIRE( GetAttachmentPoint() == AttachmentPoint::eColour );
		REQUIRE( int32_t( GetBuffer()->dimensions().width() ) >= int32_t( p_offset.x() + p_buffer.dimensions().width() ) );
		REQUIRE( int32_t( GetBuffer()->dimensions().height() ) >= int32_t( p_offset.y() + p_buffer.dimensions().height() ) );
		DoDownload( p_offset, p_buffer );
	}

	void FrameBufferAttachment::Attach( AttachmentPoint p_eAttachment )
	{
		Attach( p_eAttachment, 0 );
	}

	void FrameBufferAttachment::Attach( AttachmentPoint p_eAttachment
		, uint8_t p_index )
	{
		m_index = p_index;
		m_point = p_eAttachment;
		DoAttach();
	}

	void FrameBufferAttachment::Detach()
	{
		DoDetach();
		m_point = AttachmentPoint::eNone;
		m_index = 0;
	}

	void FrameBufferAttachment::Clear( Castor::Colour const & p_colour )const
	{
		REQUIRE( m_point == AttachmentPoint::eColour );
		DoClear( p_colour );
	}

	void FrameBufferAttachment::Clear( float p_depth )const
	{
		REQUIRE( m_point == AttachmentPoint::eDepth
			|| m_point == AttachmentPoint::eDepthStencil );
		DoClear( p_depth );
	}

	void FrameBufferAttachment::Clear( int p_stencil )const
	{
		REQUIRE( m_point == AttachmentPoint::eStencil
			|| m_point == AttachmentPoint::eDepthStencil );
		DoClear( p_stencil );
	}

	void FrameBufferAttachment::Clear( float p_depth, int p_stencil )const
	{
		REQUIRE( m_point == AttachmentPoint::eDepthStencil );
		DoClear( p_depth, p_stencil );
	}
}

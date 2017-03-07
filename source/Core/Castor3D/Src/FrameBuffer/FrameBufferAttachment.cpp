#include "FrameBufferAttachment.hpp"

#include "FrameBuffer.hpp"

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
}

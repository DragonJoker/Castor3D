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

	void FrameBufferAttachment::Attach( AttachmentPoint p_eAttachment, FrameBufferSPtr p_frameBuffer )
	{
		Attach( p_eAttachment, 0, p_frameBuffer );
	}

	void FrameBufferAttachment::Attach( AttachmentPoint p_eAttachment, uint8_t p_index, FrameBufferSPtr p_frameBuffer )
	{
		m_index = p_index;
		m_point = p_eAttachment;
		m_frameBuffer = p_frameBuffer;
		DoAttach();
	}

	void FrameBufferAttachment::Detach()
	{
		DoDetach();
		m_point = AttachmentPoint::eNone;
		m_index = 0;
		m_frameBuffer.reset();
	}

	PxBufferBaseSPtr FrameBufferAttachment::DownloadBuffer()
	{
		PxBufferBaseSPtr l_return = GetBuffer();
		FrameBufferSPtr l_frameBuffer = GetFrameBuffer();

		if ( l_return && l_frameBuffer )
		{
			l_frameBuffer->DownloadBuffer( GetAttachmentPoint(), GetAttachmentIndex(), l_return );
		}

		return l_return;
	}
}

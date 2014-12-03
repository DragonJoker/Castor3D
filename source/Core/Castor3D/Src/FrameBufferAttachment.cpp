#include "FrameBufferAttachment.hpp"

using namespace Castor;

namespace Castor3D
{
	FrameBufferAttachment::FrameBufferAttachment()
		:	m_eAttachmentPoint( eATTACHMENT_POINT_NONE )
	{
	}

	FrameBufferAttachment::~FrameBufferAttachment()
	{
	}

	bool FrameBufferAttachment::Attach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		m_eAttachmentPoint = p_eAttachment;
		m_pFrameBuffer = p_pFrameBuffer;
		return DoAttach( p_eAttachment, p_pFrameBuffer );
	}

	void FrameBufferAttachment::Detach()
	{
		DoDetach();
		m_eAttachmentPoint = eATTACHMENT_POINT_NONE;
		m_pFrameBuffer.reset();
	}
}

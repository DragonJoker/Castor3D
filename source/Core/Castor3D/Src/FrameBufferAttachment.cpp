#include "FrameBufferAttachment.hpp"

using namespace Castor;

namespace Castor3D
{
	FrameBufferAttachment::FrameBufferAttachment( eATTACHMENT_TYPE p_type )
		: m_type( p_type )
		, m_point( eATTACHMENT_POINT_NONE )
		, m_index( 0 )
	{
	}

	FrameBufferAttachment::~FrameBufferAttachment()
	{
	}

	bool FrameBufferAttachment::Attach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_frameBuffer )
	{
		return Attach( p_eAttachment, 0, p_frameBuffer );
	}

	bool FrameBufferAttachment::Attach( eATTACHMENT_POINT p_eAttachment, uint8_t p_index, FrameBufferSPtr p_frameBuffer )
	{
		m_index = p_index;
		m_point = p_eAttachment;
		m_frameBuffer = p_frameBuffer;
		return DoAttach( p_frameBuffer );
	}

	void FrameBufferAttachment::Detach()
	{
		DoDetach();
		m_point = eATTACHMENT_POINT_NONE;
		m_index = 0;
		m_frameBuffer.reset();
	}
}

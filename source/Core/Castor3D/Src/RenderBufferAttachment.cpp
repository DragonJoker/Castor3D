#include "RenderBufferAttachment.hpp"
#include "FrameBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderBufferAttachment::RenderBufferAttachment( RenderBufferSPtr p_pRenderBuffer )
		:	FrameBufferAttachment()
		,	m_pRenderBuffer( p_pRenderBuffer )
	{
	}

	RenderBufferAttachment::~RenderBufferAttachment()
	{
	}

	bool RenderBufferAttachment::Attach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer )
	{
		bool l_return = FrameBufferAttachment::Attach( p_eAttachment, p_pFrameBuffer );

		if ( l_return )
		{
			p_pFrameBuffer->Attach( this );
		}

		return l_return;
	}

	void RenderBufferAttachment::Detach()
	{
		FrameBufferSPtr l_pFrameBuffer = GetFrameBuffer();

		if ( l_pFrameBuffer )
		{
			l_pFrameBuffer->Detach( this );
			FrameBufferAttachment::Detach();
		}
	}
}

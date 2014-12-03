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
		bool l_bReturn = FrameBufferAttachment::Attach( p_eAttachment, p_pFrameBuffer );

		if ( l_bReturn )
		{
			p_pFrameBuffer->Attach( this );
		}

		return l_bReturn;
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

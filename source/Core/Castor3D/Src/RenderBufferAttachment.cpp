#include "RenderBufferAttachment.hpp"
#include "FrameBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderBufferAttachment::RenderBufferAttachment( RenderBufferSPtr p_pRenderBuffer )
		: FrameBufferAttachment( eATTACHMENT_TYPE_BUFFER )
		, m_pRenderBuffer( p_pRenderBuffer )
	{
	}

	RenderBufferAttachment::~RenderBufferAttachment()
	{
	}
}

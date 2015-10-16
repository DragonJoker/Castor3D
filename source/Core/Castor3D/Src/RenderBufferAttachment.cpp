#include "RenderBufferAttachment.hpp"

#include "FrameBuffer.hpp"
#include "RenderBuffer.hpp"

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

	PxBufferBaseSPtr RenderBufferAttachment::GetBuffer()const
	{
		RenderBufferSPtr l_renderBuffer = this->GetRenderBuffer();
		return PxBufferBase::create( l_renderBuffer->GetDimensions(), l_renderBuffer->GetPixelFormat() );
	}
}

#include "RenderBufferAttachment.hpp"

#include "RenderBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace castor;

namespace castor3d
{
	RenderBufferAttachment::RenderBufferAttachment( RenderBufferSPtr p_renderBuffer )
		: FrameBufferAttachment( AttachmentType::eBuffer )
		, m_pRenderBuffer( p_renderBuffer )
	{
	}

	RenderBufferAttachment::~RenderBufferAttachment()
	{
	}

	PxBufferBaseSPtr RenderBufferAttachment::getBuffer()const
	{
		RenderBufferSPtr renderBuffer = this->getRenderBuffer();
		return PxBufferBase::create( renderBuffer->getDimensions(), renderBuffer->getPixelFormat() );
	}
}

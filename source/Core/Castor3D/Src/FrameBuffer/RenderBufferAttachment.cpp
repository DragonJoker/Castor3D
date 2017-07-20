#include "RenderBufferAttachment.hpp"

#include "RenderBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderBufferAttachment::RenderBufferAttachment( RenderBufferSPtr p_renderBuffer )
		: FrameBufferAttachment( AttachmentType::eBuffer )
		, m_pRenderBuffer( p_renderBuffer )
	{
	}

	RenderBufferAttachment::~RenderBufferAttachment()
	{
	}

	PxBufferBaseSPtr RenderBufferAttachment::GetBuffer()const
	{
		RenderBufferSPtr renderBuffer = this->GetRenderBuffer();
		return PxBufferBase::create( renderBuffer->GetDimensions(), renderBuffer->GetPixelFormat() );
	}
}

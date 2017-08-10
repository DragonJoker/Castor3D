#include "DepthStencilRenderBuffer.hpp"

using namespace castor;

namespace castor3d
{
	DepthStencilRenderBuffer::DepthStencilRenderBuffer( PixelFormat p_format )
		:	RenderBuffer( BufferComponent::eDepth, p_format )
	{
	}

	DepthStencilRenderBuffer::~DepthStencilRenderBuffer()
	{
	}
}

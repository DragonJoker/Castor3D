﻿#include "DepthStencilRenderBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	DepthStencilRenderBuffer::DepthStencilRenderBuffer( PixelFormat p_format )
		:	RenderBuffer( BufferComponent::eDepth, p_format )
	{
	}

	DepthStencilRenderBuffer::~DepthStencilRenderBuffer()
	{
	}
}

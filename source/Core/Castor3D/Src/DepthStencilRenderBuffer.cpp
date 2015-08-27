#include "DepthStencilRenderBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	DepthStencilRenderBuffer::DepthStencilRenderBuffer( ePIXEL_FORMAT p_eFormat )
		:	RenderBuffer( eBUFFER_COMPONENT_DEPTH, p_eFormat )
	{
	}

	DepthStencilRenderBuffer::~DepthStencilRenderBuffer()
	{
	}
}

#include "ColourRenderBuffer.hpp"

using namespace castor;

namespace castor3d
{
	ColourRenderBuffer::ColourRenderBuffer( PixelFormat p_format )
		:	RenderBuffer( BufferComponent::eColour, p_format )
	{
	}

	ColourRenderBuffer::~ColourRenderBuffer()
	{
	}
}

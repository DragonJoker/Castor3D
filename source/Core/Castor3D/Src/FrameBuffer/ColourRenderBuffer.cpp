#include "ColourRenderBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	ColourRenderBuffer::ColourRenderBuffer( PixelFormat p_format )
		:	RenderBuffer( BufferComponent::eColour, p_format )
	{
	}

	ColourRenderBuffer::~ColourRenderBuffer()
	{
	}
}

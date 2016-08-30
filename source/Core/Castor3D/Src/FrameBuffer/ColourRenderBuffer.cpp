#include "ColourRenderBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	ColourRenderBuffer::ColourRenderBuffer( PixelFormat p_format )
		:	RenderBuffer( BufferComponent::Colour, p_format )
	{
	}

	ColourRenderBuffer::~ColourRenderBuffer()
	{
	}
}

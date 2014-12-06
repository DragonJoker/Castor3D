#include "ColourRenderBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	ColourRenderBuffer::ColourRenderBuffer( ePIXEL_FORMAT p_eFormat )
		:	RenderBuffer( eBUFFER_COMPONENT_COLOUR, p_eFormat )
	{
	}

	ColourRenderBuffer::~ColourRenderBuffer()
	{
	}
}

#include "RenderBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderBuffer::RenderBuffer( eBUFFER_COMPONENT p_eComponent, Castor::ePIXEL_FORMAT p_ePixelFormat )
		: m_eComponent( p_eComponent )
		, m_pixelFormat( p_ePixelFormat )
		, m_samplesCount( 0 )
	{
	}

	RenderBuffer::~RenderBuffer()
	{
	}
}

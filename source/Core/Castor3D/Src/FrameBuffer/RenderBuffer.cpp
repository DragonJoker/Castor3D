#include "RenderBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderBuffer::RenderBuffer( eBUFFER_COMPONENT p_eComponent, Castor::PixelFormat p_format )
		: m_eComponent( p_eComponent )
		, m_pixelFormat( p_format )
		, m_samplesCount( 0 )
	{
	}

	RenderBuffer::~RenderBuffer()
	{
	}
}

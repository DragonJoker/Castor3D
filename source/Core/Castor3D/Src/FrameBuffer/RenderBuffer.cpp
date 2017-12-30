#include "RenderBuffer.hpp"

using namespace castor;

namespace castor3d
{
	RenderBuffer::RenderBuffer( BufferComponent p_eComponent, castor::PixelFormat p_format )
		: m_eComponent( p_eComponent )
		, m_pixelFormat( p_format )
		, m_samplesCount( 0 )
	{
	}

	RenderBuffer::~RenderBuffer()
	{
	}
}

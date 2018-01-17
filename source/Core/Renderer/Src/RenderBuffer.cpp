/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderBuffer.hpp"

#include "Device.hpp"
#include "ImageMemoryBarrier.hpp"

namespace renderer
{
	RenderBuffer::RenderBuffer( Device const & device
		, PixelFormat format
		, IVec2 const & size )
		: m_device{ device }
		, m_format{ format }
		, m_size{ size }
	{
	}
}

/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/RenderBuffer.hpp"

#include "Core/Device.hpp"
#include "Sync/ImageMemoryBarrier.hpp"

namespace renderer
{
	RenderBuffer::RenderBuffer( Device const & device
		, PixelFormat format
		, UIVec2 const & size )
		: m_device{ device }
		, m_format{ format }
		, m_size{ size }
	{
	}
}

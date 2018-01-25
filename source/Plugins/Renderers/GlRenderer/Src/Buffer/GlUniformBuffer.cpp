#include "Buffer/GlUniformBuffer.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	UniformBuffer::UniformBuffer( renderer::Device const & device
		, uint32_t count
		, uint32_t size
		, renderer::BufferTargets target
		, renderer::MemoryPropertyFlags flags )
		: renderer::UniformBufferBase{ device
			, count
			, size
			, target
			, flags }
	{
		doCreateBuffer( count
			, target
			, flags );
	}

	void UniformBuffer::doCreateBuffer( uint32_t count
		, renderer::BufferTargets target
		, renderer::MemoryPropertyFlags flags )
	{
		m_buffer = m_device.createBuffer( count * getOffset( 1u )
			, target | renderer::BufferTarget::eUniformBuffer
			, flags );
	}
}

#include "Buffer/GlUniformBuffer.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	namespace
	{
		uint32_t getOffsetAlignment()
		{
			static uint32_t const result = []()
			{
				GLint value;
				gl::GetIntegerv( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &value );
				return uint32_t( value );
			}();
			return result;
		}

		uint32_t doGetAlignedSize( uint32_t size, uint32_t align )
		{
			uint32_t result = 0u;
			auto align32 = uint32_t( align );

			while ( size > align )
			{
				size -= align32;
				result += align32;
			}

			return result + align32;
		}
	}

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
		m_buffer = m_device.createBuffer( count * getAlignedSize( getElementSize() )
			, target | renderer::BufferTarget::eUniformBuffer
			, flags );
	}

	uint32_t UniformBuffer::getAlignedSize( uint32_t size )const
	{
		return doGetAlignedSize( size, getOffsetAlignment() );
	}
}

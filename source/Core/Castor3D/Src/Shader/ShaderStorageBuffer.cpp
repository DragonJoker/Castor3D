#include "ShaderStorageBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	ShaderStorageBuffer::ShaderStorageBuffer( Engine & engine )
		: CpuBuffer< uint8_t >{ engine }
	{
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
	}

	bool ShaderStorageBuffer::initialise( renderer::MemoryPropertyFlags flags )
	{
		if ( !m_gpuBuffer )
		{
			auto buffer = getEngine()->getRenderSystem()->getBuffer( BufferType::eShaderStorage
				, getSize()
				, flags );
			m_gpuBuffer = buffer.buffer;
			m_offset = buffer.offset;
			doInitialise( type, nature );
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			upload();
			bindTo( 0u );
		}

		return result;
	}

	void ShaderStorageBuffer::cleanup()
	{
		if ( m_gpuBuffer )
		{
			getEngine()->getRenderSystem()->putBuffer( BufferType::eShaderStorage
				, m_accessType
				, m_accessNature
				, GpuBufferOffset{ m_gpuBuffer, m_offset } );
			m_gpuBuffer.reset();
		}
	}

	void ShaderStorageBuffer::bindTo( uint32_t index )const
	{
		getGpuBuffer().setBindingPoint( index );
	}
}

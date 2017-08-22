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

	bool ShaderStorageBuffer::initialise( BufferAccessType type
		, BufferAccessNature nature )
	{
		if ( !m_gpuBuffer )
		{
			auto buffer = getEngine()->getRenderSystem()->getBuffer( BufferType::eShaderStorage
				, getSize()
				, type
				, nature );
			m_gpuBuffer = std::move( buffer.buffer );
			m_offset = buffer.offset;
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
		doCleanup();
	}

	void ShaderStorageBuffer::bindTo( uint32_t index )const
	{
		getGpuBuffer().setBindingPoint( index );
	}
}

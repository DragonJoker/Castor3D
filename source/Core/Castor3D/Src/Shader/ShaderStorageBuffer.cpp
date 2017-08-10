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

	bool ShaderStorageBuffer::initialise( BufferAccessType p_type
		, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = getEngine()->getRenderSystem()->createBuffer( BufferType::eShaderStorage );
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			result = doInitialise( p_type, p_nature );
		}

		if ( result )
		{
			m_gpuBuffer->setBindingPoint( 0u );
		}

		return result;
	}

	void ShaderStorageBuffer::cleanup()
	{
		doCleanup();
	}

	void ShaderStorageBuffer::bindTo( uint32_t p_index )const
	{
		getGpuBuffer().setBindingPoint( p_index );
	}
}

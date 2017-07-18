#include "ShaderStorageBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	ShaderStorageBuffer::ShaderStorageBuffer( Engine & p_engine )
		: CpuBuffer< uint8_t >{ p_engine }
	{
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
	}

	bool ShaderStorageBuffer::Initialise( BufferAccessType p_type
		, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetEngine()->GetRenderSystem()->CreateBuffer( BufferType::eShaderStorage );
		}

		bool l_result = m_gpuBuffer != nullptr;

		if ( l_result )
		{
			l_result = DoInitialise( p_type, p_nature );
		}

		if ( l_result )
		{
			m_gpuBuffer->SetBindingPoint( 0u );
		}

		return l_result;
	}

	void ShaderStorageBuffer::Cleanup()
	{
		DoCleanup();
	}

	void ShaderStorageBuffer::BindTo( uint32_t p_index )const
	{
		GetGpuBuffer().SetBindingPoint( p_index );
	}
}

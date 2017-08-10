#include "IndexBuffer.hpp"

#include "Engine.hpp"

using namespace castor;

namespace castor3d
{
	IndexBuffer::IndexBuffer( Engine & engine )
		: CpuBuffer< uint32_t >( engine )
	{
	}

	IndexBuffer::~IndexBuffer()
	{
	}

	bool IndexBuffer::initialise( BufferAccessType p_type, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = getEngine()->getRenderSystem()->createBuffer( BufferType::eElementArray );
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			result = doInitialise( p_type, p_nature );
		}

		return result;
	}

	void IndexBuffer::cleanup()
	{
		doCleanup();
	}
}

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

	bool IndexBuffer::initialise( BufferAccessType type
		, BufferAccessNature nature )
	{
		if ( !m_gpuBuffer )
		{
			auto buffer = getEngine()->getRenderSystem()->getBuffer( BufferType::eElementArray
				, getSize() * sizeof( uint32_t )
				, type
				, nature );
			m_gpuBuffer = std::move( buffer.buffer );
			m_offset = buffer.offset;
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			upload();
		}

		return result;
	}

	void IndexBuffer::cleanup()
	{
		doCleanup();
	}
}

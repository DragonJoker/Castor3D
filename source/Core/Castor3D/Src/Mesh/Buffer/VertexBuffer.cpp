#include "VertexBuffer.hpp"

#include "Engine.hpp"

using namespace castor;

namespace castor3d
{
	VertexBuffer::VertexBuffer( Engine & engine
		, BufferDeclaration const & declaration )
		: CpuBuffer< uint8_t >( engine )
		, m_bufferDeclaration( declaration )
	{
	}

	bool VertexBuffer::initialise( BufferAccessType type
		, BufferAccessNature nature )
	{
		if ( !m_gpuBuffer )
		{
			auto buffer = getEngine()->getRenderSystem()->getBuffer( BufferType::eArray
				, getSize()
				, type
				, nature );
			m_gpuBuffer = buffer.buffer;
			m_offset = buffer.offset;
			doInitialise( type, nature );
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			upload();
		}

		return result;
	}

	void VertexBuffer::cleanup()
	{
		if ( m_gpuBuffer )
		{
			getEngine()->getRenderSystem()->putBuffer( BufferType::eArray
				, m_accessType
				, m_accessNature
				, GpuBufferOffset{ m_gpuBuffer, m_offset } );
			m_gpuBuffer.reset();
		}
	}
}

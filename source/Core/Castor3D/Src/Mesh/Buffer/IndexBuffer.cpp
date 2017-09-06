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
			m_gpuBuffer = buffer.buffer;
			REQUIRE( !( buffer.offset % sizeof( uint32_t ) ) );
			m_offset = buffer.offset / sizeof( uint32_t );
			doInitialise( type, nature );
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
		if ( m_gpuBuffer )
		{
			getEngine()->getRenderSystem()->putBuffer( BufferType::eElementArray
				, m_accessType
				, m_accessNature
								   , GpuBufferOffset{ m_gpuBuffer, uint32_t( m_offset * sizeof( uint32_t ) ) } );
			m_gpuBuffer.reset();
		}
	}
}

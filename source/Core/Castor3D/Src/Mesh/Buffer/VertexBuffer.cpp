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

	bool VertexBuffer::initialise( renderer::MemoryPropertyFlags flags
		, std::vector< uint8_t > const & data )
	{
		if ( !m_gpuBuffer )
		{
			auto buffer = getEngine()->getRenderSystem()->getBuffer( renderer::BufferTarget::eVertexBuffer
				, uint32_t( data.size() )
				, flags );
			m_gpuBuffer = buffer.buffer;
			m_offset = buffer.offset;
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			result = false;

			if ( auto buffer = lock( 0u, uint32_t( data.size() ), renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, data.data(), data.size() );
				unlock( uint32_t( data.size() ), true );
				result = true;
			}
		}

		return result;
	}

	void VertexBuffer::cleanup()
	{
		if ( m_gpuBuffer )
		{
			getEngine()->getRenderSystem()->putBuffer( renderer::BufferTarget::eVertexBuffer
				, GpuBufferOffset{ m_gpuBuffer, m_offset } );
			m_gpuBuffer.reset();
		}
	}
}

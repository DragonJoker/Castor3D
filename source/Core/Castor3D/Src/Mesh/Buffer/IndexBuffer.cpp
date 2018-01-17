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

	bool IndexBuffer::initialise( renderer::MemoryPropertyFlags flags
		, renderer::UInt32Array const & data )
	{
		if ( !m_gpuBuffer )
		{
			auto buffer = getEngine()->getRenderSystem()->getBuffer( renderer::BufferTarget::eIndexBuffer
				, uint32_t( data.size() * sizeof( uint32_t ) )
				, flags );
			m_gpuBuffer = buffer.buffer;
			REQUIRE( !( buffer.offset % sizeof( uint32_t ) ) );
			m_offset = buffer.offset / sizeof( uint32_t );
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			result = false;

			if ( auto buffer = lock( 0u, uint32_t( data.size() ), renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, data.data(), data.size() * sizeof( uint32_t ) );
				unlock( uint32_t( data.size() ), true );
				result = true;
			}
		}

		return result;
	}

	void IndexBuffer::cleanup()
	{
		if ( m_gpuBuffer )
		{
			getEngine()->getRenderSystem()->putBuffer( renderer::BufferTarget::eIndexBuffer
				, GpuBufferOffset{ m_gpuBuffer, uint32_t( m_offset * sizeof( uint32_t ) ) } );
			m_gpuBuffer.reset();
		}
	}
}

#include "AtomicCounterBuffer.hpp"

#include "Mesh/Buffer/GpuBuffer.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	AtomicCounterBuffer::AtomicCounterBuffer( String const & p_name
		, ShaderProgram & p_program )
		: OwnedBy< ShaderProgram >{ p_program }
	{
	}

	AtomicCounterBuffer::~AtomicCounterBuffer()
	{
	}

	bool AtomicCounterBuffer::initialise( uint32_t size
		, uint32_t index )
	{
		if ( !m_gpuBuffer )
		{
			auto buffer = getOwner()->getRenderSystem()->getBuffer( BufferType::eAtomicCounter
				, size
				, BufferAccessType::eDynamic
				, BufferAccessNature::eDraw );
			m_gpuBuffer = buffer.buffer;
			m_offset = buffer.offset;
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			m_gpuBuffer->setBindingPoint( index );
		}

		return result;
	}

	void AtomicCounterBuffer::cleanup()
	{
		if ( m_gpuBuffer )
		{
			getOwner()->getRenderSystem()->putBuffer( BufferType::eAtomicCounter
				, BufferAccessType::eDynamic
				, BufferAccessNature::eDraw
				, GpuBufferOffset{ m_gpuBuffer, m_offset } );
			m_gpuBuffer.reset();
		}
	}

	uint32_t * AtomicCounterBuffer::lock( uint32_t offset
		, uint32_t count
		, AccessTypes const & flags )
	{
		REQUIRE( m_gpuBuffer );
		return reinterpret_cast< uint32_t * >( m_gpuBuffer->lock( ( m_offset + offset ) * sizeof( uint32_t )
			, count * sizeof( uint32_t )
			, flags ) );
	}

	void AtomicCounterBuffer::unlock()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->unlock();
	}

	void AtomicCounterBuffer::upload( uint32_t offset
		, uint32_t count
		, uint32_t const * buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->upload( ( m_offset + offset ) * sizeof( uint32_t )
			, count * sizeof( uint32_t )
			, reinterpret_cast< uint8_t const * >( buffer ) );
	}

	void AtomicCounterBuffer::download( uint32_t offset
		, uint32_t count
		, uint32_t * buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->download( ( m_offset + offset ) * sizeof( uint32_t )
			, count * sizeof( uint32_t )
			, reinterpret_cast< uint8_t * >( buffer ) );
	}

	void AtomicCounterBuffer::bind()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->bind();
	}

	void AtomicCounterBuffer::unbind()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->unbind();
	}

	void AtomicCounterBuffer::copy( GpuBuffer const & src
		, uint32_t srcOffset
		, uint32_t size )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->copy( src
			, srcOffset
			, m_offset * sizeof( uint32_t )
			, size * sizeof( uint32_t ) );
	}
}

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

	bool AtomicCounterBuffer::initialise( uint32_t p_size
		, uint32_t p_index )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = getOwner()->getRenderSystem()->createBuffer( BufferType::eAtomicCounter );
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			m_gpuBuffer->create();
		}

		if ( result )
		{
			m_gpuBuffer->initialiseStorage( p_size * sizeof( uint32_t )
				, BufferAccessType::eDynamic
				, BufferAccessNature::eDraw );
			m_gpuBuffer->setBindingPoint( p_index );
		}

		return result;
	}

	void AtomicCounterBuffer::cleanup()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->destroy();
			m_gpuBuffer.reset();
		}
	}

	uint32_t * AtomicCounterBuffer::lock( uint32_t p_offset
		, uint32_t p_count
		, AccessTypes const & p_flags )
	{
		REQUIRE( m_gpuBuffer );
		return reinterpret_cast< uint32_t * >( m_gpuBuffer->lock( p_offset * sizeof( uint32_t )
			, p_count * sizeof( uint32_t )
			, p_flags ) );
	}

	void AtomicCounterBuffer::unlock()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->unlock();
	}

	void AtomicCounterBuffer::upload( uint32_t p_offset
		, uint32_t p_count
		, uint32_t const * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->upload( p_offset * sizeof( uint32_t )
			, p_count * sizeof( uint32_t )
			, reinterpret_cast< uint8_t const * >( p_buffer ) );
	}

	void AtomicCounterBuffer::download( uint32_t p_offset
		, uint32_t p_count
		, uint32_t * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->download( p_offset * sizeof( uint32_t )
			, p_count * sizeof( uint32_t )
			, reinterpret_cast< uint8_t * >( p_buffer ) );
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

	void AtomicCounterBuffer::copy( GpuBuffer const & p_src
		, uint32_t p_size )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->copy( p_src
			, p_size * sizeof( uint32_t ) );
	}
}

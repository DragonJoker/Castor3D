#include "AtomicCounterBuffer.hpp"

#include "Mesh/Buffer/GpuBuffer.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	AtomicCounterBuffer::AtomicCounterBuffer( String const & p_name
		, ShaderProgram & p_program )
		: OwnedBy< ShaderProgram >{ p_program }
	{
	}

	AtomicCounterBuffer::~AtomicCounterBuffer()
	{
	}

	bool AtomicCounterBuffer::Initialise( uint32_t p_size
		, uint32_t p_index )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetOwner()->GetRenderSystem()->CreateBuffer( BufferType::eAtomicCounter );
		}

		bool l_result = m_gpuBuffer != nullptr;

		if ( l_result )
		{
			m_gpuBuffer->Create();
		}

		if ( l_result )
		{
			m_gpuBuffer->InitialiseStorage( p_size * sizeof( uint32_t )
				, BufferAccessType::eDynamic
				, BufferAccessNature::eDraw );
			m_gpuBuffer->SetBindingPoint( p_index );
		}

		return l_result;
	}

	void AtomicCounterBuffer::Cleanup()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->Destroy();
			m_gpuBuffer.reset();
		}
	}

	uint32_t * AtomicCounterBuffer::Lock( uint32_t p_offset
		, uint32_t p_count
		, AccessTypes const & p_flags )
	{
		REQUIRE( m_gpuBuffer );
		return reinterpret_cast< uint32_t * >( m_gpuBuffer->Lock( p_offset * sizeof( uint32_t )
			, p_count * sizeof( uint32_t )
			, p_flags ) );
	}

	void AtomicCounterBuffer::Unlock()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Unlock();
	}

	void AtomicCounterBuffer::Upload( uint32_t p_offset
		, uint32_t p_count
		, uint32_t const * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Upload( p_offset * sizeof( uint32_t )
			, p_count * sizeof( uint32_t )
			, reinterpret_cast< uint8_t const * >( p_buffer ) );
	}

	void AtomicCounterBuffer::Download( uint32_t p_offset
		, uint32_t p_count
		, uint32_t * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Download( p_offset * sizeof( uint32_t )
			, p_count * sizeof( uint32_t )
			, reinterpret_cast< uint8_t * >( p_buffer ) );
	}

	void AtomicCounterBuffer::Bind()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Bind();
	}

	void AtomicCounterBuffer::Unbind()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Unbind();
	}

	void AtomicCounterBuffer::Copy( GpuBuffer const & p_src
		, uint32_t p_size )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Copy( p_src
			, p_size * sizeof( uint32_t ) );
	}
}

#include "AtomicCounterBuffer.hpp"

#include "Mesh/Buffer/GpuBuffer.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	AtomicCounterBuffer::AtomicCounterBuffer( String const & p_name, ShaderProgram & p_program )
		: OwnedBy< ShaderProgram >{ p_program }
	{
	}

	AtomicCounterBuffer::~AtomicCounterBuffer()
	{
	}

	bool AtomicCounterBuffer::Initialise( uint32_t p_size, uint32_t p_index )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetOwner()->GetRenderSystem()->CreateUInt32Buffer( BufferType::eAtomicCounter );
		}

		bool l_return = m_gpuBuffer != nullptr;

		if ( l_return )
		{
			m_gpuBuffer->Create();
		}

		if ( l_return )
		{
			m_gpuBuffer->InitialiseStorage( p_size, BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			m_gpuBuffer->InitialiseBindingPoint( p_index );
		}

		return l_return;
	}

	void AtomicCounterBuffer::Cleanup()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->Destroy();
			m_gpuBuffer.reset();
		}
	}

	uint32_t * AtomicCounterBuffer::Lock( uint32_t p_offset, uint32_t p_count, AccessTypes const & p_flags )
	{
		REQUIRE( m_gpuBuffer );
		return m_gpuBuffer->Lock( p_offset, p_count, p_flags );
	}

	void AtomicCounterBuffer::Unlock()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Unlock();
	}

	void AtomicCounterBuffer::Upload( uint32_t p_offset, uint32_t p_count, uint32_t const * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Upload( p_offset, p_count, p_buffer );
	}

	void AtomicCounterBuffer::Download( uint32_t p_offset, uint32_t p_count, uint32_t * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Download( p_offset, p_count, p_buffer );
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

	void AtomicCounterBuffer::Copy( GpuBuffer< uint32_t > const & p_src, uint32_t p_size )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Copy( p_src, p_size );
	}
}

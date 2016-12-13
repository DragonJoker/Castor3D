#include "ShaderStorageBuffer.hpp"

#include "Mesh/Buffer/GpuBuffer.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	ShaderStorageBuffer::ShaderStorageBuffer( String const & p_name, ShaderProgram & p_program )
		: OwnedBy< ShaderProgram >{ p_program }
		, Named{ p_name }
	{
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
	}

	bool ShaderStorageBuffer::Initialise( uint32_t p_size, uint32_t p_index, BufferAccessType p_type, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetOwner()->GetRenderSystem()->CreateUInt8Buffer( BufferType::eShaderStorage );
		}

		bool l_return = m_gpuBuffer != nullptr;

		if ( l_return )
		{
			l_return = m_gpuBuffer->Create();
		}

		if ( l_return )
		{
			m_gpuBuffer->InitialiseStorage( p_size, p_type, p_nature );
			m_gpuBuffer->InitialiseBindingPoint( p_index );
		}

		return l_return;
	}

	void ShaderStorageBuffer::Cleanup()
	{
		if ( m_gpuBuffer )
		{
			m_gpuBuffer->Destroy();
			m_gpuBuffer.reset();
		}
	}

	uint8_t * ShaderStorageBuffer::Lock( uint32_t p_offset, uint32_t p_count, AccessTypes const & p_flags )
	{
		REQUIRE( m_gpuBuffer );
		return m_gpuBuffer->Lock( p_offset, p_count, p_flags );
	}

	void ShaderStorageBuffer::Unlock()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Unlock();
	}

	void ShaderStorageBuffer::Upload( uint32_t p_offset, uint32_t p_count, uint8_t const * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Upload( p_offset, p_count, p_buffer );
	}

	void ShaderStorageBuffer::Download( uint32_t p_offset, uint32_t p_count, uint8_t * p_buffer )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Download( p_offset, p_count, p_buffer );
	}

	void ShaderStorageBuffer::Bind()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Bind();
	}

	void ShaderStorageBuffer::BindTo( uint32_t p_point )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->InitialiseBindingPoint( p_point );
	}

	void ShaderStorageBuffer::Unbind()
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Unbind();
	}

	void ShaderStorageBuffer::Copy( GpuBuffer< uint8_t > const & p_src, uint32_t p_size )
	{
		REQUIRE( m_gpuBuffer );
		m_gpuBuffer->Copy( p_src, p_size );
	}
}

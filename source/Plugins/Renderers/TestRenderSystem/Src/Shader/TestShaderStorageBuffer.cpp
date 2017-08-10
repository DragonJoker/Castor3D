#include "Shader/TestShaderStorageBuffer.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestShaderStorageBuffer::TestShaderStorageBuffer( TestRenderSystem & p_renderSystem )
		: GpuBuffer( p_renderSystem )
	{
	}

	TestShaderStorageBuffer::~TestShaderStorageBuffer()
	{
	}

	bool TestShaderStorageBuffer::create()
	{
		return true;
	}

	void TestShaderStorageBuffer::destroy()
	{
	}

	void TestShaderStorageBuffer::initialiseStorage( uint32_t p_count
		, BufferAccessType p_type
		, BufferAccessNature p_nature )const
	{
	}

	void TestShaderStorageBuffer::setBindingPoint( uint32_t p_point )const
	{
	}

	void TestShaderStorageBuffer::bind()const
	{
	}

	void TestShaderStorageBuffer::unbind()const
	{
	}

	uint8_t * TestShaderStorageBuffer::lock( uint32_t p_offset
		, uint32_t p_count
		, AccessTypes const & p_flags )const
	{
		return nullptr;
	}

	void TestShaderStorageBuffer::unlock()const
	{
	}

	void TestShaderStorageBuffer::copy( GpuBuffer const & p_src
		, uint32_t p_size )const
	{
	}

	void TestShaderStorageBuffer::upload( uint32_t p_offset
		, uint32_t p_size
		, uint8_t const * p_buffer )const
	{
	}

	void TestShaderStorageBuffer::download( uint32_t p_offset
		, uint32_t p_size
		, uint8_t * p_buffer )const
	{
	}
}

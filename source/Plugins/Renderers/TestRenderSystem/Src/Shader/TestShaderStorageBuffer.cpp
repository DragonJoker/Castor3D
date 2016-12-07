#include "Shader/TestShaderStorageBuffer.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestShaderStorageBuffer::TestShaderStorageBuffer( TestRenderSystem & p_renderSystem )
		: GpuBuffer< uint8_t >( p_renderSystem )
	{
	}

	TestShaderStorageBuffer::~TestShaderStorageBuffer()
	{
	}

	bool TestShaderStorageBuffer::Create()
	{
		return true;
	}

	void TestShaderStorageBuffer::Destroy()
	{
	}

	void TestShaderStorageBuffer::InitialiseStorage( uint32_t p_count, BufferAccessType p_type, BufferAccessNature p_nature )const
	{
	}

	void TestShaderStorageBuffer::InitialiseBindingPoint( uint32_t p_point )const
	{
	}

	void TestShaderStorageBuffer::Bind()const
	{
	}

	void TestShaderStorageBuffer::Unbind()const
	{
	}

	uint8_t * TestShaderStorageBuffer::Lock( uint32_t p_offset, uint32_t p_count, FlagCombination< AccessType > const & p_flags )const
	{
		return nullptr;
	}

	void TestShaderStorageBuffer::Unlock()const
	{
	}

	void TestShaderStorageBuffer::Copy( GpuBuffer< uint8_t > const & p_src, uint32_t p_size )const
	{
	}

	void TestShaderStorageBuffer::Upload( uint32_t p_offset, uint32_t p_size, uint8_t const * p_buffer )const
	{
	}

	void TestShaderStorageBuffer::Download( uint32_t p_offset, uint32_t p_size, uint8_t * p_buffer )const
	{
	}
}

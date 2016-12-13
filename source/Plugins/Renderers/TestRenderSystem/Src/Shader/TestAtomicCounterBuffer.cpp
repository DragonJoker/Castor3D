#include "Shader/TestAtomicCounterBuffer.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestAtomicCounterBuffer::TestAtomicCounterBuffer( TestRenderSystem & p_renderSystem )
		: GpuBuffer< uint32_t >( p_renderSystem )
	{
	}

	TestAtomicCounterBuffer::~TestAtomicCounterBuffer()
	{
	}

	bool TestAtomicCounterBuffer::Create()
	{
		return true;
	}

	void TestAtomicCounterBuffer::Destroy()
	{
	}

	void TestAtomicCounterBuffer::InitialiseStorage( uint32_t p_count, BufferAccessType p_type, BufferAccessNature p_nature )const
	{
	}

	void TestAtomicCounterBuffer::InitialiseBindingPoint( uint32_t p_point )const
	{
	}

	void TestAtomicCounterBuffer::Bind()const
	{
	}

	void TestAtomicCounterBuffer::Unbind()const
	{
	}

	uint32_t * TestAtomicCounterBuffer::Lock( uint32_t p_offset, uint32_t p_count, AccessTypes const & p_flags )const
	{
		return nullptr;
	}

	void TestAtomicCounterBuffer::Unlock()const
	{
	}

	void TestAtomicCounterBuffer::Copy( GpuBuffer< uint32_t > const & p_src, uint32_t p_size )const
	{
	}

	void TestAtomicCounterBuffer::Upload( uint32_t p_offset, uint32_t p_size, uint32_t const * p_buffer )const
	{
	}

	void TestAtomicCounterBuffer::Download( uint32_t p_offset, uint32_t p_size, uint32_t * p_buffer )const
	{
	}
}

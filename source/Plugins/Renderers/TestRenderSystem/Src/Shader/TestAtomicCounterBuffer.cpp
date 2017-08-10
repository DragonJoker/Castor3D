#include "Shader/TestAtomicCounterBuffer.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestAtomicCounterBuffer::TestAtomicCounterBuffer( TestRenderSystem & p_renderSystem )
		: GpuBuffer( p_renderSystem )
	{
	}

	TestAtomicCounterBuffer::~TestAtomicCounterBuffer()
	{
	}

	bool TestAtomicCounterBuffer::create()
	{
		return true;
	}

	void TestAtomicCounterBuffer::destroy()
	{
	}

	void TestAtomicCounterBuffer::initialiseStorage( uint32_t p_count
		, BufferAccessType p_type
		, BufferAccessNature p_nature )const
	{
	}

	void TestAtomicCounterBuffer::setBindingPoint( uint32_t p_point )const
	{
	}

	void TestAtomicCounterBuffer::bind()const
	{
	}

	void TestAtomicCounterBuffer::unbind()const
	{
	}

	uint8_t * TestAtomicCounterBuffer::lock( uint32_t p_offset
		, uint32_t p_count
		, AccessTypes const & p_flags )const
	{
		return nullptr;
	}

	void TestAtomicCounterBuffer::unlock()const
	{
	}

	void TestAtomicCounterBuffer::copy( GpuBuffer const & p_src
		, uint32_t p_size )const
	{
	}

	void TestAtomicCounterBuffer::upload( uint32_t p_offset
		, uint32_t p_size
		, uint8_t const * p_buffer )const
	{
	}

	void TestAtomicCounterBuffer::download( uint32_t p_offset
		, uint32_t p_size
		, uint8_t * p_buffer )const
	{
	}
}

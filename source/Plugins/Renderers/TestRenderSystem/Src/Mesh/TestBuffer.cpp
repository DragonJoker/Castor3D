#include "TestBuffer.hpp"
#include "Render/TestRenderSystem.hpp"

namespace TestRender
{
	TestBuffer::TestBuffer( TestRenderSystem & p_renderSystem
		, castor3d::BufferType p_type )
		: castor3d::GpuBuffer( p_renderSystem )
		, m_type{ p_type }
	{
	}

	TestBuffer::~TestBuffer()
	{
	}

	bool TestBuffer::create()
	{
		return true;
	}

	void TestBuffer::destroy()
	{
	}

	void TestBuffer::initialiseStorage( uint32_t p_count
		, castor3d::BufferAccessType p_type
		, castor3d::BufferAccessNature p_nature )const
	{
	}

	void TestBuffer::setBindingPoint( uint32_t p_point )const
	{
	}

	uint32_t TestBuffer::getBindingPoint()const
	{
		return 0u;
	}

	void TestBuffer::bind()const
	{
	}

	void TestBuffer::unbind()const
	{
	}

	uint8_t * TestBuffer::lock( uint32_t p_offset
		, uint32_t p_count
		, castor3d::AccessTypes const & p_flags )const
	{
		return nullptr;
	}

	void TestBuffer::unlock()const
	{
	}

	void TestBuffer::copy( castor3d::GpuBuffer const & p_src
		, uint32_t p_size )const
	{
	}

	void TestBuffer::upload( uint32_t p_offset
		, uint32_t p_size
		, uint8_t const * p_buffer )const
	{
	}

	void TestBuffer::download( uint32_t p_offset
		, uint32_t p_size
		, uint8_t * p_buffer )const
	{
	}
}

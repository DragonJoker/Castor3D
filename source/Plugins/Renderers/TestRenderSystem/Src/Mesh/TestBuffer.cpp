#include "TestBuffer.hpp"
#include "Render/TestRenderSystem.hpp"

namespace TestRender
{
	TestBuffer::TestBuffer( TestRenderSystem & renderSystem
		, castor3d::BufferType type )
		: castor3d::GpuBuffer( renderSystem )
		, m_type{ type }
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

	void TestBuffer::setBindingPoint( uint32_t point )const
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

	uint8_t * TestBuffer::lock( uint32_t offset
		, uint32_t count
		, castor3d::AccessTypes const & flags )const
	{
		return nullptr;
	}

	void TestBuffer::unlock()const
	{
	}

	void TestBuffer::copy( castor3d::GpuBuffer const & src
		, uint32_t srcOffset
		, uint32_t dstOffset
		, uint32_t size )const
	{
	}

	void TestBuffer::upload( uint32_t offset
		, uint32_t size
		, uint8_t const * buffer )const
	{
	}

	void TestBuffer::download( uint32_t offset
		, uint32_t size
		, uint8_t * buffer )const
	{
	}

	void TestBuffer::doInitialiseStorage( uint32_t count
		, castor3d::BufferAccessType type
		, castor3d::BufferAccessNature nature )const
	{
	}
}

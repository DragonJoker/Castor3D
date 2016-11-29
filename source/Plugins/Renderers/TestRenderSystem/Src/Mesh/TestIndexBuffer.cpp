#include "Mesh/TestIndexBuffer.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestIndexBuffer::TestIndexBuffer( TestRenderSystem & p_renderSystem )
		: GpuBuffer< uint32_t >( p_renderSystem )
	{
	}

	TestIndexBuffer::~TestIndexBuffer()
	{
	}

	bool TestIndexBuffer::Create()
	{
		return true;
	}

	void TestIndexBuffer::Destroy()
	{
	}

	bool TestIndexBuffer::InitialiseStorage( uint32_t p_count, BufferAccessType p_type, BufferAccessNature p_nature )const
	{
		return true;
	}

	bool TestIndexBuffer::InitialiseBindingPoint( uint32_t p_point )const
	{
		return true;
	}

	bool TestIndexBuffer::Bind()const
	{
		return true;
	}

	void TestIndexBuffer::Unbind()const
	{
	}

	uint32_t * TestIndexBuffer::Lock( uint32_t p_offset, uint32_t p_count, FlagCombination< AccessType > const & p_flags )const
	{
		return nullptr;
	}

	void TestIndexBuffer::Unlock()const
	{
	}

	bool TestIndexBuffer::Copy( GpuBuffer< uint32_t > const & p_src, uint32_t p_size )const
	{
		return true;
	}

	bool TestIndexBuffer::Upload( uint32_t p_offset, uint32_t p_size, uint32_t const * p_buffer )const
	{
		return true;
	}

	bool TestIndexBuffer::Download( uint32_t p_offset, uint32_t p_size, uint32_t * p_buffer )const
	{
		return true;
	}
}

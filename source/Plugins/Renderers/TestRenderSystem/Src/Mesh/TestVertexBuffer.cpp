#include "Mesh/TestVertexBuffer.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestVertexBuffer::TestVertexBuffer( TestRenderSystem & p_renderSystem )
		: GpuBuffer< uint8_t >( p_renderSystem )
	{
	}

	TestVertexBuffer::~TestVertexBuffer()
	{
	}

	bool TestVertexBuffer::Create()
	{
		return true;
	}

	void TestVertexBuffer::Destroy()
	{
	}

	bool TestVertexBuffer::InitialiseStorage( uint32_t p_count, BufferAccessType p_type, BufferAccessNature p_nature )const
	{
		return true;
	}

	bool TestVertexBuffer::InitialiseBindingPoint( uint32_t p_point )const
	{
		return true;
	}

	bool TestVertexBuffer::Bind()const
	{
		return true;
	}

	void TestVertexBuffer::Unbind()const
	{
	}

	uint8_t * TestVertexBuffer::Lock( uint32_t p_offset, uint32_t p_count, FlagCombination< AccessType > const & p_flags )const
	{
		return nullptr;
	}

	void TestVertexBuffer::Unlock()const
	{
	}

	bool TestVertexBuffer::Copy( GpuBuffer< uint8_t > const & p_src, uint32_t p_size )const
	{
		return true;
	}

	bool TestVertexBuffer::Upload( uint32_t p_offset, uint32_t p_size, uint8_t const * p_buffer )const
	{
		return true;
	}

	bool TestVertexBuffer::Download( uint32_t p_offset, uint32_t p_size, uint8_t * p_buffer )const
	{
		return true;
	}
}

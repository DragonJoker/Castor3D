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

	bool TestVertexBuffer::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature )
	{
		return true;
	}

	void TestVertexBuffer::Cleanup()
	{
	}

	bool TestVertexBuffer::Bind()
	{
		return true;
	}

	void TestVertexBuffer::Unbind()
	{
	}

	uint8_t * TestVertexBuffer::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		return nullptr;
	}

	void TestVertexBuffer::Unlock()
	{
	}

	bool TestVertexBuffer::Fill( uint8_t const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature )
	{
		return true;
	}
}

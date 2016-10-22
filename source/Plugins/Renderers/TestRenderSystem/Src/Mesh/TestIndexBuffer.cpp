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

	bool TestIndexBuffer::Upload( BufferAccessType p_type, BufferAccessNature p_nature )
	{
		return true;
	}

	bool TestIndexBuffer::Bind()
	{
		return true;
	}

	void TestIndexBuffer::Unbind()
	{
	}

	uint32_t * TestIndexBuffer::Lock( uint32_t p_offset, uint32_t p_count, AccessType p_flags )
	{
		return nullptr;
	}

	void TestIndexBuffer::Unlock()
	{
	}

	bool TestIndexBuffer::Fill( uint32_t const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )
	{
		return true;
	}
}

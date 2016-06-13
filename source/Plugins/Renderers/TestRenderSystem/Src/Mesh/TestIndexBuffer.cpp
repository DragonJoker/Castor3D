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

	bool TestIndexBuffer::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature )
	{
		return true;
	}

	bool TestIndexBuffer::AttachTo( ShaderProgramSPtr )
	{
		return true;
	}

	void TestIndexBuffer::Cleanup()
	{
	}

	bool TestIndexBuffer::Bind()
	{
		return true;
	}

	void TestIndexBuffer::Unbind()
	{
	}

	uint32_t * TestIndexBuffer::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		return nullptr;
	}

	void TestIndexBuffer::Unlock()
	{
	}

	bool TestIndexBuffer::Fill( uint32_t const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature )
	{
		return true;
	}
}

#include "TestBuffer.hpp"
#include "Render/TestRenderSystem.hpp"

namespace TestRender
{
	TestBuffer::TestBuffer( TestRenderSystem & p_renderSystem
		, Castor3D::BufferType p_type )
		: Castor3D::GpuBuffer( p_renderSystem )
		, m_type{ p_type }
	{
	}

	TestBuffer::~TestBuffer()
	{
	}

	bool TestBuffer::Create()
	{
		return true;
	}

	void TestBuffer::Destroy()
	{
	}

	void TestBuffer::InitialiseStorage( uint32_t p_count
		, Castor3D::BufferAccessType p_type
		, Castor3D::BufferAccessNature p_nature )const
	{
	}

	void TestBuffer::SetBindingPoint( uint32_t p_point )const
	{
	}

	uint32_t TestBuffer::GetBindingPoint()const
	{
		return 0u;
	}

	void TestBuffer::Bind()const
	{
	}

	void TestBuffer::Unbind()const
	{
	}

	uint8_t * TestBuffer::Lock( uint32_t p_offset
		, uint32_t p_count
		, Castor3D::AccessTypes const & p_flags )const
	{
		return nullptr;
	}

	void TestBuffer::Unlock()const
	{
	}

	void TestBuffer::Copy( Castor3D::GpuBuffer const & p_src
		, uint32_t p_size )const
	{
	}

	void TestBuffer::Upload( uint32_t p_offset
		, uint32_t p_size
		, uint8_t const * p_buffer )const
	{
	}

	void TestBuffer::Download( uint32_t p_offset
		, uint32_t p_size
		, uint8_t * p_buffer )const
	{
	}
}

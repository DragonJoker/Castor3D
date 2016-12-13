#include "Render/TestRenderSystem.hpp"

namespace TestRender
{
	template< typename T >
	TestBuffer< T >::TestBuffer( TestRenderSystem & p_renderSystem, Castor3D::BufferType p_type )
		: Castor3D::GpuBuffer< T >( p_renderSystem )
		, m_type{ p_type }
	{
	}

	template< typename T >
	TestBuffer< T >::~TestBuffer()
	{
	}

	template< typename T >
	bool TestBuffer< T >::Create()
	{
		return true;
	}

	template< typename T >
	void TestBuffer< T >::Destroy()
	{
	}

	template< typename T >
	void TestBuffer< T >::InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const
	{
	}

	template< typename T >
	void TestBuffer< T >::InitialiseBindingPoint( uint32_t p_point )const
	{
	}

	template< typename T >
	void TestBuffer< T >::Bind()const
	{
	}

	template< typename T >
	void TestBuffer< T >::Unbind()const
	{
	}

	template< typename T >
	T * TestBuffer< T >::Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessTypes const & p_flags )const
	{
		return nullptr;
	}

	template< typename T >
	void TestBuffer< T >::Unlock()const
	{
	}

	template< typename T >
	void TestBuffer< T >::Copy( Castor3D::GpuBuffer< T > const & p_src, uint32_t p_size )const
	{
	}

	template< typename T >
	void TestBuffer< T >::Upload( uint32_t p_offset, uint32_t p_size, T const * p_buffer )const
	{
	}

	template< typename T >
	void TestBuffer< T >::Download( uint32_t p_offset, uint32_t p_size, T * p_buffer )const
	{
	}
}

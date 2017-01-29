#include "Render/GlES3RenderSystem.hpp"

namespace GlES3Render
{
	template< typename T >
	GlES3Buffer< T >::GlES3Buffer( GlES3RenderSystem & p_renderSystem, OpenGlES3 & p_gl, GlES3BufferTarget p_target )
		: Castor3D::GpuBuffer< T >( p_renderSystem )
		, Holder{ p_gl }
		, m_glBuffer{ p_gl, p_target }
	{
	}

	template< typename T >
	GlES3Buffer< T >::~GlES3Buffer()
	{
	}

	template< typename T >
	bool GlES3Buffer< T >::Create()
	{
		return m_glBuffer.Create();
	}

	template< typename T >
	void GlES3Buffer< T >::Destroy()
	{
		m_glBuffer.Destroy();
	}

	template< typename T >
	void GlES3Buffer< T >::InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const
	{
		REQUIRE( p_count );
		m_glBuffer.InitialiseStorage( p_count, p_type, p_nature );
	}
	template< typename T >
	void GlES3Buffer< T >::SetBindingPoint( uint32_t p_index )const
	{
		m_glBuffer.SetBindingPoint( p_index );
	}

	template< typename T >
	void GlES3Buffer< T >::Bind()const
	{
		m_glBuffer.Bind();
	}

	template< typename T >
	void GlES3Buffer< T >::Unbind()const
	{
		m_glBuffer.Unbind();
	}

	template< typename T >
	void GlES3Buffer< T >::Copy( Castor3D::GpuBuffer< T > const & p_src, uint32_t p_size )const
	{
		m_glBuffer.Copy( static_cast< GlES3Buffer< T > const & >( p_src ).m_glBuffer, p_size );
	}

	template< typename T >
	void GlES3Buffer< T >::Upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const
	{
		m_glBuffer.Upload( p_offset, p_count, p_buffer );
	}

	template< typename T >
	void GlES3Buffer< T >::Download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const
	{
		m_glBuffer.Download( p_offset, p_count, p_buffer );
	}

	template< typename T >
	T * GlES3Buffer< T >::Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessTypes const & p_flags )const
	{
		return m_glBuffer.Lock( p_offset, p_count, p_flags );
	}

	template< typename T >
	void GlES3Buffer< T >::Unlock()const
	{
		m_glBuffer.Unlock();
	}
}

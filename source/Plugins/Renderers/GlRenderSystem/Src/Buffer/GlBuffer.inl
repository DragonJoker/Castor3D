#include "Render/GlRenderSystem.hpp"

namespace GlRender
{
	template< typename T >
	GlBuffer< T >::GlBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, GlBufferTarget p_target )
		: Castor3D::GpuBuffer< T >( p_renderSystem )
		, Holder{ p_gl }
		, m_glBuffer{ p_gl, p_target }
	{
	}

	template< typename T >
	GlBuffer< T >::~GlBuffer()
	{
	}

	template< typename T >
	bool GlBuffer< T >::Create()
	{
		return m_glBuffer.Create();
	}

	template< typename T >
	void GlBuffer< T >::Destroy()
	{
		m_glBuffer.Destroy();
	}

	template< typename T >
	bool GlBuffer< T >::InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const
	{
		bool l_return = true;

		if ( p_count )
		{
			l_return = m_glBuffer.InitialiseStorage( p_count, p_type, p_nature );
		}

		return l_return;
	}
	template< typename T >
	bool GlBuffer< T >::InitialiseBindingPoint( uint32_t p_index )const
	{
		bool l_return = m_glBuffer.Bind();

		if ( l_return )
		{
			m_glBuffer.SetBindingPoint( p_index );
			m_glBuffer.Unbind();
		}

		return l_return;
	}

	template< typename T >
	bool GlBuffer< T >::Bind()const
	{
		return m_glBuffer.Bind();
	}

	template< typename T >
	void GlBuffer< T >::Unbind()const
	{
		m_glBuffer.Unbind();
	}

	template< typename T >
	bool GlBuffer< T >::Copy( Castor3D::GpuBuffer< T > const & p_src, uint32_t p_size )const
	{
		return m_glBuffer.Copy( static_cast< GlBuffer< T > const & >( p_src ).m_glBuffer, p_size );
	}

	template< typename T >
	bool GlBuffer< T >::Upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const
	{
		return m_glBuffer.Upload( p_offset, p_count, p_buffer );
	}

	template< typename T >
	bool GlBuffer< T >::Download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const
	{
		return m_glBuffer.Download( p_offset, p_count, p_buffer );
	}

	template< typename T >
	T * GlBuffer< T >::Lock( uint32_t p_offset, uint32_t p_count, Castor::FlagCombination< Castor3D::AccessType > const & p_flags )const
	{
		return m_glBuffer.Lock( p_offset, p_count, p_flags );
	}

	template< typename T >
	void GlBuffer< T >::Unlock()const
	{
		m_glBuffer.Unlock();
	}
}

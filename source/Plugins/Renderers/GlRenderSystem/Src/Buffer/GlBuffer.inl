#include "Render/GlRenderSystem.hpp"

namespace GlRender
{
	template< typename T >
	GlBuffer< T >::GlBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, GlBufferTarget p_target, HardwareBufferPtr p_buffer )
		: Castor3D::GpuBuffer< T >( p_renderSystem )
		, Holder{ p_gl }
		, m_buffer{ p_buffer }
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
	bool GlBuffer< T >::Upload( Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )
	{
		bool l_return = true;
		HardwareBufferPtr l_cpuBuffer = GetCpuBuffer();

		if ( l_cpuBuffer && l_cpuBuffer->GetSize() )
		{
			l_return = m_glBuffer.Fill( &l_cpuBuffer->data()[0], l_cpuBuffer->GetSize(), p_type, p_nature );
		}

		return l_return;
	}

	template< typename T >
	bool GlBuffer< T >::Bind()
	{
		return m_glBuffer.Bind();
	}

	template< typename T >
	void GlBuffer< T >::Unbind()
	{
		m_glBuffer.Unbind();
	}

	template< typename T >
	bool GlBuffer< T >::Copy( Castor3D::GpuBuffer< T > const & p_src, uint32_t p_size )
	{
		return m_glBuffer.Copy( static_cast< GlBuffer< T > const & >( p_src ).m_glBuffer, p_size );
	}

	template< typename T >
	bool GlBuffer< T >::Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )
	{
		return m_glBuffer.Fill( p_buffer, p_size, p_type, p_nature );
	}

	template< typename T >
	T * GlBuffer< T >::Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessType p_flags )
	{
		return m_glBuffer.Lock( p_offset, p_count, p_flags );
	}

	template< typename T >
	void GlBuffer< T >::Unlock()
	{
		m_glBuffer.Unlock();
	}
}

#include "GlRenderSystem.hpp"

namespace GlRender
{
	template< typename T >
	GlBuffer< T >::GlBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, eGL_BUFFER_TARGET p_target, HardwareBufferPtr p_buffer )
		: Castor3D::GpuBuffer< T >( p_renderSystem )
		, Holder( p_gl )
		, m_pBuffer( p_buffer )
		, m_glBuffer( p_gl, p_target )
	{
	}

	template< typename T >
	GlBuffer< T >::~GlBuffer()
	{
	}

	template< typename T >
	bool GlBuffer< T >::Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature )
	{
		return m_glBuffer.Fill( p_buffer, p_size, p_type, p_nature );
	}

	template< typename T >
	bool GlBuffer< T >::DoCreate()
	{
		return m_glBuffer.Create();
	}

	template< typename T >
	void GlBuffer< T >::DoDestroy()
	{
		m_glBuffer.Destroy();
	}

	template< typename T >
	bool GlBuffer< T >::DoInitialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature )
	{
		bool l_return = true;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->GetSize() )
		{
			l_return = m_glBuffer.Initialise( &l_pBuffer->data()[0], l_pBuffer->GetSize(), p_type, p_nature );
		}

		if ( l_return )
		{
			l_pBuffer->Assign();
		}

		return l_return;
	}

	template< typename T >
	void GlBuffer< T >::DoCleanup()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer )
		{
			if ( l_pBuffer->IsAssigned() )
			{
				m_glBuffer.Cleanup();
				l_pBuffer->Unassign();
			}
		}
	}

	template< typename T >
	bool GlBuffer< T >::DoBind()
	{
		return m_glBuffer.Bind();
	}

	template< typename T >
	void GlBuffer< T >::DoUnbind()
	{
		m_glBuffer.Unbind();
	}

	template< typename T >
	bool GlBuffer< T >::DoFill( T * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature )
	{
		return m_glBuffer.Fill( p_buffer, p_size, p_type, p_nature );
	}

	template< typename T >
	T * GlBuffer< T >::DoLock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		return m_glBuffer.Lock( p_offset, p_count, p_flags );
	}

	template< typename T >
	T * GlBuffer< T >::DoLock( eGL_LOCK p_access )
	{
		return m_glBuffer.Lock( p_access );
	}

	template< typename T >
	void GlBuffer< T >::DoUnlock()
	{
		m_glBuffer.Unlock();
	}
}

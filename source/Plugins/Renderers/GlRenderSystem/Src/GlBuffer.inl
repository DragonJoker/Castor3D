#include "GlRenderSystem.hpp"

namespace GlRender
{
	template< typename T >
	GlBuffer< T >::GlBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget, HardwareBufferPtr p_pBuffer )
		: Castor3D::GpuBuffer< T >( p_renderSystem )
		, m_pBuffer( p_pBuffer )
		, m_glBuffer( p_gl, p_eTarget )
		, m_gl( p_gl )
	{
	}

	template< typename T >
	GlBuffer< T >::~GlBuffer()
	{
	}

	template< typename T >
	bool GlBuffer< T >::Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
	{
		return m_glBuffer.Fill( p_pBuffer, p_iSize, p_type, p_eNature );
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
	bool GlBuffer< T >::DoInitialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
	{
		bool l_return = true;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->GetSize() )
		{
			l_return = m_glBuffer.Initialise( &l_pBuffer->data()[0], l_pBuffer->GetSize(), p_type, p_eNature );
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
	bool GlBuffer< T >::DoFill( T * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
	{
		return m_glBuffer.Fill( p_pBuffer, p_iSize, p_type, p_eNature );
	}

	template< typename T >
	T * GlBuffer< T >::DoLock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		return m_glBuffer.Lock( p_uiOffset, p_uiCount, p_uiFlags );
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

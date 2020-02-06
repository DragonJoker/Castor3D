namespace GlRender
{
	template< typename T >
	GlBufferBase< T >::GlBufferBase( OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget )
		:	m_uiGlIndex( uint32_t( eGL_INVALID_INDEX )	)
		,	m_eTarget( p_eTarget	)
		,	m_gl( p_gl	)
	{
	}

	template< typename T >
	GlBufferBase< T >::~GlBufferBase()
	{
	}

	template< typename T >
	bool GlBufferBase< T >::Create()
	{
		bool l_bReturn = true;

		if ( m_uiGlIndex == eGL_INVALID_INDEX )
		{
			l_bReturn = m_gl.GenBuffers( 1, &m_uiGlIndex );
		}

		return l_bReturn;
	}

	template< typename T >
	void GlBufferBase< T >::Destroy()
	{
		if ( m_uiGlIndex != eGL_INVALID_INDEX )
		{
			m_gl.DeleteBuffers( 1, &m_uiGlIndex );
			m_uiGlIndex = uint32_t( eGL_INVALID_INDEX );
		}
	}

	template< typename T >
	bool GlBufferBase< T >::Initialise( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
	{
		bool l_bResult = Bind();

		if ( l_bResult )
		{
			l_bResult = Fill( p_pBuffer, p_iSize, p_eType, p_eNature );
			Unbind();
		}

		return l_bResult;
	}

	template< typename T >
	void GlBufferBase< T >::Cleanup()
	{
	}

	template< typename T >
	bool GlBufferBase< T >::Bind()
	{
		return m_gl.BindBuffer( m_eTarget, m_uiGlIndex );
	}

	template< typename T >
	void GlBufferBase< T >::Unbind()
	{
		m_gl.BindBuffer( m_eTarget, 0 );
	}

	template< typename T >
	bool GlBufferBase< T >::Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
	{
		return m_gl.BufferData( m_eTarget, p_iSize * sizeof( T ), p_pBuffer, m_gl.GetBufferFlags( p_eNature | p_eType ) );
	}

	template< typename T >
	T * GlBufferBase< T >::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		T * l_pReturn = NULL;

		if ( m_uiGlIndex != eGL_INVALID_INDEX )
		{
			l_pReturn = reinterpret_cast< T * >( m_gl.MapBufferRange( m_eTarget, p_uiOffset * sizeof( T ), p_uiCount * sizeof( T ), m_gl.GetBitfieldFlags( p_uiFlags ) ) );
		}

		return l_pReturn;
	}

	template< typename T >
	T * GlBufferBase< T >::Lock( eGL_LOCK p_access )
	{
		T * l_pReturn = NULL;

		if ( m_uiGlIndex != eGL_INVALID_INDEX )
		{
			l_pReturn = reinterpret_cast< T * >( m_gl.MapBuffer( m_eTarget, p_access ) );
		}

		return l_pReturn;
	}

	template< typename T >
	bool GlBufferBase< T >::Unlock()
	{
		bool l_bReturn = m_uiGlIndex != eGL_INVALID_INDEX;
		l_bReturn &= m_gl.UnmapBuffer( m_eTarget );
		return l_bReturn;
	}
}

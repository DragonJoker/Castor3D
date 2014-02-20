//*************************************************************************************************

template< typename T > struct GlTyper;

template <> struct GlTyper< int			>	{	enum { Value = eGL_TYPE_INT				};	};
template <> struct GlTyper< uint32_t	>	{	enum { Value = eGL_TYPE_UNSIGNED_INT	};	};
template <> struct GlTyper< short		>	{	enum { Value = eGL_TYPE_SHORT			};	};
template <> struct GlTyper< uint16_t	>	{	enum { Value = eGL_TYPE_UNSIGNED_SHORT	};	};
template <> struct GlTyper< char		>	{	enum { Value = eGL_TYPE_BYTE			};	};
template <> struct GlTyper< uint8_t		>	{	enum { Value = eGL_TYPE_UNSIGNED_BYTE	};	};
template <> struct GlTyper< float		>	{	enum { Value = eGL_TYPE_FLOAT			};	};
template <> struct GlTyper< double		>	{	enum { Value = eGL_TYPE_DOUBLE			};	};

//*************************************************************************************************

template< typename T >
GlBufferBase< T > :: GlBufferBase( OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget )
	:	m_uiGlIndex	( uint32_t( eGL_INVALID_INDEX )	)
	,	m_eTarget	( p_eTarget						)
	,	m_gl		( p_gl							)
{
}

template< typename T >
GlBufferBase< T > :: ~GlBufferBase()
{
}

template< typename T >
bool GlBufferBase< T > :: Create()
{
	bool l_bReturn = true;

	if( m_uiGlIndex == eGL_INVALID_INDEX )
	{
		l_bReturn = m_gl.GenBuffers( 1, &m_uiGlIndex );
	}

	return l_bReturn;
}

template< typename T >
void GlBufferBase< T > :: Destroy()
{
	if( m_uiGlIndex != eGL_INVALID_INDEX )
	{
		m_gl.DeleteBuffers( 1, &m_uiGlIndex );
		m_uiGlIndex = uint32_t( eGL_INVALID_INDEX );
	}
}

template< typename T >
bool GlBufferBase< T > :: Initialise( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
{
	bool l_bResult = Bind();

	if( l_bResult )
	{
		l_bResult = Fill( p_pBuffer, p_iSize, p_eType, p_eNature );
		Unbind();
	}

	return l_bResult;
}

template< typename T >
void GlBufferBase< T > :: Cleanup()
{
}

template< typename T >
bool GlBufferBase< T > :: Bind()
{
	return m_gl.BindBuffer( m_eTarget, m_uiGlIndex );
}

template< typename T >
void GlBufferBase< T > :: Unbind()
{
	m_gl.BindBuffer( m_eTarget, 0 );
}

template< typename T >
bool GlBufferBase< T > :: Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
{
	return m_gl.BufferData( m_eTarget, p_iSize * sizeof( T ), p_pBuffer, m_gl.GetBufferFlags( p_eNature | p_eType ) );
}

template< typename T >
T * GlBufferBase< T > :: Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	T * l_pReturn = NULL;

	if( m_uiGlIndex != eGL_INVALID_INDEX )
	{
		l_pReturn = reinterpret_cast< T* >( m_gl.MapBufferRange( m_eTarget, p_uiOffset * sizeof( T ), p_uiCount * sizeof( T ), m_gl.GetBitfieldFlags( p_uiFlags ) ) );
	}

	return l_pReturn;
}

template< typename T >
T * GlBufferBase< T > :: Lock( eGL_LOCK p_access )
{
	T * l_pReturn = NULL;

	if( m_uiGlIndex != eGL_INVALID_INDEX )
	{
		l_pReturn = reinterpret_cast< T* >( m_gl.MapBuffer( m_eTarget, p_access ) );
	}

	return l_pReturn;
}

template< typename T >
bool GlBufferBase< T > :: Unlock()
{
	bool l_bReturn = m_uiGlIndex != eGL_INVALID_INDEX;
	l_bReturn &= m_gl.UnmapBuffer( m_eTarget );
	return l_bReturn;
}

//*************************************************************************************************

template< typename T >
GlBuffer< T > :: GlBuffer( OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget, HardwareBufferSPtr p_pBuffer)
	:	m_wpBuffer	( p_pBuffer			)
	,	m_glBuffer	( p_gl, p_eTarget	)
	,	m_gl		( p_gl				)
{
}

template< typename T >
GlBuffer< T > :: ~GlBuffer()
{
}

template< typename T >
bool GlBuffer< T > :: Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
{
	return m_glBuffer.Fill( p_pBuffer, p_iSize, p_eType, p_eNature );
}

template< typename T >
bool GlBuffer< T > :: DoCreate()
{
	return m_glBuffer.Create();
}

template< typename T >
void GlBuffer< T > :: DoDestroy()
{
	m_glBuffer.Destroy();
}

template< typename T >
bool GlBuffer< T > :: DoInitialise( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
{
	bool l_bReturn = true;
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer && l_pBuffer->GetSize() )
	{
		l_bReturn = m_glBuffer.Initialise( &l_pBuffer->data()[0], l_pBuffer->GetSize(), p_eType, p_eNature );
	}

	if( l_bReturn )
	{
		l_pBuffer->Assign();
	}

	return l_bReturn;
}

template< typename T >
void GlBuffer< T > :: DoCleanup()
{
	HardwareBufferSPtr l_pBuffer = GetCpuBuffer();

	if( l_pBuffer )
	{
		if( l_pBuffer->IsAssigned() )
		{
			m_glBuffer.Cleanup();
			l_pBuffer->Unassign();
		}
	}
}

template< typename T >
bool GlBuffer< T > :: DoBind()
{
	return m_glBuffer.Bind();
}

template< typename T >
void GlBuffer< T > :: DoUnbind()
{
	m_glBuffer.Unbind();
}

template< typename T >
bool GlBuffer< T > :: DoFill( T * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
{
	return m_glBuffer.Fill( p_pBuffer, p_iSize, p_eType, p_eNature );
}

template< typename T >
T * GlBuffer< T > :: DoLock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags)
{
	return m_glBuffer.Lock( p_uiOffset, p_uiCount, p_uiFlags );
}

template< typename T >
T * GlBuffer< T > :: DoLock( eGL_LOCK p_access)
{
	return m_glBuffer.Lock( p_access );
}

template< typename T >
void GlBuffer< T > :: DoUnlock()
{
	m_glBuffer.Unlock();
}

//*************************************************************************************************

template <typename T, uint32_t Count>
GlAttribute<T, Count> :: GlAttribute( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, Castor::String const & p_strAttribName)
	:	GlAttributeBase( p_gl, p_pRenderSystem, p_strAttribName, eGL_TYPE( GlTyper< T >::Value ), Count)
{
}

template <typename T, uint32_t Count>
GlAttribute<T, Count> :: ~GlAttribute()
{
}

//*************************************************************************************************

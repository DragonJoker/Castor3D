//*************************************************************************************************

template< typename T, class D3dBufferObject >
DxBufferObject< T, D3dBufferObject > :: DxBufferObject( HardwareBufferPtr p_pBuffer)
	:	m_uiIndex		( BuffersCount	)
	,	m_pBuffer		( p_pBuffer		)
	,	m_pBufferObject	( NULL			)
{
}

template< typename T, class D3dBufferObject >
DxBufferObject< T, D3dBufferObject > :: ~DxBufferObject()
{
}

template< typename T, class D3dBufferObject >
bool DxBufferObject< T, D3dBufferObject > :: Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE /*p_eType*/, Castor3D::eBUFFER_ACCESS_NATURE /*p_eNature*/ )
{
	DoFill( p_pBuffer, uint32_t( p_iSize ) );
	return true;
}

template< typename T, class D3dBufferObject >
void DxBufferObject< T, D3dBufferObject > :: DoDestroy()
{
}

template< typename T, class D3dBufferObject >
void DxBufferObject< T, D3dBufferObject > :: DoCleanup()
{
	if( m_pBuffer )
	{
		if( m_pBuffer->IsAssigned() )
		{
			m_pBuffer->Unassign();
		}
	}

	SafeRelease( m_pBufferObject );
}

template< typename T, class D3dBufferObject >
T * DxBufferObject< T, D3dBufferObject > :: DoLock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
{
	T * l_pReturn = NULL;

	if( m_pBufferObject )
	{
		HRESULT l_hr = m_pBufferObject->Lock( p_uiOffset * sizeof( T ), p_uiCount * sizeof( T ), reinterpret_cast< void** >( &l_pReturn ), DirectX9::GetLockFlags( p_uiFlags ) );
		dxCheckError( l_hr, "DxBufferObject :: Lock" );
	}

	return l_pReturn;
}

template< typename T, class D3dBufferObject >
void DxBufferObject< T, D3dBufferObject > :: DoFill( T const * p_pValues, uint32_t p_uiCount )
{
	if( m_pBufferObject )
	{
		if( p_uiCount )
		{
			T * l_pBuffer = Lock( 0, p_uiCount, Castor3D::eBUFFER_ACCESS_NATURE_DRAW );

			if( l_pBuffer )
			{
				memcpy( l_pBuffer, p_pValues, p_uiCount * sizeof( T ) );
				Unlock();
			}
		}
	}
}

template< typename T, class D3dBufferObject >
void DxBufferObject< T, D3dBufferObject > :: DoUnlock()
{
	if( m_pBufferObject )
	{
		HRESULT l_hr = m_pBufferObject->Unlock();
		dxCheckError( l_hr, "DxBufferObject :: Unlock" );
	}
}

//*************************************************************************************************

namespace Dx11Render
{
	template< typename T, class D3dBufferObject >
	DxBufferObject< T, D3dBufferObject >::DxBufferObject( HardwareBufferPtr p_pBuffer )
		:	m_uiIndex( BuffersCount )
		,	m_pBuffer( p_pBuffer )
		,	m_pBufferObject( NULL )
	{
	}

	template< typename T, class D3dBufferObject >
	DxBufferObject< T, D3dBufferObject >::~DxBufferObject()
	{
	}

	template< typename T, class D3dBufferObject >
	bool DxBufferObject< T, D3dBufferObject >::Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
	{
		bool l_bReturn = false;
		T * l_pReturn = NULL;
		DirectX11::LockBuffer( l_pReturn, m_pBufferObject, 0, p_iSize, DirectX11::GetBufferMapMode( p_eNature ) );

		if ( l_pReturn )
		{
			std::memcpy( l_pReturn, p_pBuffer, p_iSize );
			DirectX11::UnlockBuffer( m_pBufferObject );
			l_bReturn = true;
		}

		return l_bReturn;
	}

	template< typename T, class D3dBufferObject >
	void DxBufferObject< T, D3dBufferObject >::DoDestroy()
	{
	}

	template< typename T, class D3dBufferObject >
	void DxBufferObject< T, D3dBufferObject >::DoCleanup()
	{
		if ( m_pBuffer )
		{
			m_pBuffer->Unassign();
		}

		ReleaseTracked( m_pBuffer->GetRenderSystem(), m_pBufferObject );
	}

	template< typename T, class D3dBufferObject >
	T * DxBufferObject< T, D3dBufferObject >::DoLock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		T * l_pReturn = NULL;
		DirectX11::LockBuffer( l_pReturn, m_pBufferObject, uint32_t( p_uiOffset * sizeof( T ) ), p_uiCount * sizeof( T ), p_uiFlags );
		return l_pReturn;
	}

	template< typename T, class D3dBufferObject >
	void DxBufferObject< T, D3dBufferObject >::DoUnlock()
	{
		DirectX11::UnlockBuffer( m_pBufferObject );
	}
}

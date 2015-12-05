namespace Dx11Render
{
	template< typename T, class D3dBufferObject >
	DxBufferObject< T, D3dBufferObject >::DxBufferObject( DxRenderSystem & p_renderSystem, HardwareBufferPtr p_buffer )
		: GpuBuffer< T >( p_renderSystem )
		, m_index( BuffersCount )
		, m_pBuffer( p_buffer )
		, m_pBufferObject( NULL )
	{
	}

	template< typename T, class D3dBufferObject >
	DxBufferObject< T, D3dBufferObject >::~DxBufferObject()
	{
	}

	template< typename T, class D3dBufferObject >
	bool DxBufferObject< T, D3dBufferObject >::Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature )
	{
		bool l_return = false;
		T * l_buffer;
		DirectX11::LockBuffer( l_buffer, m_pBufferObject, 0, p_size, DirectX11::GetBufferMapMode( p_nature ) );

		if ( l_buffer )
		{
			std::memcpy( l_buffer, p_buffer, p_size );
			DirectX11::UnlockBuffer( m_pBufferObject );
			l_return = true;
		}

		return l_return;
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

		ReleaseTracked( m_pBuffer->GetOwner()->GetRenderSystem(), m_pBufferObject );
	}

	template< typename T, class D3dBufferObject >
	T * DxBufferObject< T, D3dBufferObject >::DoLock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		T * l_return = NULL;
		DirectX11::LockBuffer( l_return, m_pBufferObject, uint32_t( p_offset * sizeof( T ) ), p_count * sizeof( T ), p_flags );
		return l_return;
	}

	template< typename T, class D3dBufferObject >
	void DxBufferObject< T, D3dBufferObject >::DoUnlock()
	{
		DirectX11::UnlockBuffer( m_pBufferObject );
	}
}

#include "ComCastor3D/Castor3D/ComCamera.hpp"

namespace CastorCom
{
	namespace camera
	{
		static const tstring ERROR_UNINITIALISED = _T( "The camera must be initialised" );
	}

	STDMETHODIMP CCamera::Create( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ UINT width, /*[in]*/ UINT height )noexcept
	{
		if ( !scene || !name || !parent )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dCamera_create( static_cast< CScene * >( scene )->getInternal()
			, bstrToString( name ).c_str()
			, width, height
			, static_cast< CSceneNode * >( parent )->getInternal()
			, &m_internal ) );
	}

	STDMETHODIMP CCamera::AttachTo( /*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AttachTo" ) );

		return convert( c3dCamera_attachTo( m_internal
			, static_cast< CSceneNode * >( val )->getInternal() ) );
	}

	STDMETHODIMP CCamera::Detach()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Detach" ) );

		return convert( c3dCamera_detach( m_internal ) );
	}

	STDMETHODIMP CCamera::Resize( /*[in]*/ UINT width, /*[in]*/ UINT height )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Resize" ) );

		return convert( c3dCamera_resize( m_internal, width, height ) );
	}
}

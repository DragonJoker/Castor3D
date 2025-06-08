#include "ComCastor3D/Castor3D/ComCamera.hpp"

namespace CastorCom
{
	namespace camera
	{
		static const tstring ERROR_UNINITIALISED = _T( "The camera must be initialised" );
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

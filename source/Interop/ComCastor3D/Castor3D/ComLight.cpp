#include "ComCastor3D/Castor3D/ComLight.hpp"

namespace CastorCom
{
	STDMETHODIMP CLight::Create( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ eLIGHT_TYPE type )noexcept
	{
		if ( !scene || !name || !parent )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dLight_create( static_cast< CScene * >( scene )->getInternal()
			, bstrToString( name ).c_str()
			, static_cast< CSceneNode * >( parent )->getInternal()
			, details::parameterCast< C3D_LIGHT_TYPE >( type )
			, &m_internal ) );
	}

	STDMETHODIMP CLight::AttachTo( /*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AttachTo" ) );

		return convert( c3dLight_attachTo( m_internal
			, static_cast< CSceneNode * >( val )->getInternal() ) );
	}

	STDMETHODIMP CLight::Detach()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Detach" ) );

		return convert( c3dLight_detach( m_internal ) );
	}
}

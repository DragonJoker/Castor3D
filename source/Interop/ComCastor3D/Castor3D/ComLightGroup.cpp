#include "ComCastor3D/Castor3D/ComLightGroup.hpp"

#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
	STDMETHODIMP CLightGroup::Create( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ eLIGHT_TYPE type )noexcept
	{
		if ( !scene || !name )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dLightGroup_create( static_cast< CScene * >( scene )->getInternal()
			, bstrToString( name ).c_str()
			, details::parameterCast< C3D_LIGHT_TYPE >( type )
			, &m_internal ) );
	}

	STDMETHODIMP CLightGroup::AddLight( /*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddLight" ) );

		return convert( c3dLightGroup_addLight( m_internal
			, static_cast< CSceneNode * >( val )->getInternal() ) );
	}

	STDMETHODIMP CLightGroup::RemoveLight(/*[in]*/ ISceneNode * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveLight" ) );

		return convert( c3dLightGroup_removeLight( m_internal
			, static_cast< CSceneNode * >( val )->getInternal() ) );
	}
}

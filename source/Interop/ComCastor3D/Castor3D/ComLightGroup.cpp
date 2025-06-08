#include "ComCastor3D/Castor3D/ComLightGroup.hpp"

#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
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

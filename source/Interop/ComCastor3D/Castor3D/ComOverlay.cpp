#include "ComCastor3D/Castor3D/ComOverlay.hpp"
#include "ComCastor3D/Castor3D/ComEngine.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"

namespace CastorCom
{
	STDMETHODIMP COverlay::Create( /*[in]*/ IEngine * engine, /*[in]*/ eOVERLAY_TYPE type, /*[in]*/ BSTR name, /*[in]*/ IOverlay * parent, /*[in]*/ IScene * scene )noexcept
	{
		if ( !engine || !name )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dOverlay_create( static_cast< CEngine * >( engine )->getInternal()
			, details::parameterCast< C3D_OVERLAY_TYPE >( type )
			, bstrToString( name ).c_str()
			, parent ? static_cast< COverlay * >( parent )->getInternal() : nullptr
			, scene ? static_cast< CScene * >( scene )->getInternal() : nullptr
			, &m_internal ) );
	}

	STDMETHODIMP COverlay::GetChildrenCount( /*[in]*/ UINT level, /*[out, retval]*/ UINT * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetChildrenCount" ) );

		return convert( c3dOverlay_getChildrenCount( m_internal, level, pRet ) );
	}
}

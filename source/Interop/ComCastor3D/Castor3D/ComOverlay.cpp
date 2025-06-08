#include "ComCastor3D/Castor3D/ComOverlay.hpp"
#include "ComCastor3D/Castor3D/ComEngine.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"

namespace CastorCom
{
	STDMETHODIMP COverlay::GetChildrenCount( /*[in]*/ UINT level, /*[out, retval]*/ UINT * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetChildrenCount" ) );

		return convert( c3dOverlay_getChildrenCount( m_internal, level, pRet ) );
	}
}

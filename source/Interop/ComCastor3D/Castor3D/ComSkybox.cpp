#include "ComCastor3D/Castor3D/ComSkybox.hpp"

namespace CastorCom
{
	STDMETHODIMP CSkybox::SetEquirectangularImage( /*[in]*/ BSTR filePath, /*[in]*/ UINT size )noexcept
	{
		if ( !filePath )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "SetEquirectangularImage" ) );

		return convert( c3dSkybox_setEquirectangularImage( m_internal
			, bstrToString( filePath ).c_str()
			, size ) );
	}
}

#include "ComCastor3D/Castor3D/ComLineMapping.hpp"

namespace CastorCom
{
	STDMETHODIMP CLineMapping::AddLine( /*[in]*/ UINT x, /*[in]*/ UINT y )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddLine" ) );

		return convert( c3dLineMapping_addLine( m_internal, x, y ) );
	}
}

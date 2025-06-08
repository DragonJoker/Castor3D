#include "ComCastor3D/Castor3D/ComTriFaceMapping.hpp"

namespace CastorCom
{
	STDMETHODIMP CTriFaceMapping::AddFace( /*[in]*/ UINT x, /*[in]*/ UINT y, /*[in]*/ UINT z )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddFace" ) );

		return convert( c3dTriFaceMapping_addFace( m_internal, x, y, z ) );
	}
}

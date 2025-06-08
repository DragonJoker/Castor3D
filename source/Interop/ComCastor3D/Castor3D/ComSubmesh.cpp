#include "ComCastor3D/Castor3D/ComSubmesh.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	STDMETHODIMP CSubmesh::AddPoint( /*[in]*/ IVector3D * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddPoint" ) );

		return convert( c3dSubmesh_addPoint( m_internal, &static_cast< CVector3D * >( val )->getInternal() ) );
	}
}

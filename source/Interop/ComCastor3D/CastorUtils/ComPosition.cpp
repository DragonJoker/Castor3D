#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CPosition::Set( /* [in] */ INT x, /* [in] */ INT y )noexcept
	{
		m_internal.set( x, y );
		return S_OK;
	}

	STDMETHODIMP CPosition::Offset( /* [in] */ INT x, /* [in] */ INT y )noexcept
	{
		m_internal.offset( x, y );
		return S_OK;
	}
}

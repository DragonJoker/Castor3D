#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CSize::Set( /* [in] */ UINT x, /* [in] */ UINT y )noexcept
	{
		m_internal.set( x, y );
		return S_OK;
	}

	STDMETHODIMP CSize::Grow( /* [in] */ INT x, /* [in] */ INT y )noexcept
	{
		m_internal.grow( x, y );
		return S_OK;
	}
}

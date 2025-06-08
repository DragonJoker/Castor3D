#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	STDMETHODIMP CSize::Set( /*[in]*/ UINT x, /*[in]*/ UINT y )noexcept
	{
		m_internal.width = x;
		m_internal.height = y;
		return S_OK;
	}

	STDMETHODIMP CSize::Grow( /*[in]*/ INT x, /*[in]*/ INT y )noexcept
	{
		m_internal.width += x;
		m_internal.height += y;
		return S_OK;
	}
}

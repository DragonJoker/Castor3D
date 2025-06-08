#include "ComCastor3D/CastorUtils/ComPosition.hpp"

namespace CastorCom
{
	STDMETHODIMP CPosition::Set( /*[in]*/ INT x, /*[in]*/ INT y )noexcept
	{
		m_internal.x = x;
		m_internal.y = y;
		return S_OK;
	}

	STDMETHODIMP CPosition::Offset( /*[in]*/ INT x, /*[in]*/ INT y )noexcept
	{
		m_internal.x += x;
		m_internal.y += y;
		return S_OK;
	}
}

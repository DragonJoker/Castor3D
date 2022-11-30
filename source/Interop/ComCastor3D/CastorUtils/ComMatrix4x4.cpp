#include "ComCastor3D/CastorUtils/ComMatrix4x4.hpp"

namespace CastorCom
{
	STDMETHODIMP CMatrix4x4::Transpose()noexcept
	{
		m_internal.transpose();
		return S_OK;
	}

	STDMETHODIMP CMatrix4x4::Invert()noexcept
	{
		m_internal.invert();
		return S_OK;
	}
}

#include "ComCastor3D/CastorUtils/ComMatrix4x4.hpp"

namespace CastorCom
{
	CMatrix4x4::CMatrix4x4()
	{
	}

	CMatrix4x4::~CMatrix4x4()
	{
	}

	STDMETHODIMP CMatrix4x4::Transpose()
	{
		castor::Matrix4x4f::transpose();
		return S_OK;
	}

	STDMETHODIMP CMatrix4x4::Invert()
	{
		castor::Matrix4x4f::invert();
		return S_OK;
	}
}

#include "ComMatrix.hpp"

namespace CastorCom
{
	CMatrix::CMatrix()
	{
	}

	CMatrix::~CMatrix()
	{
	}

	STDMETHODIMP CMatrix::Transpose()
	{
		Castor::Matrix4x4r::transpose();
		return S_OK;
	}

	STDMETHODIMP CMatrix::Invert()
	{
		Castor::Matrix4x4r::invert();
		return S_OK;
	}
}

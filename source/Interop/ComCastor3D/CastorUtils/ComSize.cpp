#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	CSize::CSize()
	{
	}

	CSize::~CSize()
	{
	}

	STDMETHODIMP CSize::Set( /* [in] */ UINT x, /* [in] */ UINT y )
	{
		this->set( x, y );
		return S_OK;
	}

	STDMETHODIMP CSize::Grow( /* [in] */ INT x, /* [in] */ INT y )
	{
		this->grow( x, y );
		return S_OK;
	}
}

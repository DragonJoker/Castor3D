#include "ComSize.hpp"
#include "ComUtils.hpp"

namespace CastorCom
{
	CSize::CSize()
	{
	}

	CSize::~CSize()
	{
	}

	STDMETHODIMP CSize::set( /* [in] */ UINT x, /* [in] */ UINT y )
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

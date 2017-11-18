#include "ComPosition.hpp"
#include "ComUtils.hpp"

namespace CastorCom
{
	CPosition::CPosition()
	{
	}

	CPosition::~CPosition()
	{
	}

	STDMETHODIMP CPosition::Set( /* [in] */ INT x, /* [in] */ INT y )
	{
		this->set( x, y );
		return S_OK;
	}

	STDMETHODIMP CPosition::Offset( /* [in] */ INT x, /* [in] */ INT y )
	{
		this->offset( x, y );
		return S_OK;
	}
}

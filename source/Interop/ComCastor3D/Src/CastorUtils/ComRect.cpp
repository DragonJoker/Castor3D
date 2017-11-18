#include "ComRect.hpp"
#include "ComPosition.hpp"

namespace CastorCom
{
	CRect::CRect()
	{
	}

	CRect::~CRect()
	{
	}

	STDMETHODIMP CRect::Set( /* [in] */ INT left, /* [in] */ INT top, /* [in] */ INT right, /* [in] */ INT bottom )
	{
		this->set( left, top, right, bottom );
		return S_OK;
	}

	STDMETHODIMP CRect::IntersectsPosition( /* [in] */ IPosition * pos, /* [in] */ eINTERSECTION * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			*pVal = eINTERSECTION( this->intersects( *reinterpret_cast< CPosition * >( pos ) ) );
			hr = S_OK;
		}

		return S_OK;
	}

	STDMETHODIMP CRect::IntersectsRectangle( /* [in] */ IRect * rect, /* [in] */ eINTERSECTION * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			*pVal = eINTERSECTION( this->intersects( *reinterpret_cast< CRect * >( rect ) ) );
			hr = S_OK;
		}

		return S_OK;
	}
}

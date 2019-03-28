#include "ComCastor3D/CastorUtils/ComVector3D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	CVector3D::CVector3D()
	{
	}

	CVector3D::~CVector3D()
	{
	}

	STDMETHODIMP CVector3D::Negate()
	{
		castor::point::negate( *this );
		return S_OK;
	}

	STDMETHODIMP CVector3D::Normalise()
	{
		castor::point::normalise( *this );
		return S_OK;
	}

	STDMETHODIMP CVector3D::Dot( IVector3D * pVal, FLOAT * pRet )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( *this, *reinterpret_cast< CVector3D * >( pVal ) );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Cross( IVector3D * pVal, IVector3D ** pRet )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			hr = CVector3D::CreateInstance( pRet );

			if ( hr == S_OK )
			{
				CVector3D * ret = reinterpret_cast< CVector3D * >( *pRet );
				castor::Point3f tmp = castor::point::cross( *this, *reinterpret_cast< CVector3D * >( pVal ) );
				std::memcpy( ret->ptr(), tmp.ptr(), tmp.size() );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Length( FLOAT * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( *this ) );
			hr = S_OK;
		}

		return hr;
	}
}

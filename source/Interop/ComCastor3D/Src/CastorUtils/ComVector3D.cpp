#include "ComVector3D.hpp"
#include "ComUtils.hpp"

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
		Castor::point::negate( *this );
		return S_OK;
	}

	STDMETHODIMP CVector3D::Normalise()
	{
		Castor::point::normalise( *this );
		return S_OK;
	}

	STDMETHODIMP CVector3D::Dot( IVector3D * pVal, FLOAT * pRet )
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = Castor::point::dot( *this, *reinterpret_cast< CVector3D * >( pVal ) );
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
				Castor::Point3f tmp = *this ^ *reinterpret_cast< CVector3D * >( pVal );
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
			*pVal = FLOAT( Castor::point::distance( *this ) );
			hr = S_OK;
		}

		return hr;
	}
}

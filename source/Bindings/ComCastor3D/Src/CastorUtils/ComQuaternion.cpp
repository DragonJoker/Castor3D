#include "ComQuaternion.hpp"
#include "ComMatrix.hpp"
#include "ComVector3D.hpp"
#include "ComAngle.hpp"

namespace CastorCom
{
	CQuaternion::CQuaternion()
	{
	}

	CQuaternion::~CQuaternion()
	{
	}

	STDMETHODIMP CQuaternion::Transform( /* [in] */ IVector3D * val, /* [out, retval] */ IVector3D ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( val && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				Castor::Quaternion::Transform( *static_cast< CVector3D * >( val ), *static_cast< CVector3D * >( *pVal ) );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::ToAxisAngle( /* [out] */ IVector3D ** pAxis, /* [out] */ IAngle ** pAngle )
	{
		HRESULT hr = E_POINTER;

		if ( pAxis && pAngle )
		{
			hr = CVector3D::CreateInstance( pAxis );
			hr = hr == S_OK ? CAngle::CreateInstance( pAngle ) : hr;

			if ( hr == S_OK )
			{
				Castor::Quaternion::ToAxisAngle( *static_cast< CVector3D * >( *pAxis ), *static_cast< CAngle * >( *pAngle ) );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::FromAxisAngle( /* [in] */ IVector3D * axis, /* [in] */ IAngle * angle )
	{
		HRESULT hr = E_POINTER;

		if ( axis && angle )
		{
			Castor::Quaternion::FromAxisAngle( *static_cast< CVector3D * >( axis ), *static_cast< CAngle * >( angle ) );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::ToAxes( /* [out] */ IVector3D ** pX, /* [out] */ IVector3D ** pY, /* [out] */ IVector3D ** pZ )
	{
		HRESULT hr = E_POINTER;

		if ( pX && pY && pZ )
		{
			hr = CVector3D::CreateInstance( pX );
			hr = hr == S_OK ? CVector3D::CreateInstance( pY ) : hr;
			hr = hr == S_OK ? CVector3D::CreateInstance( pZ ) : hr;

			if ( hr == S_OK )
			{
				Castor::Quaternion::ToAxes( *static_cast< CVector3D * >( *pX ), *static_cast< CVector3D * >( *pY ), *static_cast< CVector3D * >( *pZ ) );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::FromAxes( /* [in] */ IVector3D * x, /* [in] */ IVector3D * y, /* [in] */ IVector3D * z )
	{
		HRESULT hr = E_POINTER;

		if ( x && y && z )
		{
			Castor::Quaternion::FromAxes( *static_cast< CVector3D * >( x ), *static_cast< CVector3D * >( y ), *static_cast< CVector3D * >( z ) );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::GetYaw( /* [out, retval] */ IAngle ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			hr = CAngle::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< Castor::Angle >( *static_cast< CAngle * >( *pVal ) ) = Castor::Quaternion::GetYaw();
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::GetPitch( /* [out, retval] */ IAngle ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			hr = CAngle::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< Castor::Angle >( *static_cast< CAngle * >( *pVal ) ) = Castor::Quaternion::GetPitch();
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::GetRoll( /* [out, retval] */ IAngle ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			hr = CAngle::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< Castor::Angle >( *static_cast< CAngle * >( *pVal ) ) = Castor::Quaternion::GetRoll();
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::GetMagnitude( /* [out, retval] */ float * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			*pVal = Castor::Quaternion::GetMagnitude();
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::Conjugate()
	{
		Castor::Quaternion::Conjugate();
		return S_OK;
	}

	STDMETHODIMP CQuaternion::Slerp( /* [in] */ IQuaternion * quat, /* [in] */ float percent, /* [in] */ boolean shortest, /* [out, retval] */ IQuaternion ** pQuat )
	{
		HRESULT hr = E_POINTER;

		if ( quat && pQuat )
		{
			hr = CQuaternion::CreateInstance( pQuat );

			if ( hr == S_OK )
			{
				static_cast< Castor::Quaternion >( *static_cast< CQuaternion * >( *pQuat ) ) = Castor::Quaternion::Slerp( *static_cast< CQuaternion * >( quat ), percent, shortest == TRUE );
			}
		}

		return hr;
	}
}

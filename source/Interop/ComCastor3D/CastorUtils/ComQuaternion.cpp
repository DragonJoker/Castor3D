#include "ComCastor3D/CastorUtils/ComQuaternion.hpp"

#include "ComCastor3D/CastorUtils/ComVector3D.hpp"
#include "ComCastor3D/CastorUtils/ComAngle.hpp"

namespace CastorCom
{
	STDMETHODIMP CQuaternion::get_RotationMatrix( IMatrix4x4 ** /* [out, retval] */ pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			hr = CMatrix4x4::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				castor::Matrix4x4f mtx;
				m_internal.toMatrix( mtx );
				static_cast< CMatrix4x4 * >( *pVal )->setInternal( mtx );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::Transform( /* [in] */ IVector3D * val, /* [out, retval] */ IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( val && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				castor::Point3f res;
				m_internal.transform( static_cast< CVector3D * >( val )->getInternal()
					, res );
				static_cast< CVector3D * >( *pVal )->setInternal( res );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::ToAxisAngle( /* [out] */ IVector3D ** pAxis, /* [out] */ IAngle ** pAngle )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pAxis && pAngle )
		{
			hr = CVector3D::CreateInstance( pAxis );
			hr = hr == S_OK ? CAngle::CreateInstance( pAngle ) : hr;

			if ( hr == S_OK )
			{
				castor::Point3f axis;
				castor::Angle angle;
				m_internal.toAxisAngle( axis, angle );
				static_cast< CVector3D * >( *pAxis )->setInternal( axis );
				static_cast< CAngle * >( *pAngle )->setInternal( angle );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::FromAxisAngle( /* [in] */ IVector3D * axis, /* [in] */ IAngle * angle )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( axis && angle )
		{
			castor::Quaternion::fromAxisAngle( static_cast< CVector3D const * >( axis )->getInternal()
				, static_cast< CAngle * >( angle )->getInternal() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::ToAxes( /* [out] */ IVector3D ** pX, /* [out] */ IVector3D ** pY, /* [out] */ IVector3D ** pZ )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pX && pY && pZ )
		{
			hr = CVector3D::CreateInstance( pX );
			hr = hr == S_OK ? CVector3D::CreateInstance( pY ) : hr;
			hr = hr == S_OK ? CVector3D::CreateInstance( pZ ) : hr;

			if ( hr == S_OK )
			{
				castor::Point3f x;
				castor::Point3f y;
				castor::Point3f z;
				m_internal.toAxes( x, y, z );
				static_cast< CVector3D * >( *pX )->setInternal( x );
				static_cast< CVector3D * >( *pY )->setInternal( y );
				static_cast< CVector3D * >( *pZ )->setInternal( z );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::FromAxes( /* [in] */ IVector3D * x, /* [in] */ IVector3D * y, /* [in] */ IVector3D * z )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( x && y && z )
		{
			m_internal.fromAxes( static_cast< CVector3D * >( x )->getInternal()
				, static_cast< CVector3D * >( y )->getInternal()
				, static_cast< CVector3D * >( z )->getInternal() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::GetMagnitude( /* [out, retval] */ float * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal )
		{
			*pVal = float( m_internal.getMagnitude() );
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::Conjugate()noexcept
	{
		m_internal.conjugate();
		return S_OK;
	}

	STDMETHODIMP CQuaternion::Slerp( /* [in] */ IQuaternion * pquat, /* [in] */ float percent, /* [out, retval] */ IQuaternion ** pQuat )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pquat && pQuat )
		{
			hr = CQuaternion::CreateInstance( pQuat );

			if ( hr == S_OK )
			{
				static_cast< CQuaternion * >( *pQuat )->setInternal( m_internal.slerp( static_cast< CQuaternion const * >( pquat )->getInternal(), percent ) );
			}
		}

		return hr;
	}

	STDMETHODIMP CQuaternion::Mix( /* [in] */ IQuaternion * pquat, /* [in] */ float percent, /* [out, retval] */ IQuaternion ** pQuat )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pquat && pQuat )
		{
			hr = CQuaternion::CreateInstance( pQuat );

			if ( hr == S_OK )
			{
				static_cast< CQuaternion * >( *pQuat )->setInternal( m_internal.mix( static_cast< CQuaternion const * >( pquat )->getInternal(), percent ) );
			}
		}

		return hr;
	}
}

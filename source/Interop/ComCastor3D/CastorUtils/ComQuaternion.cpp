#include "ComCastor3D/CastorUtils/ComQuaternion.hpp"

#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	STDMETHODIMP CQuaternion::Transform( /*[in]*/ IVector3D * val, /*[out, retval]*/ IVector3D ** pRet )noexcept
	{
		if ( !val || !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dQuat_transform( &m_internal
			, &static_cast< CVector3D * >( val )->getInternal()
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CQuaternion::ToAxisAngle( /*[out]*/ IVector3D ** pAxis, /*[out]*/ FLOAT * pAngle )noexcept
	{
		if ( !pAxis || !pAngle )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pAxis ) != S_OK )
			return E_FAIL;

		return convert( c3dQuat_getAxisAngle( &m_internal
			, &static_cast< CVector3D * >( *pAxis )->getInternal()
			, pAngle ) );
	}

	STDMETHODIMP CQuaternion::FromAxisAngle( /*[in]*/ IVector3D * axis, /*[in]*/ FLOAT angle )noexcept
	{
		if ( !axis )
			return E_POINTER;

		return convert( c3dQuat_fromAxisAngle( &static_cast< CVector3D * >( axis )->getInternal()
			, angle
			, &m_internal ) );
	}

	STDMETHODIMP CQuaternion::ToAxes( /*[out]*/ IVector3D ** pX, /*[out]*/ IVector3D ** pY, /*[out]*/ IVector3D ** pZ )noexcept
	{
		if ( !pX || !pY || !pZ )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pX ) != S_OK )
			return E_FAIL;
		if ( CVector3D::CreateInstance( pY ) != S_OK )
			return E_FAIL;
		if ( CVector3D::CreateInstance( pZ ) != S_OK )
			return E_FAIL;

		return convert( c3dQuat_getAxes( &m_internal
			, &static_cast< CVector3D * >( *pX )->getInternal()
			, &static_cast< CVector3D * >( *pY )->getInternal()
			, &static_cast< CVector3D * >( *pZ )->getInternal() ) );
	}

	STDMETHODIMP CQuaternion::FromAxes( /*[in]*/ IVector3D * x, /*[in]*/ IVector3D * y, /*[in]*/ IVector3D * z )noexcept
	{
		if ( !x || !y || !z )
			return E_POINTER;

		return convert( c3dQuat_fromAxes( &static_cast< CVector3D * >( x )->getInternal()
			, &static_cast< CVector3D * >( y )->getInternal()
			, &static_cast< CVector3D * >( z )->getInternal()
			, &m_internal ) );
	}

	STDMETHODIMP CQuaternion::GetMagnitude( /*[out, retval]*/ FLOAT * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;

		return convert( c3dQuat_getMagnitude( &m_internal, pRet ) );
	}

	STDMETHODIMP CQuaternion::Conjugate()noexcept
	{
		return convert( c3dQuat_conjugate( &m_internal ) );
	}

	STDMETHODIMP CQuaternion::Slerp( /*[in]*/ IQuaternion * dest, /*[in]*/ float percent, /*[out, retval]*/ IQuaternion ** pRet )noexcept
	{
		if ( !dest || !pRet )
			return E_POINTER;
		if ( CQuaternion::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dQuat_slerp( &m_internal
			, &static_cast< CQuaternion * >( dest )->getInternal()
			, percent
			, &static_cast< CQuaternion * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CQuaternion::Mix( /*[in]*/ IQuaternion * dest, /*[in]*/ float percent, /*[out, retval]*/ IQuaternion ** pRet )noexcept
	{
		if ( !dest || !pRet )
			return E_POINTER;
		if ( CQuaternion::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dQuat_mix( &m_internal
			, &static_cast< CQuaternion * >( dest )->getInternal()
			, percent
			, &static_cast< CQuaternion * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CQuaternion::Mul( /*[in]*/ IQuaternion * rhs, /*[out, retval]*/ IQuaternion ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CQuaternion::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dQuat_mul( &m_internal
			, &static_cast< CQuaternion * >( rhs )->getInternal()
			, &static_cast< CQuaternion * >( *pRet )->getInternal() ) );
	}
}

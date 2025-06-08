#include "ComCastor3D/CastorUtils/ComVector3D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CVector3D::Negate()noexcept
	{
		return convert( c3dVec3_negate( &m_internal ) );
	}

	STDMETHODIMP CVector3D::Normalise()noexcept
	{
		return convert( c3dVec3_normalise( &m_internal ) );
	}

	STDMETHODIMP CVector3D::Dot( IVector3D * rhs, FLOAT * pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;

		return convert( c3dVec3_dot( &m_internal
			, &static_cast< CVector3D * >( rhs )->getInternal()
			, pRet ) );
	}

	STDMETHODIMP CVector3D::Cross( IVector3D * rhs, IVector3D ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_cross( &m_internal
			, &static_cast< CVector3D * >( rhs )->getInternal()
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector3D::Length( FLOAT * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;

		return convert( c3dVec3_length( &m_internal, pRet ) );
	}

	STDMETHODIMP CVector3D::Set( /*[in]*/ FLOAT x, /*[in]*/ FLOAT y, /*[in]*/ FLOAT z )noexcept
	{
		m_internal.x = x;
		m_internal.y = y;
		m_internal.z = z;
		return S_OK;
	}

	STDMETHODIMP CVector3D::CompMul( IVector3D * rhs, IVector3D ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_compMul( &m_internal
			, &static_cast< CVector3D * >( rhs )->getInternal()
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector3D::CompAdd( IVector3D * rhs, IVector3D ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_compAdd( &m_internal
			, &static_cast< CVector3D * >( rhs )->getInternal()
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector3D::CompSub( IVector3D * rhs, IVector3D ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_compSub( &m_internal
			, &static_cast< CVector3D * >( rhs )->getInternal()
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector3D::Mul( float rhs, IVector3D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_mul( &m_internal
			, rhs
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector3D::Div( float rhs, IVector3D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_div( &m_internal
			, rhs
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector3D::Add( float rhs, IVector3D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_add( &m_internal
			, rhs
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector3D::Sub( float rhs, IVector3D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector3D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec3_sub( &m_internal
			, rhs
			, &static_cast< CVector3D * >( *pRet )->getInternal() ) );
	}
}

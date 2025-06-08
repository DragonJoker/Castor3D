#include "ComCastor3D/CastorUtils/ComVector2D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CVector2D::Negate()noexcept
	{
		return convert( c3dVec2_negate( &m_internal ) );
	}

	STDMETHODIMP CVector2D::Normalise()noexcept
	{
		return convert( c3dVec2_normalise( &m_internal ) );
	}

	STDMETHODIMP CVector2D::Dot( IVector2D * rhs, FLOAT * pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;

		return convert( c3dVec2_dot( &m_internal
			, &static_cast< CVector2D * >( rhs )->getInternal()
			, pRet ) );
	}

	STDMETHODIMP CVector2D::Length( FLOAT * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;

		return convert( c3dVec2_length( &m_internal, pRet ) );
	}

	STDMETHODIMP CVector2D::Set( /*[in]*/ FLOAT x, /*[in]*/ FLOAT y )noexcept
	{
		m_internal.x = x;
		m_internal.y = y;
		return S_OK;
	}

	STDMETHODIMP CVector2D::CompMul( IVector2D * rhs, IVector2D ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CVector2D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert(c3dVec2_compMul( &m_internal
			, &static_cast< CVector2D * >( rhs )->getInternal()
			, &static_cast< CVector2D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector2D::CompAdd( IVector2D * rhs, IVector2D ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CVector2D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec2_compAdd( &m_internal
			, &static_cast< CVector2D * >( rhs )->getInternal()
			, &static_cast< CVector2D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector2D::CompSub( IVector2D * rhs, IVector2D ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CVector2D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec2_compSub( &m_internal
			, &static_cast< CVector2D * >( rhs )->getInternal()
			, &static_cast< CVector2D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector2D::Mul( float rhs, IVector2D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector2D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec2_mul( &m_internal
			, rhs
			, &static_cast< CVector2D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector2D::Div( float rhs, IVector2D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector2D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec2_div( &m_internal
			, rhs
			, &static_cast< CVector2D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector2D::Add( float rhs, IVector2D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector2D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec2_add( &m_internal
			, rhs
			, &static_cast< CVector2D * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CVector2D::Sub( float rhs, IVector2D ** pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( CVector2D::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dVec2_sub( &m_internal
			, rhs
			, &static_cast< CVector2D * >( *pRet )->getInternal() ) );
	}
}

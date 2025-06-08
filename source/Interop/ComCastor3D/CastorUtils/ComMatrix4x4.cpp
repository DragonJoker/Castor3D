#include "ComCastor3D/CastorUtils/ComMatrix4x4.hpp"

namespace CastorCom
{
	STDMETHODIMP CMatrix4x4::Transpose()noexcept
	{
		return convert( c3dMat4_transpose( &m_internal ) );
	}

	STDMETHODIMP CMatrix4x4::Invert()noexcept
	{
		return convert( c3dMat4_invert( &m_internal ) );
	}

	STDMETHODIMP CMatrix4x4::SetIdentity()noexcept
	{
		return convert( c3dMat4_setIdentity( &m_internal ) );
	}

	STDMETHODIMP CMatrix4x4::CompMul( /*[in]*/ IMatrix4x4 * rhs, /*[out, retval]*/ IMatrix4x4 ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CMatrix4x4::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dMat4_compMul( &m_internal
			, &static_cast< CMatrix4x4 * >( rhs )->getInternal()
			, &static_cast< CMatrix4x4 * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CMatrix4x4::Mul( /*[in]*/ IMatrix4x4 * rhs, /*[out, retval]*/ IMatrix4x4 ** pRet )noexcept
	{
		if ( !rhs || !pRet )
			return E_POINTER;
		if ( CMatrix4x4::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dMat4_mul( &m_internal
			, &static_cast< CMatrix4x4 * >( rhs )->getInternal()
			, &static_cast< CMatrix4x4 * >( *pRet )->getInternal() ) );
	}
}

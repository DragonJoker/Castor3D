#include "ComCastor3D/CastorUtils/ComVector4D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CVector4D::Negate()noexcept
	{
		castor::point::negate( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector4D::Normalise()noexcept
	{
		castor::point::normalise( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector4D::Dot( IVector4D * pVal, FLOAT * pRet )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( m_internal
				, static_cast< CVector4D * >( pVal )->getInternal() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Length( FLOAT * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( m_internal ) );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Set( /* [in] */ FLOAT x, /* [in] */ FLOAT y, /* [in] */ FLOAT z, /* [in] */ FLOAT w )noexcept
	{
		m_internal->x = x;
		m_internal->y = y;
		m_internal->z = z;
		m_internal->w = w;
		return S_OK;
	}

	STDMETHODIMP CVector4D::CompMul( IVector4D * rhs, IVector4D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector4D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector4D * >( rhs )->getInternal();
				static_cast< CVector4D * >( *pVal )->setInternal( { m_internal->x * rhsInternal->x
					, m_internal->y * rhsInternal->y
					, m_internal->z * rhsInternal->z
					, m_internal->w * rhsInternal->w } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector4D::CompAdd( IVector4D * rhs, IVector4D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector4D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector4D * >( rhs )->getInternal();
				static_cast< CVector4D * >( *pVal )->setInternal( { m_internal->x + rhsInternal->x
					, m_internal->y + rhsInternal->y
					, m_internal->z + rhsInternal->z
					, m_internal->w + rhsInternal->w } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector4D::CompSub( IVector4D * rhs, IVector4D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector4D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector4D * >( rhs )->getInternal();
				static_cast< CVector4D * >( *pVal )->setInternal( { m_internal->x - rhsInternal->x
					, m_internal->y - rhsInternal->y
					, m_internal->z - rhsInternal->z
					, m_internal->w - rhsInternal->w } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Mul( float rhs, IVector4D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector4D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector4D * >( *pVal )->setInternal( { m_internal->x * rhs
					, m_internal->y * rhs
					, m_internal->z * rhs
					, m_internal->w * rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Div( float rhs, IVector4D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector4D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector4D * >( *pVal )->setInternal( { m_internal->x / rhs
					, m_internal->y / rhs
					, m_internal->z / rhs
					, m_internal->w / rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Add( float rhs, IVector4D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector4D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector4D * >( *pVal )->setInternal( { m_internal->x + rhs
					, m_internal->y + rhs
					, m_internal->z + rhs
					, m_internal->w + rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector4D::Sub( float rhs, IVector4D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector4D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector4D * >( *pVal )->setInternal( { m_internal->x - rhs
					, m_internal->y - rhs
					, m_internal->z - rhs
					, m_internal->w - rhs } );
			}
		}

		return hr;
	}
}

#include "ComCastor3D/CastorUtils/ComVector3D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CVector3D::Negate()noexcept
	{
		castor::point::negate( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector3D::Normalise()noexcept
	{
		castor::point::normalise( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector3D::Dot( IVector3D * pVal, FLOAT * pRet )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( m_internal
				, static_cast< CVector3D * >( pVal )->getInternal() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Cross( IVector3D * pVal, IVector3D ** pRet )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			hr = CVector3D::CreateInstance( pRet );

			if ( hr == S_OK )
			{
				static_cast< CVector3D * >( *pRet )->setInternal( castor::point::cross( m_internal
					, static_cast< CVector3D * >( pVal )->getInternal() ) );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Length( FLOAT * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( m_internal ) );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Set( /* [in] */ FLOAT x, /* [in] */ FLOAT y, /* [in] */ FLOAT z )noexcept
	{
		m_internal->x = x;
		m_internal->y = y;
		m_internal->z = z;
		return S_OK;
	}

	STDMETHODIMP CVector3D::CompMul( IVector3D * rhs, IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector3D * >( rhs )->getInternal();
				static_cast< CVector3D * >( *pVal )->setInternal( { m_internal->x * rhsInternal->x
					, m_internal->y * rhsInternal->y
					, m_internal->z * rhsInternal->z } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::CompAdd( IVector3D * rhs, IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector3D * >( rhs )->getInternal();
				static_cast< CVector3D * >( *pVal )->setInternal( { m_internal->x + rhsInternal->x
					, m_internal->y + rhsInternal->y
					, m_internal->z + rhsInternal->z } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::CompSub( IVector3D * rhs, IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector3D * >( rhs )->getInternal();
				static_cast< CVector3D * >( *pVal )->setInternal( { m_internal->x - rhsInternal->x
					, m_internal->y - rhsInternal->y
					, m_internal->z - rhsInternal->z } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Mul( float rhs, IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector3D * >( *pVal )->setInternal( { m_internal->x * rhs
					, m_internal->y * rhs
					, m_internal->z * rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Div( float rhs, IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector3D * >( *pVal )->setInternal( { m_internal->x / rhs
					, m_internal->y / rhs
					, m_internal->z / rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Add( float rhs, IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector3D * >( *pVal )->setInternal( { m_internal->x + rhs
					, m_internal->y + rhs
					, m_internal->z + rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector3D::Sub( float rhs, IVector3D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector3D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector3D * >( *pVal )->setInternal( { m_internal->x - rhs
					, m_internal->y - rhs
					, m_internal->z - rhs } );
			}
		}

		return hr;
	}
}

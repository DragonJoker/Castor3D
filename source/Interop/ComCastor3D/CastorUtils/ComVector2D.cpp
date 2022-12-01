#include "ComCastor3D/CastorUtils/ComVector2D.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CVector2D::Negate()noexcept
	{
		castor::point::negate( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector2D::Normalise()noexcept
	{
		castor::point::normalise( m_internal );
		return S_OK;
	}

	STDMETHODIMP CVector2D::Dot( IVector2D * pVal, FLOAT * pRet )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pRet )
		{
			*pRet = castor::point::dot( m_internal
				, static_cast< CVector2D * >( pVal )->getInternal() );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Length( FLOAT * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( pVal && pVal )
		{
			*pVal = FLOAT( castor::point::length( m_internal ) );
			hr = S_OK;
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Set( /* [in] */ FLOAT x, /* [in] */ FLOAT y )noexcept
	{
		m_internal->x = x;
		m_internal->y = y;
		return S_OK;
	}

	STDMETHODIMP CVector2D::CompMul( IVector2D * rhs, IVector2D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector2D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector2D * >( rhs )->getInternal();
				static_cast< CVector2D * >( *pVal )->setInternal( { m_internal->x * rhsInternal->x
					, m_internal->y * rhsInternal->y } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector2D::CompAdd( IVector2D * rhs, IVector2D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector2D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector2D * >( rhs )->getInternal();
				static_cast< CVector2D * >( *pVal )->setInternal( { m_internal->x + rhsInternal->x
					, m_internal->y + rhsInternal->y } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector2D::CompSub( IVector2D * rhs, IVector2D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector2D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				auto rhsInternal = static_cast< CVector2D * >( rhs )->getInternal();
				static_cast< CVector2D * >( *pVal )->setInternal( { m_internal->x - rhsInternal->x
					, m_internal->y - rhsInternal->y } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Mul( float rhs, IVector2D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector2D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector2D * >( *pVal )->setInternal( { m_internal->x * rhs
					, m_internal->y * rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Div( float rhs, IVector2D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector2D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector2D * >( *pVal )->setInternal( { m_internal->x / rhs
					, m_internal->y / rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Add( float rhs, IVector2D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector2D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector2D * >( *pVal )->setInternal( { m_internal->x + rhs
					, m_internal->y + rhs } );
			}
		}

		return hr;
	}

	STDMETHODIMP CVector2D::Sub( float rhs, IVector2D ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( rhs && pVal )
		{
			hr = CVector2D::CreateInstance( pVal );

			if ( hr == S_OK )
			{
				static_cast< CVector2D * >( *pVal )->setInternal( { m_internal->x - rhs
					, m_internal->y - rhs } );
			}
		}

		return hr;
	}
}

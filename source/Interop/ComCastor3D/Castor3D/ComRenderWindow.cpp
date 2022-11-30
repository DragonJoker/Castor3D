#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"

#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"

#include <Castor3D/Event/UserInput/UserInputListener.hpp>

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The render window must be initialised" );

	STDMETHODIMP CRenderWindow::Initialise( /* [in] */ IRenderTarget * target )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			try
			{
				m_internal->initialise( *static_cast< CRenderTarget * >( target )->getInternal() );
				hr = S_OK;
			}
			catch ( castor::Exception & p_exc )
			{
				castor::Logger::logError( p_exc.getFullDescription() );
			}
			catch ( std::exception & p_exc )
			{
				castor::Logger::logError( p_exc.what() );
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "Initialise" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Cleanup()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "Cleanup" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Resize( /* [in] */ ISize * size )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->resize( static_cast< CSize * >( size )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "Resize" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMove( /* [in] */ IPosition * pos )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );
#if HAS_CASTORGUI

			try
			{
				auto l_inputListener = m_internal->getEngine()->getUserInputListener();

				if ( l_inputListener )
				{
					l_inputListener->fireMouseMove( castor::Position( int32_t( m_oldX ), int32_t( m_oldY ) ) );
				}
			}
			catch ( castor::Exception & )
			{
			}

#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseMove" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonDown( /* [in] */ IPosition * pos )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );
#if HAS_CASTORGUI

			try
			{
				auto l_inputListener = m_internal->getEngine()->getUserInputListener();

				if ( l_inputListener )
				{
					l_inputListener->fireMouseButtonPushed( castor3d::eMOUSE_BUTTON_LEFT );
				}
			}
			catch ( castor::Exception & )
			{
			}

#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseLButtondown" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonUp( /* [in] */ IPosition * pos )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );
#if HAS_CASTORGUI

			try
			{
				auto l_inputListener = m_internal->getEngine()->getUserInputListener();

				if ( l_inputListener )
				{
					l_inputListener->fireMouseButtonReleased( castor3d::eMOUSE_BUTTON_LEFT );
				}
			}
			catch ( castor::Exception & )
			{
			}

#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseLButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonDown( /* [in] */ IPosition * pos )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );
#if HAS_CASTORGUI

			try
			{
				auto l_inputListener = m_internal->getEngine()->getUserInputListener();

				if ( l_inputListener )
				{
					l_inputListener->fireMouseButtonPushed( castor3d::eMOUSE_BUTTON_MIDDLE );
				}
			}
			catch ( castor::Exception & )
			{
			}

#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseMButtondown" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonUp( /* [in] */ IPosition * pos )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );
#if HAS_CASTORGUI

			try
			{
				auto l_inputListener = m_internal->getEngine()->getUserInputListener();

				if ( l_inputListener )
				{
					l_inputListener->fireMouseButtonReleased( castor3d::eMOUSE_BUTTON_MIDDLE );
				}
			}
			catch ( castor::Exception & )
			{
			}

#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseMButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonDown( /* [in] */ IPosition * pos )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );
#if HAS_CASTORGUI

			try
			{
				auto l_inputListener = m_internal->getEngine()->getUserInputListener();

				if ( l_inputListener )
				{
					l_inputListener->fireMouseButtonPushed( castor3d::eMOUSE_BUTTON_RIGHT );
				}
			}
			catch ( castor::Exception & )
			{
			}

#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseRButtondown" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonUp( /* [in] */ IPosition * pos )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );
#if HAS_CASTORGUI

			try
			{
				auto l_inputListener = m_internal->getEngine()->getUserInputListener();

				if ( l_inputListener )
				{
					l_inputListener->fireMouseButtonReleased( castor3d::eMOUSE_BUTTON_RIGHT );
				}
			}
			catch ( castor::Exception & )
			{
			}

#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseRButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}
}

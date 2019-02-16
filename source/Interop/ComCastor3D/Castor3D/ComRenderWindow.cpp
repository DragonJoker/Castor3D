#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"

#include <Castor3D/Event/UserInput/UserInputListener.hpp>

#include <Ashes/Core/PlatformWindowHandle.hpp>

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The render window must be initialised" );

	CRenderWindow::CRenderWindow()
	{
	}

	CRenderWindow::~CRenderWindow()
	{
	}

	STDMETHODIMP CRenderWindow::Initialise( /* [in] */ ISize * size, /* [in] */ LPVOID val, /* [out, retval] */ VARIANT_BOOL * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				try
				{
					*pVal = m_internal->initialise( *static_cast< CSize * >( size )
						, ashes::WindowHandle{ std::make_unique< ashes::IMswWindowHandle >( ::GetModuleHandle( nullptr ), HWND( val ) ) } )
							? VARIANT_TRUE
							: VARIANT_FALSE;
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
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "Initialise" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "Cleanup" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Resize( /* [in] */ ISize * size )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->resize( *static_cast< CSize * >( size ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "Resize" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMove( /* [in] */ IPosition * pos )
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseMove" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonDown( /* [in] */ IPosition * pos )
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseLButtondown" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonUp( /* [in] */ IPosition * pos )
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseLButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonDown( /* [in] */ IPosition * pos )
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseMButtondown" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonUp( /* [in] */ IPosition * pos )
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseMButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonDown( /* [in] */ IPosition * pos )
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseRButtondown" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonUp( /* [in] */ IPosition * pos )
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseRButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}
}

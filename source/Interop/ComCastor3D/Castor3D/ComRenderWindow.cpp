#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"

#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"

#include <Castor3D/Event/UserInput/UserInputListener.hpp>

namespace CastorCom
{
	namespace window
	{
		static const tstring ERROR_UNINITIALISED = _T( "The render window must be initialised" );
	}

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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "Initialise" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "Cleanup" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
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
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "Resize" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMove( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );

			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireMouseMove( castor::Position( int32_t( m_oldX ), int32_t( m_oldY ) ) );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseMove" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonDown( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );

			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireMouseButtonPushed( castor3d::MouseButton::eLeft );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseLButtondown" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonUp( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );

			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireMouseButtonReleased( castor3d::MouseButton::eLeft );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseLButtonUp" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonDown( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );

			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireMouseButtonPushed( castor3d::MouseButton::eMiddle );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseMButtondown" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonUp( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );

			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireMouseButtonReleased( castor3d::MouseButton::eMiddle );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseMButtonUp" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonDown( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );

			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireMouseButtonPushed( castor3d::MouseButton::eRight );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseRButtondown" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonUp( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			pos->get_X( &m_oldX );
			pos->get_Y( &m_oldY );

			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireMouseButtonReleased( castor3d::MouseButton::eRight );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnMouseRButtonUp" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnKeyboardKeyDown( /* [in] */ eKEYBOARD_KEY key, /* [in] */ boolean isCtrlDown, /* [in] */ boolean isAltDown, /* [in] */ boolean isShiftDown, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireKeydown( castor3d::KeyboardKey( key )
						, isCtrlDown != FALSE
						, isAltDown != FALSE
						, isShiftDown != FALSE );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnKeyboardKeyDown" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnKeyboardKeyUp( /* [in] */ eKEYBOARD_KEY key, /* [in] */ boolean isCtrlDown, /* [in] */ boolean isAltDown, /* [in] */ boolean isShiftDown, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireKeyUp( castor3d::KeyboardKey( key )
						, isCtrlDown != FALSE
						, isAltDown != FALSE
						, isShiftDown != FALSE );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnKeyboardKeyUp" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::OnKeyboardChar( /* [in] */ eKEYBOARD_KEY key, /* [in] */ BSTR c, /* [out, retval] */ boolean * pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			try
			{
				if ( auto inputListener = m_internal->getEngine()->getUserInputListener() )
				{
					*pVal = inputListener->fireChar( castor3d::KeyboardKey( key )
						, fromBstr( c ) );
				}
			}
			catch ( castor::Exception & )
			{
			}

			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IRenderWindow, _T( "OnKeyboardKeyUp" ), window::ERROR_UNINITIALISED.c_str(), 0, nullptr );
		}

		return hr;
	}
}

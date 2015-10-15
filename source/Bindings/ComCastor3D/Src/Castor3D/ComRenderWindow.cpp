#include "ComRenderWindow.hpp"

#include <PlatformWindowHandle.hpp>

#if HAS_CASTORGUI
#	include <ControlsManager.hpp>
#	include <ListenerManager.hpp>
#endif

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The render window must be initialised" );

	CRenderWindow::CRenderWindow()
	{
	}

	CRenderWindow::~CRenderWindow()
	{
	}

	STDMETHODIMP CRenderWindow::Initialise( /* [in] */ LPVOID val, /* [out, retval] */ VARIANT_BOOL * pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				try
				{
					*pVal = m_internal->Initialise( Castor3D::WindowHandle( std::make_shared< Castor3D::IMswWindowHandle >( HWND( val ) ) ) ) ? VARIANT_TRUE : VARIANT_FALSE;
					hr = S_OK;
				}
				catch ( Castor::Exception & p_exc )
				{
					Castor::Logger::LogError( p_exc.GetFullDescription() );
				}
				catch ( std::exception & p_exc )
				{
					Castor::Logger::LogError( p_exc.what() );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "Initialise" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "Cleanup" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CRenderWindow::Resize( /* [in] */ ISize * size )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Resize( *static_cast< CSize * >( size ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "Resize" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
				CastorGui::ControlsManager & l_controlsManager = *std::static_pointer_cast< CastorGui::ControlsManager >( m_internal->GetOwner()->GetListenerManager().Find( CastorGui::PLUGIN_NAME ) );
				l_controlsManager.FireMouseMove( Castor::Position( m_oldX, m_oldY ) );
			}
			catch ( Castor::Exception & )
			{
			}
#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseMove" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
				CastorGui::ControlsManager & l_controlsManager = *std::static_pointer_cast< CastorGui::ControlsManager >( m_internal->GetOwner()->GetListenerManager().Find( CastorGui::PLUGIN_NAME ) );
				l_controlsManager.FireMouseButtonPushed( CastorGui::eMOUSE_BUTTON_LEFT );
			}
			catch ( Castor::Exception & )
			{
			}
#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseLButtonDown" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
				CastorGui::ControlsManager & l_controlsManager = *std::static_pointer_cast< CastorGui::ControlsManager >( m_internal->GetOwner()->GetListenerManager().Find( CastorGui::PLUGIN_NAME ) );
				l_controlsManager.FireMouseButtonReleased( CastorGui::eMOUSE_BUTTON_LEFT );
			}
			catch ( Castor::Exception & )
			{
			}
#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseLButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
				CastorGui::ControlsManager & l_controlsManager = *std::static_pointer_cast< CastorGui::ControlsManager >( m_internal->GetOwner()->GetListenerManager().Find( CastorGui::PLUGIN_NAME ) );
				l_controlsManager.FireMouseButtonPushed( CastorGui::eMOUSE_BUTTON_MIDDLE );
			}
			catch ( Castor::Exception & )
			{
			}
#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseMButtonDown" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
				CastorGui::ControlsManager & l_controlsManager = *std::static_pointer_cast< CastorGui::ControlsManager >( m_internal->GetOwner()->GetListenerManager().Find( CastorGui::PLUGIN_NAME ) );
				l_controlsManager.FireMouseButtonReleased( CastorGui::eMOUSE_BUTTON_MIDDLE );
			}
			catch ( Castor::Exception & )
			{
			}
#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseMButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
				CastorGui::ControlsManager & l_controlsManager = *std::static_pointer_cast< CastorGui::ControlsManager >( m_internal->GetOwner()->GetListenerManager().Find( CastorGui::PLUGIN_NAME ) );
				l_controlsManager.FireMouseButtonPushed( CastorGui::eMOUSE_BUTTON_RIGHT );
			}
			catch ( Castor::Exception & )
			{
			}
#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseRButtonDown" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
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
				CastorGui::ControlsManager & l_controlsManager = *std::static_pointer_cast< CastorGui::ControlsManager >( m_internal->GetOwner()->GetListenerManager().Find( CastorGui::PLUGIN_NAME ) );
				l_controlsManager.FireMouseButtonReleased( CastorGui::eMOUSE_BUTTON_RIGHT );
			}
			catch ( Castor::Exception & )
			{
			}
#endif
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IRenderWindow, cuT( "OnMouseRButtonUp" ), ERROR_UNINITIALISED.c_str(), 0, NULL );
		}

		return hr;
	}
}

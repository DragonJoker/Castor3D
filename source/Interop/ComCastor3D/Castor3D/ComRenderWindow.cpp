#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"

#include "ComCastor3D/Castor3D/ComEngine.hpp"
#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <bit>

namespace CastorCom
{
	STDMETHODIMP CRenderWindow::Initialise( /*[in]*/ IRenderTarget * target )noexcept
	{
		if ( !target )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "Initialise" ) );

		return convert( c3dRenderWindow_initialise( m_internal
			, static_cast< CRenderTarget * >( target )->getInternal() ) );
	}

	STDMETHODIMP CRenderWindow::Cleanup()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Initialise" ) );

		return convert( c3dRenderWindow_cleanup( m_internal ) );
	}

	STDMETHODIMP CRenderWindow::Resize( /*[in]*/ ISize * size )noexcept
	{
		if ( !size )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "Initialise" ) );

		return convert( c3dRenderWindow_resize( m_internal
			, &static_cast< CSize * >( size )->getInternal() ) );
	}

	STDMETHODIMP CRenderWindow::OnMouseMove( /*[in]*/ IPosition * pos, /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet || !pos )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnMouseMove" ) );

		return convert( c3dRenderWindow_onMouseMove( m_internal
			, &static_cast< CPosition * >( pos )->getInternal()
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonDown( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnMouseLButtonDown" ) );

		return convert( c3dRenderWindow_onMouseLButtonDown( m_internal
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnMouseLButtonUp( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnMouseLButtonUp" ) );

		return convert( c3dRenderWindow_onMouseLButtonUp( m_internal
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonDown( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnMouseMButtonDown" ) );

		return convert( c3dRenderWindow_onMouseMButtonDown( m_internal
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnMouseMButtonUp( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnMouseMButtonUp" ) );

		return convert( c3dRenderWindow_onMouseMButtonUp( m_internal
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonDown( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnMouseRButtonDown" ) );

		return convert( c3dRenderWindow_onMouseRButtonDown( m_internal
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnMouseRButtonUp( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnMouseRButtonUp" ) );

		return convert( c3dRenderWindow_onMouseRButtonUp( m_internal
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnKeyboardKeyDown( /*[in]*/ eKEYBOARD_KEY key, /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnKeyboardKeyDown" ) );

		return convert( c3dRenderWindow_onKeyboardKeyDown( m_internal
			, details::parameterCast< C3D_KEYBOARD_KEY >( key )
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnKeyboardKeyUp( /*[in]*/ eKEYBOARD_KEY key, /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnKeyboardKeyUp" ) );

		return convert( c3dRenderWindow_onKeyboardKeyUp( m_internal
			, details::parameterCast< C3D_KEYBOARD_KEY >( key )
			, details::parameterCast< bool >( isCtrlDown )
			, details::parameterCast< bool >( isAltDown )
			, details::parameterCast< bool >( isShiftDown )
			, std::bit_cast< bool * >( pRet ) ) );
	}

	STDMETHODIMP CRenderWindow::OnKeyboardChar( /*[in]*/ eKEYBOARD_KEY key, /*[in]*/ BSTR c, /*[out, retval]*/ boolean * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "OnKeyboardChar" ) );

		return convert( c3dRenderWindow_onKeyboardChar( m_internal
			, details::parameterCast< C3D_KEYBOARD_KEY >( key )
			, bstrToString( c ).c_str()
			, std::bit_cast< bool * >( pRet ) ) );
	}
}

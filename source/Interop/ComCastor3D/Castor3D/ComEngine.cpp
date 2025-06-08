#include "ComCastor3D/Castor3D/ComEngine.hpp"

#include "ComCastor3D/Castor3D/ComOverlay.hpp"
#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"
#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"
#include "ComCastor3D/Castor3D/ComSampler.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/CastorUtils/ComFont.hpp"
#include "ComCastor3D/CastorUtils/ComImage.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <CCastor3D/Castor3DPlatformTypes.h>

#undef max
#undef min
#undef abs

#define CASTOR3D_THREADED false

#if defined( NDEBUG )
static const int CASTOR_WANTED_FPS	= 120;
#else
static const int CASTOR_WANTED_FPS	= 30;
#endif

namespace CastorCom
{
	static const tstring ERROR_RENDERER_NOT_LOADED = _T( "The renderer couldn't be loaded" );

	STDMETHODIMP CEngine::get_PluginsDirectory( /*[out, retval]*/ BSTR * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;

		std::vector< C3DChar > result( 1025U, '\0' );
		auto hr = convert( c3d_getPluginsDirectory( result.data(), result.size() - 1U ) );
		if ( hr == S_OK)
			*pRet = stringToBstr( result.data() );
		return hr;
	}

	STDMETHODIMP CEngine::get_EngineDirectory( /*[out, retval]*/ BSTR * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;

		std::vector< C3DChar > result( 1025U, '\0' );
		auto hr = convert( c3d_getEngineDirectory( result.data(), result.size() - 1U ) );
		if ( hr == S_OK )
			*pRet = stringToBstr( result.data() );
		return hr;
	}

	STDMETHODIMP CEngine::get_DataDirectory( /*[out, retval]*/ BSTR * pRet )noexcept
	{
		if ( !pRet )
			return E_POINTER;

		std::vector< C3DChar > result( 1025U, '\0' );
		auto hr = convert( c3d_getDataDirectory( result.data(), result.size() - 1U ) );
		if ( hr == S_OK )
			*pRet = stringToBstr( result.data() );
		return hr;
	}

	STDMETHODIMP CEngine::Create( BSTR appName, boolean enableValidation )noexcept
	{
		if ( !appName )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dEngine_create( bstrToString( appName ).c_str()
			, details::parameterCast< bool >( enableValidation )
			, &m_internal ) );
	}

	STDMETHODIMP CEngine::Initialise( /*[in]*/ int fps, /*[in]*/ boolean threaded )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Initialise" ) );

		return convert( c3dEngine_initialise( m_internal
			, fps
			, details::parameterCast< bool >( threaded ) ) );
	}

	STDMETHODIMP CEngine::Cleanup()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "Cleanup" ) );

		return convert( c3dEngine_cleanup( m_internal ) );
	}

	STDMETHODIMP CEngine::ClearScenes()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "ClearScenes" ) );

		return convert( c3dEngine_clearScenes( m_internal ) );
	}

	STDMETHODIMP CEngine::LoadRenderer( /*[in]*/ BSTR type )noexcept
	{
		if ( !type )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "LoadRenderer" ) );

		return convert( c3dEngine_loadRenderer( m_internal, bstrToString( type ).c_str() ) );
	}

	STDMETHODIMP CEngine::LoadPlugin( /*[in]*/ BSTR path )noexcept
	{
		if ( !path )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "LoadPlugin" ) );

		return convert( c3dEngine_loadPlugin( m_internal, bstrToString( path ).c_str() ) );
	}

	STDMETHODIMP CEngine::LoadScene( /*[in]*/ BSTR path, /*[out, retval]*/ IRenderTarget ** pTarget )noexcept
	{
		if ( !path || !pTarget )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "LoadScene" ) );
		if ( CRenderTarget::CreateInstance( pTarget ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_loadScene( m_internal
			, bstrToString( path ).c_str()
			, &static_cast< CRenderTarget * >( *pTarget )->getInternal() ) );
	}

	STDMETHODIMP CEngine::LoadImageFromFile( /*[in]*/ BSTR name, /*[in]*/ BSTR path, /*[out, retval]*/ IImage ** pRet )noexcept
	{
		if ( !name || !path || !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "LoadImageFromFile" ) );
		if ( CImage::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_loadImageFromFile( m_internal
			, bstrToString( name ).c_str()
			, bstrToString( path ).c_str()
			, &static_cast< CImage * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::LoadImageFromFormat( /*[in]*/ BSTR name, /*[in]*/ ePIXEL_FORMAT fmt, /*[in]*/ ISize * size, /*[out, retval]*/ IImage ** pRet )noexcept
	{
		if ( !name || !size || !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "LoadImageFromFormat" ) );
		if ( CImage::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_loadImageFromFormat( m_internal
			, bstrToString( name ).c_str()
			, details::parameterCast< C3D_PIXEL_FORMAT >( fmt )
			, &static_cast< CSize * >( size )->getInternal()
			, &static_cast< CImage * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::LoadFontFromFile( /*[in]*/ BSTR name, /*[in]*/ BSTR path, /*[in]*/ UINT height, /*[out, retval]*/ ICastorFont ** pRet )noexcept
	{
		if ( !name || !path || !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "LoadFontFromFile" ) );
		if ( CFont::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_loadFontFromFile( m_internal
			, bstrToString( name ).c_str()
			, bstrToString( path ).c_str()
			, height
			, &static_cast< CFont * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::AddScene( /*[in]*/ IScene * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddScene" ) );

		return convert( c3dEngine_addScene( m_internal
			, static_cast< CScene * >( val )->getInternal() ) );
	}

	STDMETHODIMP CEngine::AddOverlay( /*[in]*/ IOverlay * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddOverlay" ) );

		return convert( c3dEngine_addOverlay( m_internal
			, static_cast< COverlay * >( val )->getInternal() ) );
	}

	STDMETHODIMP CEngine::AddSampler( /*[in]*/ ISampler * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddSampler" ) );

		return convert( c3dEngine_addSampler( m_internal
			, static_cast< CSampler * >( val )->getInternal() ) );
	}

	STDMETHODIMP CEngine::RemoveScene( /*[in]*/ IScene * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveScene" ) );

		return convert( c3dEngine_removeScene( m_internal
			, static_cast< CScene * >( val )->getInternal() ) );
	}

	STDMETHODIMP CEngine::RemoveOverlay( /*[in]*/ IOverlay * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveOverlay" ) );

		return convert( c3dEngine_removeOverlay( m_internal
			, static_cast< COverlay * >( val )->getInternal() ) );
	}

	STDMETHODIMP CEngine::RemoveSampler( /*[in]*/ ISampler * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemoveSampler" ) );

		return convert( c3dEngine_removeSampler( m_internal
			, static_cast< CSampler * >( val )->getInternal() ) );
	}

	STDMETHODIMP CEngine::GetScene( /*[in]*/ BSTR name, /*[out, retval]*/ IScene ** pRet )noexcept
	{
		if ( !name || !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetScene" ) );
		if ( CScene::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_getScene( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CScene * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::GetOverlay( /*[in]*/ BSTR name, /*[out, retval]*/ IOverlay ** pRet )noexcept
	{
		if ( !name || !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetOverlay" ) );
		if ( COverlay::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_getOverlay( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< COverlay * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::GetSampler( /*[in]*/ BSTR name, /*[out, retval]*/ ISampler ** pRet )noexcept
	{
		if ( !name || !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetSampler" ) );
		if ( CSampler::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_getSampler( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CSampler * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::RenderOneFrame()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "RenderOneFrame" ) );

		return convert( c3dEngine_renderOneFrame( m_internal ) );
	}

	STDMETHODIMP CEngine::StartRendering()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "StartRendering" ) );

		return convert( c3dEngine_startRendering( m_internal ) );
	}

	STDMETHODIMP CEngine::PauseRendering()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "PauseRendering" ) );

		return convert( c3dEngine_pauseRendering( m_internal ) );
	}

	STDMETHODIMP CEngine::ResumeRendering()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "ResumeRendering" ) );

		return convert( c3dEngine_resumeRendering( m_internal ) );
	}

	STDMETHODIMP CEngine::EndRendering()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "EndRendering" ) );

		return convert( c3dEngine_endRendering( m_internal ) );
	}

	STDMETHODIMP CEngine::RegisterGuiCallbacks( /*[in]*/ IGuiCallbacks * callbacks )noexcept
	{
		if ( !callbacks )
			return E_POINTER;
		if ( m_registered.userContext )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "EndRendering" ) );

		m_callbacks = callbacks;
		m_registered.userContext = this;
		m_registered.onGetClipBoardText = onGetClipBoardText;
		m_registered.onSetClipBoardText = onSetClipBoardText;
		m_registered.onCursorChange = onCursorChange;
		return convert( c3dEngine_registerGuiCallbacks( m_internal, &m_registered ) );
	}

	STDMETHODIMP CEngine::UnregisterGuiCallbacks()noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "EndRendering" ) );

		m_callbacks = nullptr;
		return convert( c3dEngine_unregisterGuiCallbacks( m_internal ) );
	}

	STDMETHODIMP CEngine::CreateOverlay( /*[in]*/ eOVERLAY_TYPE type, /*[in]*/ BSTR name, /*[in]*/ IOverlay * parent, /*[out, retval]*/ IOverlay ** pRet )noexcept
	{
		if ( !pRet || !name )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateOverlay" ) );
		if ( COverlay::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_createOverlay( m_internal
			, details::parameterCast< C3D_OVERLAY_TYPE >( type )
			, bstrToString( name ).c_str()
			, parent ? static_cast< COverlay * >( parent )->getInternal() : nullptr
			, &static_cast< COverlay * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::CreateRenderWindow( /*[in]*/ BSTR name, /*[in]*/ ISize * size, /*[in]*/ LPVOID hWnd, /*[out, retval]*/ IRenderWindow ** pRet )noexcept
	{
		if ( !pRet || !name || !size )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateRenderWindow" ) );
		if ( CRenderWindow::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_createRenderWindow( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CSize * >( size )->getInternal()
			, C3DWindowHandle{ hWnd }
			, &static_cast< CRenderWindow * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::CreateSampler( /*[in]*/ BSTR name, /*[out, retval]*/ ISampler ** pRet )noexcept
	{
		if ( !pRet || !name )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateSampler" ) );
		if ( CSampler::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_createSampler( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CSampler * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::CreateScene( /*[in]*/ BSTR name, /*[out, retval]*/ IScene ** pRet )noexcept
	{
		if ( !pRet || !name )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateScene" ) );
		if ( CScene::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_createScene( m_internal
			, bstrToString( name ).c_str()
			, &static_cast< CScene * >( *pRet )->getInternal() ) );
	}

	STDMETHODIMP CEngine::CreateSkybox( /*[in]*/ IScene * scene, /*[out, retval]*/ ISkybox ** pRet )noexcept
	{
		if ( !scene || !pRet )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "CreateSkybox" ) );
		if ( CSkybox::CreateInstance( pRet ) != S_OK )
			return E_FAIL;

		return convert( c3dEngine_createSkybox( m_internal
			, static_cast< CScene * >( scene )->getInternal()
			, &static_cast< CSkybox * >( *pRet )->getInternal() ) );
	}

	void CEngine::onGetClipBoardText( C3DString * text )noexcept
	{
		if ( m_callbacks && text && *text )
		{
			BSTR res;
			if ( m_callbacks->OnGetClipBoardText( &res ) == S_OK )
				*text = bstrToC3DString( res );
		}
	}

	void CEngine::onSetClipBoardText( C3DString text )noexcept
	{
		if ( m_callbacks && text && *text )
			m_callbacks->OnSetClipBoardText( stringToBstr( text ) );
	}

	void CEngine::onCursorChange( C3D_MOUSE_CURSOR cursor )noexcept
	{
		if ( m_callbacks )
			m_callbacks->OnCursorChange( eMOUSE_CURSOR( cursor ) );
	}
}

#include "ComCastor3D/Castor3D/ComEngine.hpp"
#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComSampler.hpp"
#include "ComCastor3D/Castor3D/ComRenderWindow.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"

#undef max
#undef min
#undef abs

#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <ashespp/Core/PlatformWindowHandle.hpp>

#define CASTOR3D_THREADED false

#if defined( NDEBUG )
static const int CASTOR_WANTED_FPS	= 120;
#else
static const int CASTOR_WANTED_FPS	= 30;
#endif

namespace CastorCom
{
	namespace
	{
		castor3d::RenderWindowDesc doLoadSceneFile( castor3d::Engine & p_engine, castor::Path const & p_fileName )
		{
			castor3d::RenderWindowDesc l_return;

			if ( castor::File::fileExists( p_fileName ) )
			{
				castor::Logger::logInfo( cuT( "Loading scene file : " ) + p_fileName );

				if ( p_fileName.getExtension() == cuT( "cscn" ) || p_fileName.getExtension() == cuT( "zip" ) )
				{
					try
					{
						castor3d::SceneFileParser l_parser( p_engine );

						if ( l_parser.parseFile( p_fileName ) )
						{
							l_return = l_parser.getRenderWindow();
						}
						else
						{
							castor::Logger::logWarning( cuT( "Can't read scene file" ) );
						}
					}
					catch ( std::exception & exc )
					{
						castor::Logger::logError( cuT( "Failed to parse the scene file, with following error:" ) + castor::string::stringCast< castor::xchar >( exc.what() ) );
					}
				}
			}
			else
			{
				castor::Logger::logError( cuT( "Scene file doesn't exist: " ) + p_fileName );
			}

			return l_return;
		}
	}

	static const tstring ERROR_UNINITIALISED_ENGINE = _T( "The IEngine must be initialised" );
	static const tstring ERROR_INITIALISED_ENGINE = _T( "The IEngine has already been initialised" );
	static const tstring ERROR_RENDERER_NOT_LOADED = _T( "The renderer couldn't be loaded" );

	STDMETHODIMP CEngine::Create( BSTR appName, boolean enableValidation )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( !m_internal )
		{
			m_internal = new castor3d::Engine{ fromBstr( appName )
				, castor3d::Version{ ComCastor3D_VERSION_MAJOR, ComCastor3D_VERSION_MINOR, ComCastor3D_VERSION_BUILD }
				, enableValidation == TRUE };
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "create" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Destroy()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			delete m_internal;
			m_internal = nullptr;
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "Destroy" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Initialise( /* [in] */ int fps, /* [in] */ boolean threaded )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialise( fps, threaded == TRUE );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "Initialise" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Cleanup()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "Cleanup" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateScene( /* [in] */ BSTR name, /* [out, retval] */ IScene ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CScene::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CScene * >( *pVal )->setInternal( m_internal->createScene( fromBstr( name )
						, *m_internal ).get() );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "CreateScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::ClearScenes()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getSceneCache().clear();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "ClearScenes" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadRenderer( /* [in] */ BSTR type )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( m_internal->loadRenderer( fromBstr( type ) ) )
			{
				hr = S_OK;
			}
			else
			{
				hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "loadRenderer" ), ERROR_RENDERER_NOT_LOADED.c_str(), 0, nullptr );
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "loadRenderer" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RenderOneFrame()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getRenderLoop().renderSyncFrame();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "RenderOneFrame" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::StartRendering()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getRenderLoop().beginRendering();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "StartRendering" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::PauseRendering()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getRenderLoop().pause();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "PauseRendering" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::EndRendering()noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getRenderLoop().endRendering();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "EndRendering" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadPlugin( /* [in] */ BSTR path )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getPluginCache().loadPlugin( castor::Path{ fromBstr( path ) } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "loadPlugin" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateOverlay( /* [in] */ eOVERLAY_TYPE type, /* [in] */ BSTR name, /* [in] */ IOverlay * parent, /* [in] */ IScene * scene, /* [out, retval] */ IOverlay ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "CreateOverlay" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateRenderWindow( /* [in] */ BSTR name, /* [in] */ ISize * size, /* [in] */ LPVOID hWnd, /* [out, retval] */ IRenderWindow ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CRenderWindow::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CRenderWindow * >( *pVal )->setInternal( std::make_shared< castor3d::RenderWindow >( fromBstr( name )
						, *m_internal
						, static_cast< CSize * >( size )->getInternal()
						, ashes::WindowHandle{ std::make_unique< ashes::IMswWindowHandle >( ::GetModuleHandle( nullptr ), HWND( hWnd ) ) } ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "CreateRenderWindow" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RemoveWindow( /* [in] */ IRenderWindow * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			//m_internal->getRenderWindowCache().remove(  static_cast< CRenderWindow * >( val )->getName() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "RemoveWindow" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateSampler( /* [in] */ BSTR name, /* [out, retval] */ ISampler ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSampler::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSampler * >( *pVal )->setInternal( m_internal->createSampler( fromBstr( name ), *m_internal ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "createSampler" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RemoveScene( /* [in] */ BSTR name )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getSceneCache().remove( fromBstr( name ) );
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "RemoveScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadScene( /* [in] */ BSTR name, /* [out, retval] */ IRenderTarget ** pTarget )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pTarget )
			{
				castor::Path fileName{ fromBstr( name ) };
				castor3d::RenderWindowSPtr l_return;

				if ( castor::File::fileExists( fileName ) )
				{
					castor::String l_strLowered = castor::string::lowerCase( fileName );
					auto desc = doLoadSceneFile( *m_internal, fileName );

					if ( desc.renderTarget && !desc.name.empty() )
					{
						hr = CRenderTarget::CreateInstance( pTarget );

						if ( hr == S_OK )
						{
							static_cast< CRenderTarget * >( *pTarget )->setInternal( desc.renderTarget.get() );
						}
					}
				}
				else
				{
					hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "LoadScene" ), _T( "Scene file doesn't exist" ), 0, nullptr );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, _T( "LoadScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}
}

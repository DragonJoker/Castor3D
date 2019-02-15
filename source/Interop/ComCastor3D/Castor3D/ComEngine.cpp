#include "ComEngine.hpp"
#include "ComLogger.hpp"
#include "ComMesh.hpp"
#include "ComSampler.hpp"
#include "ComRenderWindow.hpp"
#include "ComScene.hpp"

#undef max
#undef min
#undef abs

#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/WindowCache.hpp>

#include <Event/Frame/FunctorEvent.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Render/RenderLoop.hpp>
#include <Scene/SceneFileParser.hpp>

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
		castor3d::RenderWindowSPtr doLoadSceneFile( castor3d::Engine & p_engine, castor::Path const & p_fileName )
		{
			castor3d::RenderWindowSPtr l_return;

			if ( castor::File::fileExists( p_fileName ) )
			{
				castor::Logger::logInfo( cuT( "Loading scene file : " ) + p_fileName );
				bool l_initialised = false;

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
						castor::Logger::logError( cuT( "Failed to parse the scene file, with following error:" ) + castor::string::stringCast< xchar >( exc.what() ) );
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

	static const castor::String ERROR_UNINITIALISED_ENGINE = cuT( "The IEngine must be initialised" );
	static const castor::String ERROR_INITIALISED_ENGINE = cuT( "The IEngine has already been initialised" );

	CEngine::CEngine()
		:	m_internal( nullptr )
	{
	}

	CEngine::~CEngine()
	{
	}

	STDMETHODIMP CEngine::Create( BSTR appName, boolean enableValidation )
	{
		HRESULT hr = E_POINTER;

		if ( !m_internal )
		{
			m_internal = new castor3d::Engine{ fromBstr( appName ), enableValidation == TRUE };
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "create" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Destroy()
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
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "Destroy" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Initialise( /* [in] */ int fps )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->initialise( fps );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "Initialise" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "Cleanup" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateScene( /* [in] */ BSTR name, /* [out, retval] */ IScene ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CScene::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CScene * >( *pVal )->setInternal( m_internal->getSceneCache().add( fromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "CreateScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::ClearScenes()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getSceneCache().clear();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "ClearScenes" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadRenderer( /* [in] */ BSTR type )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->loadRenderer( fromBstr( type ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "loadRenderer" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RenderOneFrame()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getRenderLoop().renderSyncFrame();
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "RenderOneFrame" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadPlugin( /* [in] */ BSTR path )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getPluginCache().loadPlugin( castor::Path{ fromBstr( path ) } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "loadPlugin" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateOverlay( /* [in] */ eOVERLAY_TYPE type, /* [in] */ BSTR name, /* [in] */ IOverlay * parent, /* [in] */ IScene * scene, /* [out, retval] */ IOverlay ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "CreateOverlay" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateRenderWindow( /* [in] */ BSTR name, /* [out, retval] */ IRenderWindow ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CRenderWindow::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CRenderWindow * >( *pVal )->setInternal( m_internal->getRenderWindowCache().add( fromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "CreateRenderWindow" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RemoveWindow( /* [in] */ IRenderWindow * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			//m_internal->getRenderWindowCache().remove(  static_cast< CRenderWindow * >( val )->getName() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "RemoveWindow" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateSampler( /* [in] */ BSTR name, /* [out, retval] */ ISampler ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CSampler::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CSampler * >( *pVal )->setInternal( m_internal->getSamplerCache().add( fromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "createSampler" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RemoveScene( /* [in] */ BSTR name )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->getSceneCache().remove( fromBstr( name ) );
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "RemoveScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadScene( /* [in] */ BSTR name, /* [out, retval] */ IRenderWindow ** window )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( window )
			{
				castor::Path fileName{ fromBstr( name ) };
				castor3d::RenderWindowSPtr l_return;

				if ( castor::File::fileExists( fileName ) )
				{
					castor::String l_strLowered = castor::string::lowerCase( fileName );
					m_internal->cleanup();

					bool l_continue = true;
					castor::Logger::logDebug( cuT( "GuiCommon::LoadSceneFile - Engine cleared" ) );

					try
					{
						m_internal->initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );
					}
					catch ( std::exception & exc )
					{
						castor::Logger::logError( "Castor initialisation failed with following error: " + std::string( exc.what() ) );
					}

					l_return = doLoadSceneFile( *m_internal, fileName );

					if ( l_return )
					{
						hr = CRenderWindow::CreateInstance( window );

						if ( hr == S_OK )
						{
							static_cast< CRenderWindow * >( *window )->setInternal( l_return );
						}
					}
				}
				else
				{
					hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "LoadScene" ), cuT( "Scene file doesn't exist" ), 0, nullptr );
				}
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL, IID_IEngine, cuT( "LoadScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, nullptr );
		}

		return hr;
	}
}

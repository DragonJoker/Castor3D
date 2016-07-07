#include "ComEngine.hpp"
#include "ComLogger.hpp"
#include "ComMesh.hpp"
#include "ComSampler.hpp"
#include "ComBlendState.hpp"
#include "ComDepthStencilState.hpp"
#include "ComRasteriserState.hpp"
#include "ComRenderWindow.hpp"
#include "ComScene.hpp"

#undef max
#undef min
#undef abs

#include <BlendStateCache.hpp>
#include <DepthStencilStateCache.hpp>
#include <MaterialCache.hpp>
#include <MeshCache.hpp>
#include <PluginCache.hpp>
#include <RasteriserStateCache.hpp>
#include <SamplerCache.hpp>
#include <SceneCache.hpp>
#include <WindowCache.hpp>

#include <Event/Frame/FunctorEvent.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Render/RenderLoop.hpp>
#include <Scene/SceneFileParser.hpp>

#define CASTOR3D_THREADED 0

#if defined( NDEBUG )
static const int CASTOR_WANTED_FPS	= 120;
#else
static const int CASTOR_WANTED_FPS	= 30;
#endif

namespace CastorCom
{
	namespace
	{
		Castor3D::RenderWindowSPtr DoLoadSceneFile( Castor3D::Engine & p_engine, Castor::Path const & p_fileName )
		{
			Castor3D::RenderWindowSPtr l_return;

			if ( Castor::File::FileExists( p_fileName ) )
			{
				Castor::Logger::LogInfo( cuT( "Loading scene file : " ) + p_fileName );
				bool l_initialised = false;

				if ( p_fileName.GetExtension() == cuT( "cscn" ) || p_fileName.GetExtension() == cuT( "zip" ) )
				{
					try
					{
						Castor3D::SceneFileParser l_parser( p_engine );

						if ( l_parser.ParseFile( p_fileName ) )
						{
							l_return = l_parser.GetRenderWindow();
						}
						else
						{
							Castor::Logger::LogWarning( cuT( "Can't read scene file" ) );
						}
					}
					catch ( std::exception & exc )
					{
						Castor::Logger::LogError( cuT( "Failed to parse the scene file, with following error:" ) + Castor::string::string_cast< xchar >( exc.what() ) );
					}
				}
			}
			else
			{
				Castor::Logger::LogError( cuT( "Scene file doesn't exist: " ) + p_fileName );
			}

			return l_return;
		}
	}

	static const Castor::String ERROR_UNINITIALISED_ENGINE = cuT( "The IEngine must be initialised" );
	static const Castor::String ERROR_INITIALISED_ENGINE = cuT( "The IEngine has already been initialised" );

	CEngine::CEngine()
		:	m_internal( NULL )
	{
	}

	CEngine::~CEngine()
	{
	}

	STDMETHODIMP CEngine::Create()
	{
		HRESULT hr = E_POINTER;

		if ( !m_internal )
		{
			m_internal = new Castor3D::Engine;
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "Create" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Destroy()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			delete m_internal;
			m_internal = NULL;
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "Destroy" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Initialise( /* [in] */ int fps )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Initialise( fps );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "Initialise" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::Cleanup()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Cleanup();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "Cleanup" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
					static_cast< CScene * >( *pVal )->SetInternal( m_internal->GetSceneCache().Add( FromBstr( name ), *m_internal ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::ClearScenes()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->GetSceneCache().Clear();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "ClearScenes" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadRenderer( /* [in] */ BSTR type )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->LoadRenderer( FromBstr( type ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "LoadRenderer" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RenderOneFrame()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->GetRenderLoop().RenderSyncFrame();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "RenderOneFrame" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadPlugin( /* [in] */ BSTR path )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->GetPluginCache().LoadPlugin( Castor::Path{ FromBstr( path ) } );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "LoadPlugin" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateOverlay" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RemoveWindow( /* [in] */ IRenderWindow * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "RemoveWindow" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
					static_cast< CSampler * >( *pVal )->SetInternal( m_internal->GetSamplerCache().Add( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateSampler" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateBlendState( /* [in] */ BSTR name, /* [out, retval] */ IBlendState ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CBlendState::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CBlendState * >( *pVal )->SetInternal( m_internal->GetBlendStateCache().Add( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateBlendState" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateDepthStencilState( /* [in] */ BSTR name, /* [out, retval] */ IDepthStencilState ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CDepthStencilState::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CDepthStencilState * >( *pVal )->SetInternal( m_internal->GetDepthStencilStateCache().Add( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateDepthStencilState" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateRasteriserState( /* [in] */ BSTR name, /* [out, retval] */ IRasteriserState ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CRasteriserState::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CRasteriserState * >( *pVal )->SetInternal( m_internal->GetRasteriserStateCache().Add( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateRasteriserState" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RemoveScene( /* [in] */ BSTR name )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->GetSceneCache().Remove( FromBstr( name ) );
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "RemoveScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
				Castor::Path fileName{ FromBstr( name ) };
				Castor3D::RenderWindowSPtr l_return;

				if ( Castor::File::FileExists( fileName ) )
				{
					Castor::String l_strLowered = Castor::string::lower_case( fileName );
					m_internal->Cleanup();

					bool l_continue = true;
					Castor::Logger::LogDebug( cuT( "GuiCommon::LoadSceneFile - Engine cleared" ) );

					try
					{
						m_internal->Initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );
					}
					catch ( std::exception & exc )
					{
						Castor::Logger::LogError( "Castor initialisation failed with following error: " + std::string( exc.what() ) );
					}

					l_return = DoLoadSceneFile( *m_internal, fileName );

					if ( l_return )
					{
						hr = CRenderWindow::CreateInstance( window );

						if ( hr == S_OK )
						{
							static_cast< CRenderWindow * >( *window )->SetInternal( l_return );
						}
					}
				}
				else
				{
					hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "LoadScene" ), cuT( "Scene file doesn't exist" ), 0, NULL );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "LoadScene" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
		}

		return hr;
	}
}

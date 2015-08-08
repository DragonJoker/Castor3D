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

#include <SceneFileParser.hpp>

#define CASTOR3D_THREADED 0

#if defined( NDEBUG )
static const int CASTOR_WANTED_FPS	= 120;
#else
static const int CASTOR_WANTED_FPS	= 30;
#endif

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED_ENGINE = cuT( "The IEngine must be initialised" );
	static const Castor::String ERROR_INITIALISED_ENGINE = cuT( "The IEngine has already been initialised" );

	CEngine::CEngine()
		:	m_internal( NULL )
	{
	}

	CEngine::~CEngine()
	{
	}
	
	STDMETHODIMP CEngine::Create( /* [in] */ ILogger * logger )
	{
		HRESULT hr = E_POINTER;

		if ( !m_internal )
		{
			if ( logger )
			{
				m_internal = new Castor3D::Engine( Castor::Logger::GetSingletonPtr() );
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IEngine,						// This is the GUID of component throwing error
					 cuT( "Initialise" ),				// This is generally displayed as the title
					 ERROR_INITIALISED_ENGINE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "Cleanup" ),						// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,							// This represents the error
					 IID_IEngine,						// This is the GUID of component throwing error
					 cuT( "Initialise" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,									// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "Cleanup" ),						// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
					static_cast< CScene * >( *pVal )->SetInternal( m_internal->CreateScene( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateScene" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::ClearScenes()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->ClearScenes();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "ClearScenes" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadRenderer( /* [in] */ eRENDERER_TYPE type )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->LoadRenderer( Castor3D::eRENDERER_TYPE( type ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "LoadRenderer" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RenderOneFrame()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->RenderOneFrame();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "RenderOneFrame" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadPlugin( /* [in] */ BSTR path )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->LoadPlugin( FromBstr( path ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "LoadPlugin" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateMesh( /* [in] */ eMESH_TYPE type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMesh::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CMesh * >( *pVal )->SetInternal( m_internal->CreateMesh( Castor3D::eMESH_TYPE( type ), FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateMesh" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateOverlay" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::CreateRenderWindow( /* [out, retval] */ IRenderWindow ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CRenderWindow::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CRenderWindow * >( *pVal )->SetInternal( m_internal->CreateRenderWindow() );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateRenderWindow" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "RemoveWindow" ),					// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
					static_cast< CSampler * >( *pVal )->SetInternal( m_internal->CreateSampler( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateSampler" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
					static_cast< CBlendState * >( *pVal )->SetInternal( m_internal->CreateBlendState( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateBlendState" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
					static_cast< CDepthStencilState * >( *pVal )->SetInternal( m_internal->CreateDepthStencilState( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateDepthStencilState" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
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
					static_cast< CRasteriserState * >( *pVal )->SetInternal( m_internal->CreateRasteriserState( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateRasteriserState" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::RemoveScene( /* [in] */ BSTR name )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->GetSceneManager().erase( FromBstr( name ) );
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateRasteriserState" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CEngine::LoadScene( /* [in] */ BSTR name, /* [out, retval] */ IRenderWindow ** window )
	{
		HRESULT hr = E_POINTER;
		Castor::Logger::LogDebug( cuT( "CEngine::LoadScene - Coucou 1" ) );

		if ( m_internal )
		{
			Castor::Logger::LogDebug( cuT( "CEngine::LoadScene - Coucou 2" ) );
			if ( window )
			{
				Castor::Logger::LogDebug( cuT( "CEngine::LoadScene - Coucou 3" ) );
				Castor::Path fileName = FromBstr( name );

				if ( fileName.GetExtension() != cuT( "cbsn" ) )
				{
					Castor::Path l_meshFilePath = fileName;
					Castor::str_utils::replace( l_meshFilePath, cuT( "cscn" ), cuT( "cmsh" ) );

					if ( Castor::File::FileExists( l_meshFilePath ) )
					{
						Castor::BinaryFile l_fileMesh( l_meshFilePath, Castor::File::eOPEN_MODE_READ );
						Castor::Logger::LogMessage( cuT( "Loading meshes file : " ) + l_meshFilePath );

						if ( m_internal->LoadMeshes( l_fileMesh ) )
						{
							Castor::Logger::LogMessage( cuT( "Meshes read" ) );
						}
						else
						{
							hr = CComError::DispatchError(
									 E_FAIL,								// This represents the error
									 IID_IEngine,							// This is the GUID of component throwing error
									 cuT( "LoadScene" ),					// This is generally displayed as the title
									 cuT( "Can't read meshes" ),			// This is the description
									 0,										// This is the context in the help file
									 NULL );
						}
					}
				}

				if ( Castor::File::FileExists( fileName ) )
				{
					Castor::Logger::LogMessage( cuT( "Loading scene file : " ) + fileName );

					if ( fileName.GetExtension() == cuT( "cscn" ) )
					{
						Castor3D::SceneFileParser l_parser( m_internal );

						if ( l_parser.ParseFile( fileName ) )
						{
							m_internal->Initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );
							Castor3D::RenderWindowSPtr l_pRenderWindow = l_parser.GetRenderWindow();

							if ( l_pRenderWindow )
							{
								hr = CRenderWindow::CreateInstance( window );

								if ( hr == S_OK )
								{
									static_cast< CRenderWindow * >( *window )->SetInternal( l_pRenderWindow );
								}
							}
						}
						else
						{
							Castor::Logger::LogWarning( cuT( "Can't read scene file" ) );
						}
					}
				}
				else
				{
					hr = CComError::DispatchError(
							 E_FAIL,							// This represents the error
							 IID_IEngine,						// This is the GUID of component throwing error
							 cuT( "LoadScene" ),				// This is generally displayed as the title
							 cuT( "Scene file doesn't exist" ),	// This is the description
							 0,									// This is the context in the help file
							 NULL );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,								// This represents the error
					 IID_IEngine,							// This is the GUID of component throwing error
					 cuT( "CreateRasteriserState" ),		// This is generally displayed as the title
					 ERROR_UNINITIALISED_ENGINE.c_str(),	// This is the description
					 0,										// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}

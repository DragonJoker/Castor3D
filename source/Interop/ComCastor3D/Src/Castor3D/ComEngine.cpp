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

#include <BlendStateManager.hpp>
#include <DepthStencilStateManager.hpp>
#include <FunctorEvent.hpp>
#include <InitialiseEvent.hpp>
#include <MaterialManager.hpp>
#include <MeshManager.hpp>
#include <PluginManager.hpp>
#include <RasteriserStateManager.hpp>
#include <RenderLoop.hpp>
#include <SamplerManager.hpp>
#include <SceneFileParser.hpp>
#include <SceneManager.hpp>
#include <WindowManager.hpp>

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
		template< typename TObj, typename TMgr >
		std::shared_ptr< TObj > CreateObject( Castor3D::Engine & p_engine, TMgr & p_manager )
		{
			return p_manager.Create( Castor::String(), p_engine );
		}

		template<>
		std::shared_ptr< Castor3D::Sampler > CreateObject< Castor3D::Sampler, Castor3D::SamplerManager >( Castor3D::Engine & p_engine, Castor3D::SamplerManager & p_manager )
		{
			return p_manager.Create( Castor::String() );
		}

		template<>
		std::shared_ptr< Castor3D::RenderWindow > CreateObject< Castor3D::RenderWindow, Castor3D::WindowManager >( Castor3D::Engine & p_engine, Castor3D::WindowManager & p_manager )
		{
			return p_manager.Create( Castor::String() );
		}

		template< typename TObj >
		bool DoParse( TObj & p_obj, Castor3D::BinaryChunk & p_chunk, typename TObj::BinaryParser & p_parser )
		{
			return p_parser.Parse( p_obj, p_chunk );
		}

		template< typename TObj, typename TKey >
		bool DoFillManager( Castor3D::Engine & p_engine, Castor3D::ResourceManager< TKey, TObj > & p_manager, Castor3D::BinaryChunk & p_chunk, typename TObj::BinaryParser p_parser )
		{
			return DoParse( *CreateObject< TObj >( p_engine, p_manager ), p_chunk, p_parser );
		}

		bool DoFillManager( Castor3D::Engine & p_engine, Castor3D::SamplerManager & p_manager, Castor3D::BinaryChunk & p_chunk, Castor3D::Sampler::BinaryParser p_parser )
		{
			return DoParse< Castor3D::Sampler >( *CreateObject< Castor3D::Sampler >( p_engine, p_manager ), p_chunk, p_parser );
		}

		bool DoFillManager( Castor3D::Engine & p_engine, Castor3D::WindowManager & p_manager, Castor3D::BinaryChunk & p_chunk, Castor3D::RenderWindow::BinaryParser p_parser )
		{
			return DoParse< Castor3D::RenderWindow >( *CreateObject< Castor3D::RenderWindow >( p_engine, p_manager ), p_chunk, p_parser );
		}

		bool DoLoadMeshFile( Castor3D::Engine & p_engine, Castor::Path const & p_fileName )
		{
			bool l_return = true;

			if ( p_fileName.GetExtension() != cuT( "cbsn" ) && p_fileName.GetExtension() != cuT( "zip" ) )
			{
				Castor::Path l_meshFilePath = p_fileName;
				Castor::string::replace( l_meshFilePath, cuT( "cscn" ), cuT( "cmsh" ) );

				if ( Castor::File::FileExists( l_meshFilePath ) )
				{
					Castor::BinaryFile l_fileMesh( l_meshFilePath, Castor::File::eOPEN_MODE_READ );
					Castor::Logger::LogInfo( cuT( "Loading meshes file : " ) + l_meshFilePath );

					if ( p_engine.GetMeshManager().Load( l_fileMesh ) )
					{
						Castor::Logger::LogInfo( cuT( "Meshes read" ) );
					}
					else
					{
						Castor::Logger::LogError( cuT( "Can't read meshes" ) );
						l_return = false;
					}
				}
			}

			return l_return;
		}

		Castor3D::RenderWindowSPtr DoLoadTextSceneFile( Castor3D::Engine & p_engine, Castor::Path const & p_fileName )
		{
			Castor3D::RenderWindowSPtr l_return;
			Castor3D::SceneFileParser l_parser( p_engine );

			if ( l_parser.ParseFile( p_fileName ) )
			{
				l_return = l_parser.GetRenderWindow();
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "Can't read scene file" ) );
			}

			return l_return;
		}

		Castor3D::RenderWindowSPtr DoLoadBinarySceneFile( Castor3D::Engine & p_engine, Castor::Path const & p_fileName )
		{
			Castor3D::RenderWindowSPtr l_return;
			bool l_continue = true;
			Castor::BinaryFile l_file( p_fileName, Castor::File::eOPEN_MODE_READ );
			Castor3D::BinaryChunk l_chunkFile;
			Castor3D::RenderWindowSPtr l_window;
			Castor::Path l_path = l_file.GetFilePath();
			l_chunkFile.Read( l_file );

			if ( l_chunkFile.GetChunkType() == Castor3D::eCHUNK_TYPE_CBSN_FILE )
			{
				while ( l_continue && l_chunkFile.CheckAvailable( 1 ) )
				{
					Castor3D::BinaryChunk l_chunk;
					l_continue = l_chunkFile.GetSubChunk( l_chunk );

					switch ( l_chunk.GetChunkType() )
					{
					case Castor3D::eCHUNK_TYPE_SAMPLER:
						l_continue = DoFillManager( p_engine, p_engine.GetSamplerManager(), l_chunk, Castor3D::Sampler::BinaryParser( l_path ) );
						break;

					case Castor3D::eCHUNK_TYPE_MATERIAL:
						l_continue = DoFillManager( p_engine, p_engine.GetMaterialManager(), l_chunk, Castor3D::Material::BinaryParser( l_path, &p_engine ) );
						break;

					case Castor3D::eCHUNK_TYPE_MESH:
						l_continue = DoFillManager( p_engine, p_engine.GetMeshManager(), l_chunk, Castor3D::Mesh::BinaryParser( l_path ) );
						break;

					case Castor3D::eCHUNK_TYPE_SCENE:
						l_continue = DoFillManager( p_engine, p_engine.GetSceneManager(), l_chunk, Castor3D::Scene::BinaryParser( l_path ) );
						break;

					case Castor3D::eCHUNK_TYPE_WINDOW:
						l_continue = DoFillManager( p_engine, p_engine.GetWindowManager(), l_chunk, Castor3D::RenderWindow::BinaryParser( l_path ) );
						break;
					}

					if ( !l_continue )
					{
						l_chunk.EndParse();
					}
				}

				if ( l_continue )
				{
					Castor::Logger::LogInfo( cuT( "Import successful" ) );
				}
			}
			else
			{
				Castor::Logger::LogWarning( cuT( "The given file is not a valid CBSN file: " ) + l_file.GetFileName() );
			}

			if ( !l_continue )
			{
				Castor::Logger::LogWarning( cuT( "Failed to read the binary scene file: " ) + l_file.GetFileName() );
			}

			return l_return;
		}

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
						l_return = DoLoadTextSceneFile( p_engine, p_fileName );
					}
					catch ( std::exception & exc )
					{
						Castor::Logger::LogError( cuT( "Failed to parse the scene file, with following error:" ) + Castor::string::string_cast< xchar >( exc.what() ) );
					}
				}
				else
				{
					try
					{
						l_return = DoLoadBinarySceneFile( p_engine, p_fileName );
					}
					catch ( std::exception & exc )
					{
						Castor::Logger::LogError( cuT( "Failed to parse the binary scene file, with following error: " ) + Castor::string::string_cast< xchar >( exc.what() ) );
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
					static_cast< CScene * >( *pVal )->SetInternal( m_internal->GetSceneManager().Create( FromBstr( name ), *m_internal ) );
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
			m_internal->GetSceneManager().Clear();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "ClearScenes" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
			m_internal->GetPluginManager().LoadPlugin( FromBstr( path ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "LoadPlugin" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
					static_cast< CMesh * >( *pVal )->SetInternal( m_internal->GetMeshManager().Create( FromBstr( name ), Castor3D::eMESH_TYPE( type ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateMesh" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
					static_cast< CRenderWindow * >( *pVal )->SetInternal( m_internal->GetWindowManager().Create( FromBstr( name ) ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError( E_FAIL, IID_IEngine, cuT( "CreateRenderWindow" ), ERROR_UNINITIALISED_ENGINE.c_str(), 0, NULL );
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
					static_cast< CSampler * >( *pVal )->SetInternal( m_internal->GetSamplerManager().Create( FromBstr( name ) ) );
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
					static_cast< CBlendState * >( *pVal )->SetInternal( m_internal->GetBlendStateManager().Create( FromBstr( name ) ) );
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
					static_cast< CDepthStencilState * >( *pVal )->SetInternal( m_internal->GetDepthStencilStateManager().Create( FromBstr( name ) ) );
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
					static_cast< CRasteriserState * >( *pVal )->SetInternal( m_internal->GetRasteriserStateManager().Create( FromBstr( name ) ) );
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
			m_internal->GetSceneManager().Remove( FromBstr( name ) );
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
				Castor::Path fileName = FromBstr( name );
				Castor3D::RenderWindowSPtr l_return;

				if ( Castor::File::FileExists( fileName ) )
				{
					Castor::String l_strLowered = Castor::string::lower_case( fileName );
					m_internal->Cleanup();

					bool l_continue = true;
					Castor::Logger::LogDebug( cuT( "GuiCommon::LoadSceneFile - Engine cleared" ) );

					if ( DoLoadMeshFile( *m_internal, fileName ) )
					{
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

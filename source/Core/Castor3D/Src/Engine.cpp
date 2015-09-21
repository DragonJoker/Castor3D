#include "Engine.hpp"

#include "BlendState.hpp"
#include "CleanupEvent.hpp"
#include "DebugOverlays.hpp"
#include "DepthStencilState.hpp"
#include "DividerPlugin.hpp"
#include "GenericPlugin.hpp"
#include "ImporterPlugin.hpp"
#include "InitialiseEvent.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Overlay.hpp"
#include "Pipeline.hpp"
#include "Plugin.hpp"
#include "RasteriserState.hpp"
#include "RendererPlugin.hpp"
#include "RenderSystem.hpp"
#include "RenderTarget.hpp"
#include "RenderTechnique.hpp"
#include "RenderWindow.hpp"
#include "Sampler.hpp"
#include "Scene.hpp"
#include "SceneFileParser.hpp"
#include "ShaderProgram.hpp"
#include "TechniquePlugin.hpp"
#include "TextOverlay.hpp"
#include "VersionException.hpp"

#include <Logger.hpp>
#include <Factory.hpp>
#include <File.hpp>
#include <Utils.hpp>
#include <DynamicLibrary.hpp>
#include <PreciseTimer.hpp>
#include <Templates.hpp>

using namespace Castor;

#if defined( _MSC_VER)
static const String GetTypeFunctionABIName = cuT( "?GetType@@YA?AW4ePLUGIN_TYPE@Castor3D@@XZ" );
#elif defined( __GNUG__)
static const String GetTypeFunctionABIName = cuT( "_Z7GetTypev" );
#endif

//*************************************************************************************************

namespace Castor3D
{
	template< typename T > struct StateCleanerFunc
	{
		FrameListenerSPtr m_pListener;

		StateCleanerFunc( FrameListenerSPtr p_pListener )
			:	m_pListener( p_pListener )
		{
		}

		void operator()( std::pair< String, std::shared_ptr< T > > p_pair )
		{
			m_pListener->PostEvent( MakeCleanupEvent( *p_pair.second ) );
		}

		void operator()( std::shared_ptr< T > p_pState )
		{
			m_pListener->PostEvent( MakeCleanupEvent( *p_pState ) );
		}
	};

	template< typename T > struct CleanerFunc
	{
		void operator()( std::pair< String, std::shared_ptr< T > > p_pair )
		{
			p_pair.second->Cleanup();
		}

		void operator()( std::pair< int, std::shared_ptr< T > > p_pair )
		{
			p_pair.second->Cleanup();
		}

		void operator()( std::shared_ptr< T > p_pObject )
		{
			p_pObject->Cleanup();
		}

		void operator()( T * p_pObject )
		{
			p_pObject->Cleanup();
		}
	};

	template< typename T > struct RendererFunc
	{
		void operator()( std::pair< String, std::shared_ptr< T > > p_pair )
		{
			p_pair.second->Render();
		}

		void operator()( std::pair< int, std::shared_ptr< T > > p_pair )
		{
			p_pair.second->Render();
		}

		void operator()( std::shared_ptr< T > p_pObject )
		{
			p_pObject->Render();
		}

		void operator()( T * p_pObject )
		{
			p_pObject->Render();
		}
	};

	template< typename T > struct SceneRendererFunc
	{
		Scene const & m_scene;

		SceneRendererFunc( Scene const & p_scene )
			:	m_scene( p_scene )
		{
		}

		void operator()( std::pair< String, std::shared_ptr< T > > p_pair )
		{
			p_pair.second->Render( m_scene );
		}

		void operator()( std::pair< int, std::shared_ptr< T > > p_pair )
		{
			p_pair.second->Render( m_scene );
		}

		void operator()( std::shared_ptr< T > p_pObject )
		{
			p_pObject->Render( m_scene );
		}

		void operator()( T * p_pObject )
		{
			p_pObject->Render( m_scene );
		}
	};

	//*************************************************************************************************

	CASTOR_DECLARE_UNIQUE_INSTANCE( Engine );

	Engine::Engine()
		: m_bEnded( false )
		, m_uiWantedFPS( 100 )
		, m_dFrameTime( 0.01 )
		, m_pRenderSystem( NULL )
		, m_pThreadMainLoop( nullptr )
		, m_bStarted( false )
		, m_bCreateContext( false )
		, m_bCreated( false )
		, m_bCleaned( true )
		, m_materialManager( this )
		, m_overlayManager( this )
		, m_pMainWindow( NULL )
		, m_bDefaultInitialised( false )
		, m_debugOverlays( std::make_unique< DebugOverlays >() )
	{
		CASTOR_INIT_UNIQUE_INSTANCE();

		if ( !File::DirectoryExists( GetEngineDirectory() ) )
		{
			File::DirectoryCreate( GetEngineDirectory() );
		}

		CreateFrameListener();

		Version l_version;
		String l_strVersion;
		Logger::LogInfo( l_strVersion + cuT( "Castor3D - Core engine version : " ) + string::to_string( l_version.m_iMajor ) + cuT( "." ) + string::to_string( l_version.m_iMinor ) + cuT( "." ) + string::to_string( l_version.m_iBuild ) );
		std::locale::global( std::locale() );
	}

	Engine::~Engine()
	{
		m_debugOverlays.reset();
		m_pDefaultBlendState.reset();
		m_pLightsSampler.reset();
		m_pDefaultSampler.reset();

		// To destroy before RenderSystem, since it contain elements instantiated in Renderer plug-in
		m_samplerManager.clear();
		m_shaderManager.Clear();
		m_depthStencilStateManager.clear();
		m_rasteriserStateManager.clear();
		m_blendStateManager.clear();
		m_animationManager.clear();
		m_meshManager.clear();
		m_overlayManager.clear();
		m_fontManager.clear();
		m_imageManager.clear();
		m_sceneManager.clear();
		m_materialManager.clear();
		m_arrayListeners.clear();

		// Destroy the RenderSystem
		if ( m_pRenderSystem )
		{
			m_mutexRenderers.lock();
			RendererPluginSPtr l_pPlugin = m_arrayRenderers[m_pRenderSystem->GetRendererType()];
			m_arrayRenderers[m_pRenderSystem->GetRendererType()].reset();
			m_mutexRenderers.unlock();

			if ( l_pPlugin )
			{
				l_pPlugin->DestroyRenderSystem( m_pRenderSystem );
				m_pRenderSystem = NULL;
			}
			else
			{
				Logger::LogError( cuT( "RenderSystem still exists while it's plugin doesn't exist anymore" ) );
			}
		}

		// Destroy plugins
		m_mutexLoadedPlugins.lock();
		m_mutexLibraries.lock();
		m_mapLoadedPluginTypes.clear();

		for ( auto & l_it : m_arrayRenderers )
		{
			l_it.reset();
		}

		for ( auto & l_it : m_arrayLoadedPlugins )
		{
			l_it.clear();
		}

		for ( auto & l_it : m_librariesMap )
		{
			l_it.clear();
		}

		m_mapLoadedPluginTypes.clear();
		m_mutexLibraries.unlock();
		m_mutexLoadedPlugins.unlock();
		CASTOR_CLEANUP_UNIQUE_INSTANCE();
	}

	void Engine::Initialise( uint32_t p_wantedFPS, bool p_bThreaded )
	{
		if ( m_pThreadMainLoop )
		{
			CASTOR_ASSERT( false );
			CASTOR_EXCEPTION( "Render loop is already started" );
		}
		else if ( m_bCleaned )
		{
			m_bCleaned = false;
			m_uiWantedFPS = p_wantedFPS;
			m_bThreaded = p_bThreaded;
			m_dFrameTime = 1.0 / m_uiWantedFPS;
			DoLoadCoreData();

			if ( m_bThreaded )
			{
				m_pThreadMainLoop.reset( new std::thread( std::bind( &Engine::DoMainLoop, this ) ) );
			}
		}
	}

	void Engine::Cleanup()
	{
		if ( !IsCleaned() )
		{
			ClearScenes();

			m_depthStencilStateManager.lock();

			for ( auto && l_it : m_depthStencilStateManager )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_depthStencilStateManager.unlock();
			m_rasteriserStateManager.lock();

			for ( auto && l_it : m_rasteriserStateManager )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_rasteriserStateManager.unlock();
			m_blendStateManager.lock();

			for ( auto && l_it : m_blendStateManager )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_blendStateManager.unlock();
			m_samplerManager.lock();

			for ( auto && l_it : m_samplerManager )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_samplerManager.unlock();
			m_meshManager.lock();

			for ( auto && l_it : m_meshManager )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_meshManager.unlock();
			m_overlayManager.lock();

			for ( auto && l_it : reinterpret_cast< OverlayCollection & >( m_overlayManager ) )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_overlayManager.unlock();
			m_materialManager.lock();

			for ( auto && l_it : m_materialManager )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_materialManager.unlock();

			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );

				for ( auto && l_it : m_shaderManager )
				{
					m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it ) );
				}

				for ( auto && l_it : m_mapWindows )
				{
					m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *l_it.second ) );
				}
			}

			if ( m_pDefaultBlendState )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *m_pDefaultBlendState ) );
			}

			if ( m_pLightsSampler )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *m_pLightsSampler ) );
			}

			if ( m_pDefaultSampler )
			{
				m_arrayListeners[0]->PostEvent( MakeCleanupEvent( *m_pDefaultSampler ) );
			}

			SetCleaned();

			if ( m_pThreadMainLoop )
			{
				// We wait for the main loop to end (it makes a final render to clean the render system)
				m_pThreadMainLoop->join();
				m_pThreadMainLoop.reset();
			}
			else
			{
				// No render loop so we clean the render system ourselves with that single call
				DoRenderFlushFrame();

				if ( GetRenderSystem()->GetMainContext() )
				{
					GetRenderSystem()->GetMainContext()->Cleanup();
				}

				GetRenderSystem()->Cleanup();
			}

			m_samplerManager.clear();
			m_shaderManager.ClearShaders();
			m_overlayManager.ClearOverlays();
			m_materialManager.DeleteAll();
			m_sceneManager.clear();
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
				m_meshManager.clear();
			}
			m_animationManager.clear();
			m_fontManager.clear();
			m_imageManager.clear();
			m_shaderManager.Clear();
			m_depthStencilStateManager.clear();
			m_rasteriserStateManager.clear();
			m_blendStateManager.clear();
			RemoveAllRenderWindows();

			for ( auto && l_listener : m_arrayListeners )
			{
				l_listener->Flush();
			}

			FrameListenerSPtr l_pListener = m_arrayListeners[0];

			if ( m_pDefaultBlendState )
			{
				l_pListener->PostEvent( MakeInitialiseEvent( *m_pDefaultBlendState ) );
			}

			if ( m_pDefaultSampler )
			{
				l_pListener->PostEvent( MakeInitialiseEvent( *m_pDefaultSampler ) );
			}

			if ( m_pLightsSampler )
			{
				l_pListener->PostEvent( MakeInitialiseEvent( *m_pLightsSampler ) );
			}
		}
	}

	ContextSPtr Engine::CreateContext( RenderWindow * p_pRenderWindow )
	{
		ContextSPtr l_pReturn;

		if ( !m_pThreadMainLoop )
		{
			l_pReturn = m_pRenderSystem->GetMainContext();

			if ( !l_pReturn )
			{
				l_pReturn = m_pRenderSystem->CreateContext();

				if ( l_pReturn->Initialise( p_pRenderWindow ) )
				{
					m_pRenderSystem->SetMainContext( l_pReturn );
				}
			}
			else if ( !l_pReturn->IsInitialised() )
			{
				l_pReturn->Initialise( p_pRenderWindow );
			}
		}
		else
		{
			if ( !IsToCreate() )
			{
				{
					CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
					m_pMainWindow = p_pRenderWindow;
				}
				SetToCreate();

				while ( !IsCreated() )
				{
					System::Sleep( 1 );
				}

				{
					CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
					m_bCreateContext = false;
				}

				l_pReturn = m_pRenderSystem->GetMainContext();
			}
		}

		return l_pReturn;
	}

	void Engine::StartRendering()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bEnded = false;

		if ( !m_pThreadMainLoop )
		{
			CASTOR_ASSERT( false );
			CASTOR_EXCEPTION( "Rendering is not threaded" );
		}
		else
		{
			m_bStarted = true;
		}
	}

	void Engine::EndRendering()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bEnded = true;

		if ( !m_pThreadMainLoop )
		{
			CASTOR_ASSERT( false );
			CASTOR_EXCEPTION( "Rendering is not threaded" );
		}
		else
		{
			m_bStarted = false;
		}
	}

	void Engine::RenderOneFrame()
	{
		if ( m_pThreadMainLoop )
		{
			CASTOR_ASSERT( false );
			CASTOR_EXCEPTION( "Can't call RenderOneFrame in threaded mode" );
		}
		else
		{
			DoRenderOneFrame();
		}
	}

	void Engine::RenderOverlays( Scene const & p_scene, Castor::Size const & p_size )
	{
		m_overlayManager.RenderOverlays( p_scene, p_size );
	}

	SceneSPtr Engine::CreateScene( String const & p_strName )
	{
		SceneSPtr l_pReturn = m_sceneManager.find( p_strName );

		if ( ! l_pReturn )
		{
			l_pReturn = std::make_shared< Scene >( this, m_lightFactory, p_strName );
			l_pReturn->Initialise();
			m_sceneManager.insert( p_strName, l_pReturn );
		}

		return l_pReturn;
	}

	void Engine::ClearScenes()
	{
		m_sceneManager.lock();

		for ( auto l_it : m_sceneManager )
		{
			l_it.second->ClearScene();
		}

		m_sceneManager.unlock();
	}

	MeshSPtr Engine::CreateMesh( eMESH_TYPE p_eType, Castor::String const & p_strMeshName )
	{
		return CreateMesh( p_eType, p_strMeshName, UIntArray(), RealArray() );
	}

	MeshSPtr Engine::CreateMesh( eMESH_TYPE p_eType, Castor::String const & p_strMeshName, UIntArray const & p_arrayFaces )
	{
		return CreateMesh( p_eType, p_strMeshName, p_arrayFaces, RealArray() );
	}

	MeshSPtr Engine::CreateMesh( eMESH_TYPE p_eType, Castor::String const & p_strMeshName, UIntArray const & p_arrayFaces, RealArray const & p_arraySizes )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		MeshSPtr l_pReturn = m_meshManager.find( p_strMeshName );

		if ( !l_pReturn )
		{
			l_pReturn = std::make_shared< Mesh >( this, p_eType, p_strMeshName );
			l_pReturn->Initialise( p_arrayFaces, p_arraySizes );
			m_meshManager.insert( p_strMeshName, l_pReturn );
			Logger::LogInfo( cuT( "Engine::CreateMesh - Mesh [" ) + p_strMeshName + cuT( "] - Created" ) );
		}
		else
		{
			Logger::LogWarning( cuT( "Engine::CreateMesh - Can't create Mesh [" ) + p_strMeshName + cuT( "] - Another mesh with the same name already exists" ) );
		}

		return  l_pReturn;
	}

	bool Engine::SaveMeshes( BinaryFile & p_file )
	{
		m_meshManager.lock();
		Path l_path = p_file.GetFileFullPath();
		bool l_return = true;
		MeshCollectionConstIt l_it = m_meshManager.begin();

		if ( l_path.GetExtension() == cuT( "cmsh" ) )
		{
			l_return = p_file.Write( uint32_t( m_meshManager.size() ) ) == sizeof( uint32_t );

			while ( l_it != m_meshManager.end() && l_return )
			{
				//l_return = Mesh::BinaryLoader()( * l_it->second, p_file);
				++l_it;
			}
		}
		else
		{
			l_path = l_path.GetPath() / l_path.GetFileName() + cuT( ".cmsh" );
			BinaryFile l_file( l_path, File::eOPEN_MODE_WRITE );
			l_return = l_file.Write( uint32_t( m_meshManager.size() ) ) == sizeof( uint32_t );

			while ( l_it != m_meshManager.end() && l_return )
			{
				//l_return = Mesh::BinaryLoader()( * l_it->second, p_file);
				++l_it;
			}
		}

		m_meshManager.unlock();
		return l_return;
	}

	bool Engine::LoadMeshes( BinaryFile & p_file )
	{
		uint32_t l_uiSize;
		bool l_return = p_file.Read( l_uiSize ) == sizeof( uint32_t );

		for ( uint32_t i = 0; i < l_uiSize && l_return; i++ )
		{
			MeshSPtr l_pMesh = m_meshManager.find( cuEmptyString );

			if ( ! l_pMesh )
			{
				l_pMesh = std::make_shared< Mesh >( this, eMESH_TYPE_CUSTOM, cuEmptyString );
				m_meshManager.insert( cuEmptyString, l_pMesh );
			}

			//l_return = Mesh::BinaryLoader()( * l_pMesh, p_file);
		}

		return l_return;
	}

	OverlaySPtr Engine::CreateOverlay( eOVERLAY_TYPE p_eType, String const & p_strName, OverlaySPtr p_pParent, SceneSPtr p_pScene )
	{
		OverlaySPtr l_pReturn = m_overlayManager.GetOverlay( p_strName );

		if ( !l_pReturn )
		{
			l_pReturn = std::make_shared< Overlay >( this, p_eType, p_pScene, p_pParent );
			l_pReturn->SetName( p_strName );

			if ( p_pScene )
			{
				p_pScene->AddOverlay( l_pReturn );
			}

			m_overlayManager.AddOverlay( p_strName, l_pReturn, p_pParent );
			Logger::LogInfo( cuT( "Scene::CreateOverlay - Overlay [" ) + p_strName + cuT( "] - Created" ) );
		}
		else
		{
			Logger::LogWarning( cuT( "Scene::CreateOverlay - Can't create Overlay [" ) + p_strName + cuT( "] - Another overlay with the same name already exists" ) );
		}

		return  l_pReturn;
	}

	RenderWindowSPtr Engine::CreateRenderWindow()
	{
		RenderWindowSPtr l_pReturn = m_pRenderSystem->CreateRenderWindow();
		{
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
			m_mapWindows[l_pReturn->GetIndex()] = l_pReturn;
		}
		return l_pReturn;
	}

	bool Engine::RemoveRenderWindow( uint32_t p_uiIndex )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		bool l_return = false;
		RenderWindowMap::iterator l_it = m_mapWindows.find( p_uiIndex );

		if ( l_it != m_mapWindows.end() )
		{
			m_mapWindows.erase( l_it );
			l_return = true;
		}

		return l_return;
	}

	bool Engine::RemoveRenderWindow( RenderWindowSPtr p_pWindow )
	{
		bool l_return = false;

		if ( p_pWindow )
		{
			l_return = RemoveRenderWindow( p_pWindow->GetIndex() );
		}

		return l_return;
	}

	void Engine::RemoveAllRenderWindows()
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		clear_pair_container( m_mapWindows );
	}

	PluginBaseSPtr Engine::LoadPlugin( String const & p_strPluginName, Path const & p_pathFolder )throw()
	{
		Path l_strFilePath = CASTOR_DLL_PREFIX + p_strPluginName + cuT( "." ) + CASTOR_DLL_EXT;
		PluginBaseSPtr l_pReturn;

		try
		{
			l_pReturn = InternalLoadPlugin( l_strFilePath );
		}
		catch ( VersionException & p_exc )
		{
			Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
		}
		catch ( PluginException & p_exc )
		{
			if ( !p_pathFolder.empty() )
			{
				l_pReturn = LoadPlugin( p_pathFolder / l_strFilePath );
			}
			else
			{
				Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
			}
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - " ) + string::string_cast< xchar >( p_exc.what() ) );
		}
		catch ( ... )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - Unknown error" ) );
		}

		return l_pReturn;
	}

	PluginBaseSPtr Engine::LoadPlugin( Path const & p_fileFullPath )throw()
	{
		PluginBaseSPtr l_pReturn;

		try
		{
			l_pReturn = InternalLoadPlugin( p_fileFullPath );
		}
		catch ( VersionException & p_exc )
		{
			Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
		}
		catch ( PluginException & p_exc )
		{
			Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - " ) + string::string_cast< xchar >( p_exc.what() ) );
		}
		catch ( ... )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - Unknown error" ) );
		}

		return l_pReturn;
	}

	PluginStrMap Engine::GetPlugins( ePLUGIN_TYPE p_type )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexLoadedPlugins );
		return m_arrayLoadedPlugins[p_type];
	}

	bool Engine::LoadRenderer( eRENDERER_TYPE p_eType )
	{
		bool l_return = false;
		m_mutexRenderers.lock();
		RendererPluginSPtr l_pPlugin = m_arrayRenderers[p_eType];
		m_mutexRenderers.unlock();

		if ( l_pPlugin )
		{
			m_pRenderSystem = l_pPlugin->CreateRenderSystem( this );
			m_shaderManager.SetRenderSystem( m_pRenderSystem );
			m_pDefaultBlendState = CreateBlendState( cuT( "Default" ) );
			m_pDefaultSampler = CreateSampler( cuT( "Default" ) );
			m_pDefaultSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
			m_pDefaultSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
			m_pDefaultSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIP, eINTERPOLATION_MODE_LINEAR );
			m_pLightsSampler = CreateSampler( cuT( "LightsSampler" ) );
			m_pLightsSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_NEAREST );
			m_pLightsSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_NEAREST );
			l_return = true;
		}

		return l_return;
	}

	void Engine::LoadAllPlugins( Path const & p_strFolder )
	{
		PathArray l_arrayFiles;
		File::ListDirectoryFiles( p_strFolder, l_arrayFiles );

		if ( l_arrayFiles.size() > 0 )
		{
			for ( uint32_t i = 0; i < l_arrayFiles.size(); i++ )
			{
				Path l_file = l_arrayFiles[i];

				if ( l_file.GetExtension() == CASTOR_DLL_EXT )
				{
					try
					{
						InternalLoadPlugin( l_file );
					}
					catch ( ... )
					{
						Logger::LogInfo( cuT( "Can't load plugin : " ) + l_file );
					}
				}
			}
		}
	}

	void Engine::PostEvent( FrameEventSPtr p_pEvent )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );

		if ( m_arrayListeners.size() )
		{
			m_arrayListeners[0]->PostEvent( p_pEvent );
		}
	}

	Path Engine::GetPluginsDirectory()
	{
		Path l_pathReturn;
		Path l_pathBin = File::DirectoryGetCurrent();
		Path l_pathUsr = l_pathBin.GetPath();
		l_pathReturn = l_pathUsr / cuT( "lib" ) / cuT( "Castor3D" );
		return l_pathReturn;
	}

	Castor::Path Engine::GetEngineDirectory()
	{
		return File::GetUserDirectory() / cuT( ".Castor3D" );
	}

	Path Engine::GetDataDirectory()
	{
		Path l_pathReturn;
		Path l_pathBin = File::DirectoryGetCurrent();
		Path l_pathUsr = l_pathBin.GetPath();
		l_pathReturn = l_pathUsr / cuT( "share" );
		return l_pathReturn;
	}

	PluginBaseSPtr Engine::LoadRendererPlugin( DynamicLibrarySPtr p_pLibrary )
	{
		RendererPluginSPtr l_pRenderer = std::make_shared< RendererPlugin >( p_pLibrary, this );
		PluginBaseSPtr l_pReturn = std::static_pointer_cast<PluginBase, RendererPlugin>( l_pRenderer );
		eRENDERER_TYPE l_eRendererType = l_pRenderer->GetRendererType();

		if ( l_eRendererType == eRENDERER_TYPE_UNDEFINED )
		{
			l_pReturn.reset();
		}
		else
		{
			m_mutexRenderers.lock();
			m_arrayRenderers[l_eRendererType] = l_pRenderer;
			m_mutexRenderers.unlock();
		}

		return l_pReturn;
	}

	PluginBaseSPtr Engine::LoadTechniquePlugin( DynamicLibrarySPtr p_pLibrary )
	{
		return std::make_shared< TechniquePlugin >( p_pLibrary, this );
	}

	PluginBaseSPtr Engine::InternalLoadPlugin( Path const & p_pathFile )
	{
		PluginBaseSPtr l_pReturn;
		m_mutexLoadedPluginTypes.lock();
		PluginTypePathMapIt l_it = m_mapLoadedPluginTypes.find( p_pathFile );
		PluginTypePathMapConstIt l_itEnd = m_mapLoadedPluginTypes.end();

		if ( l_it == l_itEnd )
		{
			m_mutexLoadedPluginTypes.unlock();

			if ( File::FileExists( p_pathFile ) )
			{
				DynamicLibrarySPtr l_pLibrary = std::make_shared< DynamicLibrary >();

				if ( !l_pLibrary->Open( p_pathFile ) )
				{
					String l_strError = cuT( "Error encountered while loading file [" ) + p_pathFile + cuT( "] : " );
					l_strError += System::GetLastErrorText();
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
				}

				PluginBase::PGetTypeFunction l_pfnGetType;

				if ( !l_pLibrary->GetFunction( l_pfnGetType, GetTypeFunctionABIName ) )
				{
					String l_strError = cuT( "Error encountered while loading file [" ) + p_pathFile.GetFileName() + cuT( "] GetType plugin function => Not a Castor3D plugin" );
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
				}

				ePLUGIN_TYPE l_eType = l_pfnGetType();

				switch ( l_eType )
				{
				case ePLUGIN_TYPE_DIVIDER:
					l_pReturn = std::make_shared< DividerPlugin >( l_pLibrary, this );
					break;

				case ePLUGIN_TYPE_IMPORTER:
					l_pReturn = std::make_shared< ImporterPlugin >( l_pLibrary, this );
					break;

				case ePLUGIN_TYPE_RENDERER:
				{
					l_pReturn = LoadRendererPlugin( l_pLibrary );
				}
				break;

				case ePLUGIN_TYPE_GENERIC:
				{
					l_pReturn = std::make_shared< GenericPlugin >( l_pLibrary, this );
				}
				break;

				case ePLUGIN_TYPE_TECHNIQUE:
				{
					l_pReturn = LoadTechniquePlugin( l_pLibrary );
				}
				break;

				default:
					break;
				}

				if ( l_pReturn )
				{
					Version l_toCheck( 0, 0 );
					l_pReturn->GetRequiredVersion( l_toCheck );
					String l_strToLog( cuT( "LoadPlugin - Plugin [" ) );
					Logger::LogInfo( l_strToLog + l_pReturn->GetName() + cuT( "] - Required engine version : " ) + string::to_string( l_toCheck.m_iMajor ) + cuT( "." ) + string::to_string( l_toCheck.m_iMinor ) + cuT( "." ) + string::to_string( l_toCheck.m_iBuild ) );

					if ( l_toCheck <= m_version )
					{
						m_mutexLoadedPluginTypes.lock();
						m_mapLoadedPluginTypes.insert( std::make_pair( p_pathFile, l_eType ) );
						m_mutexLoadedPluginTypes.unlock();
						m_mutexLoadedPlugins.lock();
						m_arrayLoadedPlugins[l_eType].insert( std::make_pair( p_pathFile, l_pReturn ) );
						m_mutexLoadedPlugins.unlock();
						m_mutexLibraries.lock();
						m_librariesMap[l_eType].insert( std::make_pair( p_pathFile, l_pLibrary ) );
						m_mutexLibraries.unlock();
						l_strToLog = cuT( "LoadPlugin - Plugin [" );
						Logger::LogInfo( l_strToLog + l_pReturn->GetName() + cuT( "] loaded" ) );
					}
					else
					{
						CASTOR_VERSION_EXCEPTION( l_toCheck, m_version );
					}
				}
				else
				{
					// Plugin not loaded, due to an error
				}
			}
			else
			{
				// File doesn't exist
			}
		}
		else
		{
			ePLUGIN_TYPE l_eType = l_it->second;
			m_mutexLoadedPluginTypes.unlock();
			m_mutexLoadedPlugins.lock();
			l_pReturn = m_arrayLoadedPlugins[l_eType].find( p_pathFile )->second;
			m_mutexLoadedPlugins.unlock();
		}

		return l_pReturn;
	}

	void Engine::UpdateOverlayManager()
	{
		m_overlayManager.Update();
	}

	void Engine::UpdateShaderManager()
	{
		m_shaderManager.Update();
	}

	RenderTechniqueBaseSPtr Engine::CreateTechnique( Castor::String const & p_strName, RenderTarget & p_renderTarget, Parameters const & p_params )
	{
		return m_techniqueFactory.Create( p_strName, p_renderTarget, m_pRenderSystem, p_params );
	}

	bool Engine::IsEnded()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		return m_bEnded;
	}

	void Engine::SetEnded()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bEnded = true;
	}

	bool Engine::IsCleaned()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		return m_bCleaned;
	}

	void Engine::SetCleaned()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bCleaned = true;
	}

	bool Engine::IsToCreate()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		return m_bCreateContext;
	}

	void Engine::SetToCreate()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bCreateContext = true;
	}

	bool Engine::IsCreated()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		return m_bCreated;
	}

	void Engine::SetCreated()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bCreated = true;
	}

	bool Engine::IsStarted()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		return m_bStarted;
	}

	void Engine::SetStarted()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bStarted = true;
	}

	double Engine::GetFrameTime()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		return m_dFrameTime;
	}

	bool Engine::SupportsShaderModel( eSHADER_MODEL p_eShaderModel )
	{
		bool l_return = false;

		if ( m_pRenderSystem )
		{
			l_return = m_pRenderSystem->CheckSupport( p_eShaderModel );
		}

		return l_return;
	}

	bool Engine::SupportsDepthBuffer()const
	{
		bool l_return = false;

		if ( m_pRenderSystem )
		{
			l_return = m_pRenderSystem->SupportsDepthBuffer();
		}

		return l_return;
	}

	RenderTargetSPtr Engine::CreateRenderTarget( eTARGET_TYPE p_eType )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		RenderTargetSPtr l_pReturn = m_pRenderSystem->CreateRenderTarget( p_eType );
		m_mapRenderTargets.insert( std::make_pair( p_eType, l_pReturn ) );
		return l_pReturn;
	}

	void Engine::RemoveRenderTarget( RenderTargetSPtr && p_pRenderTarget )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		eTARGET_TYPE l_eType = p_pRenderTarget->GetTargetType();
		RenderTargetMMapIt l_it = m_mapRenderTargets.find( l_eType );

		while ( l_it != m_mapRenderTargets.end() )
		{
			if ( l_it->first != l_eType )
			{
				l_it = m_mapRenderTargets.end();
			}
			else if ( l_it->second == p_pRenderTarget )
			{
				m_mapRenderTargets.erase( l_it );
				l_it = m_mapRenderTargets.end();
			}
			else
			{
				++l_it;
			}
		}
	}

	FrameListenerSPtr Engine::CreateFrameListener()
	{
		FrameListenerSPtr l_pReturn = std::make_shared< FrameListener >();
		m_arrayListeners.push_back( l_pReturn );
		return l_pReturn;
	}

	void Engine::DestroyFrameListener( FrameListenerSPtr & p_pListener )
	{
		FrameListenerPtrArrayIt l_it = std::find( m_arrayListeners.begin(), m_arrayListeners.end(), p_pListener );

		if ( l_it != m_arrayListeners.end() )
		{
			m_arrayListeners.erase( l_it );
		}

		p_pListener.reset();
	}

	SamplerSPtr Engine::CreateSampler( String const & p_strName )
	{
		SamplerSPtr l_pReturn;

		if ( p_strName.empty() )
		{
			l_pReturn = m_pRenderSystem->CreateSampler( p_strName );
		}
		else
		{
			l_pReturn = m_samplerManager.find( p_strName );

			if ( !l_pReturn )
			{
				l_pReturn = m_pRenderSystem->CreateSampler( p_strName );
				m_samplerManager.insert( p_strName, l_pReturn );
				m_arrayListeners[0]->PostEvent( MakeInitialiseEvent( *l_pReturn ) );
			}
		}

		return l_pReturn;
	}

	DepthStencilStateSPtr Engine::CreateDepthStencilState( String const & p_strName )
	{
		DepthStencilStateSPtr l_pReturn = m_depthStencilStateManager.find( p_strName );

		if ( m_pRenderSystem && !l_pReturn )
		{
			l_pReturn = m_pRenderSystem->CreateDepthStencilState();
			m_depthStencilStateManager.insert( p_strName, l_pReturn );
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
			m_arrayListeners[0]->PostEvent( MakeInitialiseEvent( *l_pReturn ) );
		}

		return l_pReturn;
	}

	RasteriserStateSPtr Engine::CreateRasteriserState( String const & p_strName )
	{
		RasteriserStateSPtr l_pReturn = m_rasteriserStateManager.find( p_strName );

		if ( m_pRenderSystem && !l_pReturn )
		{
			l_pReturn = m_pRenderSystem->CreateRasteriserState();
			m_rasteriserStateManager.insert( p_strName, l_pReturn );
			m_arrayListeners[0]->PostEvent( MakeInitialiseEvent( *l_pReturn ) );
		}

		return l_pReturn;
	}

	BlendStateSPtr Engine::CreateBlendState( String const & p_strName )
	{
		BlendStateSPtr l_pReturn = m_blendStateManager.find( p_strName );

		if ( m_pRenderSystem && !l_pReturn )
		{
			l_pReturn = m_pRenderSystem->CreateBlendState();
			m_blendStateManager.insert( p_strName, l_pReturn );
			m_arrayListeners[0]->PostEvent( MakeInitialiseEvent( *l_pReturn ) );
		}

		return l_pReturn;
	}

	void Engine::ShowDebugOverlays( bool p_show )
	{
		m_debugOverlays->Show( p_show );
	}

	void Engine::DoPreRender()
	{
		PreciseTimer l_timer;
		m_pRenderSystem->GetMainContext()->SetCurrent();
		m_debugOverlays->EndGpuTask();

		for ( auto && l_listener : m_arrayListeners )
		{
			l_listener->FireEvents( eEVENT_TYPE_PRE_RENDER );
		}

		UpdateOverlayManager();
		UpdateShaderManager();

		m_debugOverlays->EndCpuTask();
		m_pRenderSystem->GetMainContext()->EndCurrent();
	}

	void Engine::DoRender( bool p_bForce, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount )
	{
		m_pRenderSystem->GetMainContext()->SetCurrent();

		// Reverse iterator because we want to render textures before windows
		for ( auto l_rit = m_mapRenderTargets.rbegin(); l_rit != m_mapRenderTargets.rend(); ++l_rit )
		{
			p_objCount += l_rit->second->GetScene()->GetGeometriesCount();
			p_fceCount += l_rit->second->GetScene()->GetFaceCount();
			p_vtxCount += l_rit->second->GetScene()->GetVertexCount();
			l_rit->second->Render( m_dFrameTime );
		}

		m_debugOverlays->EndGpuTask();

		for ( auto && l_listener : m_arrayListeners )
		{
			l_listener->FireEvents( eEVENT_TYPE_QUEUE_RENDER );
		}

		m_debugOverlays->EndCpuTask();

		for ( auto && l_it : m_mapWindows )
		{
			l_it.second->RenderOneFrame( p_bForce );
		}

		m_pRenderSystem->GetMainContext()->EndCurrent();
		m_debugOverlays->EndGpuTask();
	}

	void Engine::DoPostRender()
	{
		m_debugOverlays->EndGpuTask();

		for ( auto && l_listener : m_arrayListeners )
		{
			l_listener->FireEvents( eEVENT_TYPE_POST_RENDER );
		}

		m_debugOverlays->EndCpuTask();
	}

	void Engine::DoUpdate( bool p_bForce )
	{
		if ( m_pRenderSystem->GetMainContext() )
		{
			uint32_t l_vertices = 0;
			uint32_t l_faces = 0;
			uint32_t l_objects = 0;
			m_debugOverlays->StartFrame();
			DoPreRender();
			DoRender( p_bForce, l_vertices, l_faces, l_objects );
			DoPostRender();
			m_debugOverlays->EndFrame( l_vertices, l_faces, l_objects );
		}
	}

	uint32_t Engine::DoMainLoop()
	{
		uint32_t l_uiReturn = 0;
		PreciseTimer l_timer;
		ContextSPtr l_pContext;

		// Tant qu'on n'est pas nettoyé, on continue
		while ( !IsCleaned() )
		{
			// Tant qu'on n'a pas de contexte et qu'on ne nous a pas demandé de le créer, on attend.
			while ( !IsCleaned() && !IsToCreate() && !IsCreated() )
			{
				System::Sleep( 10 );
			}

			if ( !IsCleaned() && !IsCreated() )
			{
				// On nous a demandé de le créer, on le crée
				l_pContext = m_pRenderSystem->CreateContext();
				l_pContext->Initialise( m_pMainWindow );
				m_pRenderSystem->SetMainContext( l_pContext );
				SetCreated();
			}

			// Tant qu'on n'a pas demandé le début du rendu, on attend.
			while ( !IsCleaned() && !IsStarted() )
			{
				System::Sleep( 10 );
			}

			l_pContext.reset();

			// Le rendu est en cours
			while ( !IsCleaned() && !IsEnded() )
			{
				double l_dFrameTime = GetFrameTime();
				l_timer.TimeS();
				DoRenderOneFrame();
				double l_dTimeDiff = l_timer.TimeS();

				if ( l_dTimeDiff < l_dFrameTime )
				{
					System::Sleep( uint32_t( ( l_dFrameTime - l_dTimeDiff ) * 1000 ) );
				}
				else
				{
					// In order to let the CPU work, we sleep at least 1ms
					System::Sleep( 1 );
				}
			}
		}

		// A final render to clean the renderers
		DoRenderFlushFrame();
		return l_uiReturn;
	}

	void Engine::DoRenderOneFrame()
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );

		if ( !m_mapWindows.empty() )
		{
			DoUpdate( true );
		}
		else
		{
			System::Sleep( 1 );
		}
	}

	void Engine::DoRenderFlushFrame()
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );

		if ( !m_mapWindows.empty() )
		{
			DoUpdate( true );
		}
	}

	void Engine::DoLoadCoreData()
	{
		Path l_path = Engine::GetDataDirectory() / cuT( "Castor3D" );

		if ( File::FileExists( l_path / cuT( "Core.zip" ) ) )
		{
			SceneFileParser l_parser( this );

			if ( !l_parser.ParseFile( l_path / cuT( "Core.zip" ) ) )
			{
				Logger::LogError( cuT( "Can't read Core.zip data file" ) );
			}
			else
			{
				m_debugOverlays->Initialise( m_overlayManager );
			}
		}
	}
}

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
#include "PostFxPlugin.hpp"
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
	static const char * C3D_NO_RENDERSYSTEM = "No RenderSystem loaded, call Castor3D::Engine::LoadRenderer before Castor3D::Engine::Initialise";
	static const char * C3D_MAIN_LOOP_EXISTS = "Render loop is already started";

	Engine::Engine()
		: m_bEnded( false )
		, m_uiWantedFPS( 100 )
		, m_dFrameTime( 0.01 )
		, m_renderSystem( NULL )
		, m_mainLoopThread( nullptr )
		, m_bStarted( false )
		, m_bCreateContext( false )
		, m_bCreated( false )
		, m_bCleaned( true )
		, m_materialManager( *this )
		, m_overlayManager( *this )
		, m_pMainWindow( NULL )
		, m_bDefaultInitialised( false )
		, m_debugOverlays( std::make_unique< DebugOverlays >() )
	{
		CASTOR_INIT_UNIQUE_INSTANCE();

		if ( !File::DirectoryExists( GetEngineDirectory() ) )
		{
			File::DirectoryCreate( GetEngineDirectory() );
		}

		m_defaultListener = CreateFrameListener( cuT( "Default" ) );

		Version l_version;
		String l_strVersion;
		Logger::LogInfo( StringStream() << cuT( "Castor3D - Core engine version : " ) << l_version );
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
		m_overlayManager.Clear();
		m_fontManager.clear();
		m_imageManager.clear();
		m_sceneManager.clear();
		m_materialManager.clear();
		m_listeners.clear();

		// Destroy the RenderSystem
		if ( m_renderSystem )
		{
			m_mutexRenderers.lock();
			RendererPluginSPtr l_pPlugin = m_arrayRenderers[m_renderSystem->GetRendererType()];
			m_arrayRenderers[m_renderSystem->GetRendererType()].reset();
			m_mutexRenderers.unlock();

			if ( l_pPlugin )
			{
				l_pPlugin->DestroyRenderSystem( m_renderSystem );
				m_renderSystem = NULL;
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
		if ( !m_renderSystem )
		{
			CASTOR_ASSERT( false );
			CASTOR_EXCEPTION( C3D_NO_RENDERSYSTEM );
		}

		if ( m_mainLoopThread )
		{
			CASTOR_ASSERT( false );
			CASTOR_EXCEPTION( C3D_MAIN_LOOP_EXISTS );
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
				m_mainLoopThread.reset( new std::thread( std::bind( &Engine::DoMainLoop, this ) ) );
			}
		}
	}

	void Engine::Cleanup()
	{
		if ( !IsCleaned() )
		{
			SetCleaned();

			for ( auto && l_listener : m_listeners )
			{
				l_listener.second->Flush();
			}

			ClearScenes();

			m_depthStencilStateManager.lock();

			for ( auto && l_it : m_depthStencilStateManager )
			{
				PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_depthStencilStateManager.unlock();
			m_rasteriserStateManager.lock();

			for ( auto && l_it : m_rasteriserStateManager )
			{
				PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_rasteriserStateManager.unlock();
			m_blendStateManager.lock();

			for ( auto && l_it : m_blendStateManager )
			{
				PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_blendStateManager.unlock();
			m_samplerManager.lock();

			for ( auto && l_it : m_samplerManager )
			{
				PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_samplerManager.unlock();
			m_meshManager.lock();

			for ( auto && l_it : m_meshManager )
			{
				PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_meshManager.unlock();
			m_overlayManager.Cleanup();
			m_materialManager.lock();

			for ( auto && l_it : m_materialManager )
			{
				PostEvent( MakeCleanupEvent( *l_it.second ) );
			}

			m_materialManager.unlock();

			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );

				for ( auto && l_it : m_shaderManager )
				{
					PostEvent( MakeCleanupEvent( *l_it ) );
				}

				for ( auto && l_it : m_mapWindows )
				{
					PostEvent( MakeCleanupEvent( *l_it.second ) );
				}
			}

			if ( m_pDefaultBlendState )
			{
				PostEvent( MakeCleanupEvent( *m_pDefaultBlendState ) );
			}

			if ( m_pLightsSampler )
			{
				PostEvent( MakeCleanupEvent( *m_pLightsSampler ) );
			}

			if ( m_pDefaultSampler )
			{
				PostEvent( MakeCleanupEvent( *m_pDefaultSampler ) );
			}

			if ( m_mainLoopThread )
			{
				// We wait for the main loop to end (it makes a final render to clean the render system)
				m_mainLoopThread->join();
				m_mainLoopThread.reset();
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
			m_overlayManager.Clear();
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

			if ( m_pDefaultBlendState )
			{
				PostEvent( MakeInitialiseEvent( *m_pDefaultBlendState ) );
			}

			if ( m_pDefaultSampler )
			{
				PostEvent( MakeInitialiseEvent( *m_pDefaultSampler ) );
			}

			if ( m_pLightsSampler )
			{
				PostEvent( MakeInitialiseEvent( *m_pLightsSampler ) );
			}
		}
	}

	ContextSPtr Engine::CreateContext( RenderWindow * p_pRenderWindow )
	{
		ContextSPtr l_pReturn;

		if ( !m_mainLoopThread )
		{
			l_pReturn = m_renderSystem->GetMainContext();

			if ( !l_pReturn )
			{
				l_pReturn = m_renderSystem->CreateContext();

				if ( l_pReturn->Initialise( p_pRenderWindow ) )
				{
					m_renderSystem->SetMainContext( l_pReturn );
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

				l_pReturn = m_renderSystem->GetMainContext();
			}
		}

		return l_pReturn;
	}

	void Engine::StartRendering()
	{
		CASTOR_MUTEX_SCOPED_LOCK( m_mutexMainLoop );
		m_bEnded = false;

		if ( !m_mainLoopThread )
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

		if ( !m_mainLoopThread )
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
		if ( m_mainLoopThread )
		{
			CASTOR_ASSERT( false );
			CASTOR_EXCEPTION( "Can't call RenderOneFrame in threaded mode" );
		}
		else
		{
			DoRenderOneFrame();
		}
	}

	SceneSPtr Engine::CreateScene( String const & p_name )
	{
		SceneSPtr l_pReturn = m_sceneManager.find( p_name );

		if ( ! l_pReturn )
		{
			l_pReturn = std::make_shared< Scene >( *this, m_lightFactory, p_name );
			l_pReturn->Initialise();
			m_sceneManager.insert( p_name, l_pReturn );
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

	MeshSPtr Engine::CreateMesh( eMESH_TYPE p_type, Castor::String const & p_strMeshName )
	{
		return CreateMesh( p_type, p_strMeshName, UIntArray(), RealArray() );
	}

	MeshSPtr Engine::CreateMesh( eMESH_TYPE p_type, Castor::String const & p_strMeshName, UIntArray const & p_arrayFaces )
	{
		return CreateMesh( p_type, p_strMeshName, p_arrayFaces, RealArray() );
	}

	MeshSPtr Engine::CreateMesh( eMESH_TYPE p_type, Castor::String const & p_strMeshName, UIntArray const & p_arrayFaces, RealArray const & p_arraySizes )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		MeshSPtr l_pReturn = m_meshManager.find( p_strMeshName );

		if ( !l_pReturn )
		{
			l_pReturn = std::make_shared< Mesh >( *this, p_strMeshName );
			l_pReturn->Initialise( *m_meshFactory.Create( p_type ), p_arrayFaces, p_arraySizes );
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
				l_pMesh = std::make_shared< Mesh >( *this, cuEmptyString );
				m_meshManager.insert( cuEmptyString, l_pMesh );
			}

			l_return = Mesh::BinaryParser( p_file.GetFilePath() ).Parse( *l_pMesh, p_file );
		}

		return l_return;
	}

	RenderWindowSPtr Engine::CreateRenderWindow()
	{
		RenderWindowSPtr l_pReturn = m_renderSystem->CreateRenderWindow();
		{
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
			m_mapWindows[l_pReturn->GetIndex()] = l_pReturn;
		}
		return l_pReturn;
	}

	bool Engine::RemoveRenderWindow( uint32_t p_index )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		bool l_return = false;
		RenderWindowMap::iterator l_it = m_mapWindows.find( p_index );

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

	bool Engine::LoadRenderer( eRENDERER_TYPE p_type )
	{
		bool l_return = false;
		m_mutexRenderers.lock();
		RendererPluginSPtr l_pPlugin = m_arrayRenderers[p_type];
		m_mutexRenderers.unlock();

		if ( l_pPlugin )
		{
			m_renderSystem = l_pPlugin->CreateRenderSystem( this );
			m_shaderManager.SetRenderSystem( m_renderSystem );
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
		FrameListenerSPtr l_listener = m_defaultListener.lock();

		if ( l_listener )
		{
			l_listener->PostEvent( p_pEvent );
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
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( cuT( "Error encountered while loading file [" ) + p_pathFile + cuT( "]" ) ), true );
				}

				PluginBase::PGetTypeFunction l_pfnGetType;

				if ( !l_pLibrary->GetFunction( l_pfnGetType, GetTypeFunctionABIName ) )
				{
					String l_strError = cuT( "Error encountered while loading file [" ) + p_pathFile.GetFileName() + cuT( "] GetType plugin function => Not a Castor3D plugin" );
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
				}

				ePLUGIN_TYPE l_type = l_pfnGetType();

				switch ( l_type )
				{
				case ePLUGIN_TYPE_DIVIDER:
					l_pReturn = std::make_shared< DividerPlugin >( l_pLibrary, this );
					break;

				case ePLUGIN_TYPE_IMPORTER:
					l_pReturn = std::make_shared< ImporterPlugin >( l_pLibrary, this );
					break;

				case ePLUGIN_TYPE_RENDERER:
					l_pReturn = LoadRendererPlugin( l_pLibrary );
					break;

				case ePLUGIN_TYPE_GENERIC:
					l_pReturn = std::make_shared< GenericPlugin >( l_pLibrary, this );
					break;

				case ePLUGIN_TYPE_TECHNIQUE:
					l_pReturn = LoadTechniquePlugin( l_pLibrary );
					break;

				case ePLUGIN_TYPE_POSTFX:
					l_pReturn = std::make_shared< PostFxPlugin >( l_pLibrary, this );
					break;

				default:
					break;
				}

				if ( l_pReturn )
				{
					Version l_toCheck( 0, 0 );
					l_pReturn->GetRequiredVersion( l_toCheck );
					String l_strToLog( cuT( "LoadPlugin - Plugin [" ) );
					Logger::LogInfo( StringStream() << l_strToLog << l_pReturn->GetName() << cuT( "] - Required engine version : " ) << l_toCheck );

					if ( l_toCheck <= m_version )
					{
						m_mutexLoadedPluginTypes.lock();
						m_mapLoadedPluginTypes.insert( std::make_pair( p_pathFile, l_type ) );
						m_mutexLoadedPluginTypes.unlock();
						m_mutexLoadedPlugins.lock();
						m_arrayLoadedPlugins[l_type].insert( std::make_pair( p_pathFile, l_pReturn ) );
						m_mutexLoadedPlugins.unlock();
						m_mutexLibraries.lock();
						m_librariesMap[l_type].insert( std::make_pair( p_pathFile, l_pLibrary ) );
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
			ePLUGIN_TYPE l_type = l_it->second;
			m_mutexLoadedPluginTypes.unlock();
			m_mutexLoadedPlugins.lock();
			l_pReturn = m_arrayLoadedPlugins[l_type].find( p_pathFile )->second;
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

	RenderTechniqueBaseSPtr Engine::CreateTechnique( Castor::String const & p_name, RenderTarget & p_renderTarget, Parameters const & p_params )
	{
		return m_techniqueFactory.Create( p_name, p_renderTarget, m_renderSystem, p_params );
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

		if ( m_renderSystem )
		{
			l_return = m_renderSystem->CheckSupport( p_eShaderModel );
		}

		return l_return;
	}

	bool Engine::SupportsDepthBuffer()const
	{
		bool l_return = false;

		if ( m_renderSystem )
		{
			l_return = m_renderSystem->SupportsDepthBuffer();
		}

		return l_return;
	}

	RenderTargetSPtr Engine::CreateRenderTarget( eTARGET_TYPE p_type )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		RenderTargetSPtr l_pReturn = m_renderSystem->CreateRenderTarget( p_type );
		m_mapRenderTargets.insert( std::make_pair( p_type, l_pReturn ) );
		return l_pReturn;
	}

	void Engine::RemoveRenderTarget( RenderTargetSPtr && p_pRenderTarget )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		eTARGET_TYPE l_type = p_pRenderTarget->GetTargetType();
		RenderTargetMMapIt l_it = m_mapRenderTargets.find( l_type );

		while ( l_it != m_mapRenderTargets.end() )
		{
			if ( l_it->first != l_type )
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

	FrameListenerWPtr Engine::CreateFrameListener( String const & p_name )
	{
		FrameListenerSPtr l_return;
		auto l_it = m_listeners.find( p_name );

		if ( l_it == m_listeners.end() )
		{
			l_it = m_listeners.insert( std::make_pair( p_name, std::make_shared< FrameListener >() ) ).first;
		}

		return l_it->second;
	}

	void Engine::AddFrameListener( String const & p_name, FrameListenerSPtr && p_listener )
	{
		if ( m_listeners.find( p_name ) != m_listeners.end() )
		{
			CASTOR_EXCEPTION( "A listener with this name already exists: " + string::string_cast< char >( p_name ) );
		}

		m_listeners.insert( std::make_pair( p_name, std::move( p_listener ) ) );
	}

	FrameListener & Engine::GetFrameListener( String const & p_name )
	{
		auto l_it = m_listeners.find( p_name );

		if ( l_it == m_listeners.end() )
		{
			CASTOR_EXCEPTION( "No listener with this name: " + string::string_cast< char >( p_name ) );
		}

		return *l_it->second;
	}

	void Engine::DestroyFrameListener( String const & p_name )
	{
		auto l_it = m_listeners.find( p_name );

		if ( l_it != m_listeners.end() )
		{
			m_listeners.erase( l_it );
		}
	}

	SamplerSPtr Engine::CreateSampler( String const & p_name )
	{
		SamplerSPtr l_pReturn;

		if ( p_name.empty() )
		{
			l_pReturn = m_renderSystem->CreateSampler( p_name );
		}
		else
		{
			l_pReturn = m_samplerManager.find( p_name );

			if ( !l_pReturn )
			{
				l_pReturn = m_renderSystem->CreateSampler( p_name );
				m_samplerManager.insert( p_name, l_pReturn );
				PostEvent( MakeInitialiseEvent( *l_pReturn ) );
			}
		}

		return l_pReturn;
	}

	DepthStencilStateSPtr Engine::CreateDepthStencilState( String const & p_name )
	{
		DepthStencilStateSPtr l_pReturn = m_depthStencilStateManager.find( p_name );

		if ( m_renderSystem && !l_pReturn )
		{
			l_pReturn = m_renderSystem->CreateDepthStencilState();
			m_depthStencilStateManager.insert( p_name, l_pReturn );
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
			PostEvent( MakeInitialiseEvent( *l_pReturn ) );
		}

		return l_pReturn;
	}

	RasteriserStateSPtr Engine::CreateRasteriserState( String const & p_name )
	{
		RasteriserStateSPtr l_pReturn = m_rasteriserStateManager.find( p_name );

		if ( m_renderSystem && !l_pReturn )
		{
			l_pReturn = m_renderSystem->CreateRasteriserState();
			m_rasteriserStateManager.insert( p_name, l_pReturn );
			PostEvent( MakeInitialiseEvent( *l_pReturn ) );
		}

		return l_pReturn;
	}

	BlendStateSPtr Engine::CreateBlendState( String const & p_name )
	{
		BlendStateSPtr l_pReturn = m_blendStateManager.find( p_name );

		if ( m_renderSystem && !l_pReturn )
		{
			l_pReturn = m_renderSystem->CreateBlendState();
			m_blendStateManager.insert( p_name, l_pReturn );
			PostEvent( MakeInitialiseEvent( *l_pReturn ) );
		}

		return l_pReturn;
	}

	void Engine::ShowDebugOverlays( bool p_show )
	{
		m_debugOverlays->Show( p_show );
	}

	void Engine::RegisterParsers( Castor::String const & p_name, Castor::FileParser::AttributeParsersBySection && p_parsers )
	{
		auto && l_it = m_additionalParsers.find( p_name );

		if ( l_it != m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "RegisterParsers - Duplicate entry for " + p_name );
		}

		m_additionalParsers.insert( std::make_pair( p_name, p_parsers ) );
	}

	void Engine::UnregisterParsers( Castor::String const & p_name )
	{
		auto && l_it = m_additionalParsers.find( p_name );

		if ( l_it == m_additionalParsers.end() )
		{
			CASTOR_EXCEPTION( "UnregisterParsers - Unregistered entry " + p_name );
		}

		m_additionalParsers.erase( l_it );
	}

	void Engine::DoPreRender()
	{
		PreciseTimer l_timer;
		m_renderSystem->GetMainContext()->SetCurrent();
		m_debugOverlays->EndGpuTask();

		for ( auto && l_listener : m_listeners )
		{
			l_listener.second->FireEvents( eEVENT_TYPE_PRE_RENDER );
		}

		UpdateOverlayManager();
		UpdateShaderManager();

		m_debugOverlays->EndCpuTask();
		m_renderSystem->GetMainContext()->EndCurrent();
	}

	void Engine::DoRender( bool p_bForce, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount )
	{
		m_renderSystem->GetMainContext()->SetCurrent();

		if ( m_renderSystem->GetRendererType() != eRENDERER_TYPE_DIRECT3D )
		{
			// Reverse iterator because we want to render textures before windows
			for ( auto l_rit = m_mapRenderTargets.rbegin(); l_rit != m_mapRenderTargets.rend(); ++l_rit )
			{
				p_objCount += l_rit->second->GetScene()->GetGeometriesCount();
				p_fceCount += l_rit->second->GetScene()->GetFaceCount();
				p_vtxCount += l_rit->second->GetScene()->GetVertexCount();
				l_rit->second->Render( m_dFrameTime );
			}

			m_debugOverlays->EndGpuTask();
		}

		for ( auto && l_listener : m_listeners )
		{
			l_listener.second->FireEvents( eEVENT_TYPE_QUEUE_RENDER );
		}

		m_debugOverlays->EndCpuTask();
		m_renderSystem->GetMainContext()->EndCurrent();

		for ( auto && l_it : m_mapWindows )
		{
			l_it.second->RenderOneFrame( p_bForce );
		}

		m_debugOverlays->EndGpuTask();
	}

	void Engine::DoPostRender()
	{
		m_debugOverlays->EndGpuTask();

		for ( auto && l_listener : m_listeners )
		{
			l_listener.second->FireEvents( eEVENT_TYPE_POST_RENDER );
		}

		m_debugOverlays->EndCpuTask();
	}

	void Engine::DoUpdate( bool p_bForce )
	{
		if ( m_renderSystem->GetMainContext() )
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
				l_pContext = m_renderSystem->CreateContext();
				l_pContext->Initialise( m_pMainWindow );
				m_renderSystem->SetMainContext( l_pContext );
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
			SceneFileParser l_parser( *this );

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

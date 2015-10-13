#include "Engine.hpp"

#include "BlendState.hpp"
#include "CleanupEvent.hpp"
#include "DebugOverlays.hpp"
#include "DepthStencilState.hpp"
#include "FrameListener.hpp"
#include "InitialiseEvent.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "Overlay.hpp"
#include "Pipeline.hpp"
#include "PostFxPlugin.hpp"
#include "PluginManager.hpp"
#include "RasteriserState.hpp"
#include "RendererPlugin.hpp"
#include "RenderSystem.hpp"
#include "RenderTarget.hpp"
#include "RenderTechnique.hpp"
#include "Sampler.hpp"
#include "Scene.hpp"
#include "SceneFileParser.hpp"
#include "ShaderProgram.hpp"
#include "TechniquePlugin.hpp"
#include "TextOverlay.hpp"
#include "VersionException.hpp"
#include "WindowManager.hpp"

#include <Logger.hpp>
#include <Factory.hpp>
#include <File.hpp>
#include <Utils.hpp>
#include <DynamicLibrary.hpp>
#include <PreciseTimer.hpp>
#include <Templates.hpp>

using namespace Castor;

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
		, m_pMainWindow( NULL )
		, m_bDefaultInitialised( false )
		, m_overlayManager( *this )
		, m_debugOverlays( std::make_unique< DebugOverlays >() )
	{
		m_materialManager = std::make_unique< MaterialManager >( *this );
		m_windowManager = std::make_unique< WindowManager >( *this );
		m_meshManager = std::make_unique < MeshManager >( *this );
		m_pluginManager = std::make_unique< PluginManager >( *this );
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
		m_meshManager->Clear();
		m_overlayManager.Clear();
		m_fontManager.clear();
		m_imageManager.clear();
		m_sceneManager.clear();
		m_materialManager->Clear();
		m_windowManager->Clear();
		m_listeners.clear();

		// Destroy the RenderSystem
		if ( m_renderSystem )
		{
			RendererPluginSPtr l_plugin = m_pluginManager->GetRenderersList()[m_renderSystem->GetRendererType()];

			if ( l_plugin )
			{
				l_plugin->DestroyRenderSystem( m_renderSystem );
				m_renderSystem = NULL;
			}
			else
			{
				Logger::LogError( cuT( "RenderSystem still exists while it's plugin doesn't exist anymore" ) );
			}
		}

		m_pluginManager->Clear();
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
			m_meshManager->Cleanup();
			m_overlayManager.Cleanup();
			m_materialManager->Cleanup();
			m_windowManager->Cleanup();

			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );

				for ( auto && l_it : m_shaderManager )
				{
					PostEvent( MakeCleanupEvent( *l_it ) );
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
			m_materialManager->Clear();
			m_sceneManager.clear();
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
				m_blendStateManager.clear();
			}
			m_animationManager.clear();
			m_fontManager.clear();
			m_imageManager.clear();
			m_shaderManager.Clear();
			m_depthStencilStateManager.clear();
			m_rasteriserStateManager.clear();
			m_blendStateManager.clear();
			m_windowManager->Clear();

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

	bool Engine::LoadRenderer( eRENDERER_TYPE p_type )
	{
		bool l_return = false;
		m_renderSystem = m_pluginManager->LoadRenderer( p_type );

		if ( m_renderSystem )
		{
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

	void Engine::DoRender( bool p_force, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount )
	{
		m_renderSystem->GetMainContext()->SetCurrent();

		// Reverse iterator because we want to render textures before windows
		for ( auto l_rit = m_mapRenderTargets.rbegin(); l_rit != m_mapRenderTargets.rend(); ++l_rit )
		{
			p_objCount += l_rit->second->GetScene()->GetGeometriesCount();
			p_fceCount += l_rit->second->GetScene()->GetFaceCount();
			p_vtxCount += l_rit->second->GetScene()->GetVertexCount();
			l_rit->second->Render( m_dFrameTime );
		}

		m_debugOverlays->EndGpuTask();

		for ( auto && l_listener : m_listeners )
		{
			l_listener.second->FireEvents( eEVENT_TYPE_QUEUE_RENDER );
		}

		m_debugOverlays->EndCpuTask();
		m_renderSystem->GetMainContext()->EndCurrent();
		m_windowManager->Render( p_force );
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
		DoUpdate( true );
	}

	void Engine::DoRenderFlushFrame()
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( m_mutexResources );
		DoUpdate( true );
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

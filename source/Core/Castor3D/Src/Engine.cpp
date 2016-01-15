#include "Engine.hpp"

#include "AnimationManager.hpp"
#include "BlendStateManager.hpp"
#include "CleanupEvent.hpp"
#include "DebugOverlays.hpp"
#include "DepthStencilStateManager.hpp"
#include "InitialiseEvent.hpp"
#include "ListenerManager.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "OverlayManager.hpp"
#include "Pipeline.hpp"
#include "PostFxPlugin.hpp"
#include "PluginManager.hpp"
#include "RasteriserStateManager.hpp"
#include "RenderLoopAsync.hpp"
#include "RenderLoopSync.hpp"
#include "RendererPlugin.hpp"
#include "RenderSystem.hpp"
#include "RenderTechnique.hpp"
#include "SamplerManager.hpp"
#include "SceneManager.hpp"
#include "SceneFileParser.hpp"
#include "ShaderManager.hpp"
#include "TargetManager.hpp"
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
		: Unique< Engine >( this )
		, m_renderSystem( NULL )
		, m_cleaned( true )
		, m_perObjectLighting( true )
	{
		std::locale::global( std::locale() );

		m_animationManager = std::make_unique< AnimationManager >( *this );
		m_shaderManager = std::make_unique< ShaderManager >( *this );
		m_samplerManager = std::make_unique< SamplerManager >( *this );
		m_depthStencilStateManager = std::make_unique< DepthStencilStateManager >( *this );
		m_rasteriserStateManager = std::make_unique< RasteriserStateManager >( *this );
		m_blendStateManager = std::make_unique< BlendStateManager >( *this );
		m_materialManager = std::make_unique< MaterialManager >( *this );
		m_windowManager = std::make_unique< WindowManager >( *this );
		m_meshManager = std::make_unique < MeshManager >( *this );
		m_pluginManager = std::make_unique< PluginManager >( *this );
		m_overlayManager = std::make_unique< OverlayManager >( *this );
		m_sceneManager = std::make_unique< SceneManager >( *this );
		m_targetManager = std::make_unique< TargetManager >( *this );
		m_listenerManager = std::make_unique< ListenerManager >( *this );

		if ( !File::DirectoryExists( GetEngineDirectory() ) )
		{
			File::DirectoryCreate( GetEngineDirectory() );
		}

		Version l_version;
		String l_strVersion;
		Logger::LogInfo( StringStream() << cuT( "Castor3D - Core engine version : " ) << l_version );
	}

	Engine::~Engine()
	{
		m_defaultBlendState.reset();
		m_lightsSampler.reset();
		m_defaultSampler.reset();

		// To destroy before RenderSystem, since it contain elements instantiated in Renderer plug-in
		m_samplerManager->Clear();
		m_shaderManager->Clear();
		m_depthStencilStateManager->Clear();
		m_rasteriserStateManager->Clear();
		m_blendStateManager->Clear();
		m_animationManager->Clear();
		m_meshManager->Clear();
		m_overlayManager->Clear();
		m_fontManager.clear();
		m_imageManager.clear();
		m_sceneManager->Clear();
		m_materialManager->Clear();
		m_windowManager->Clear();
		m_listenerManager->Clear();

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
	}

	void Engine::Initialise( uint32_t p_wanted, bool p_threaded )
	{
		if ( m_renderSystem )
		{
			m_targetManager->SetRenderSystem( m_renderSystem );
			m_samplerManager->SetRenderSystem( m_renderSystem );
			m_shaderManager->SetRenderSystem( m_renderSystem );
			m_overlayManager->SetRenderSystem( m_renderSystem );
			m_materialManager->SetRenderSystem( m_renderSystem );
			m_sceneManager->SetRenderSystem( m_renderSystem );
			m_blendStateManager->SetRenderSystem( m_renderSystem );
			m_animationManager->SetRenderSystem( m_renderSystem );
			m_shaderManager->SetRenderSystem( m_renderSystem );
			m_depthStencilStateManager->SetRenderSystem( m_renderSystem );
			m_rasteriserStateManager->SetRenderSystem( m_renderSystem );
			m_blendStateManager->SetRenderSystem( m_renderSystem );
			m_windowManager->SetRenderSystem( m_renderSystem );

			m_defaultBlendState = m_blendStateManager->Create( cuT( "Default" ) );
			m_defaultSampler = m_samplerManager->Create( cuT( "Default" ) );
			m_defaultSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
			m_defaultSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
			m_defaultSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIP, eINTERPOLATION_MODE_LINEAR );
			m_lightsSampler = m_samplerManager->Create( cuT( "LightsSampler" ) );
			m_lightsSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_NEAREST );
			m_lightsSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_NEAREST );

			DoLoadCoreData();
		}

		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( C3D_NO_RENDERSYSTEM );
		}

		if ( m_defaultBlendState )
		{
			m_listenerManager->PostEvent( MakeInitialiseEvent( *m_defaultBlendState ) );
		}

		if ( m_lightsSampler )
		{
			m_listenerManager->PostEvent( MakeInitialiseEvent( *m_lightsSampler ) );
		}

		if ( m_defaultSampler )
		{
			m_listenerManager->PostEvent( MakeInitialiseEvent( *m_defaultSampler ) );
		}

		if ( p_threaded )
		{
			m_renderLoop = std::make_unique< RenderLoopAsync >( *this, m_renderSystem, p_wanted );
		}
		else
		{
			m_renderLoop = std::make_unique< RenderLoopSync >( *this, m_renderSystem, p_wanted );
		}

		m_cleaned = false;
	}

	void Engine::Cleanup()
	{
		if ( !IsCleaned() )
		{
			SetCleaned();
			m_listenerManager->Cleanup();
			m_sceneManager->Cleanup();
			m_depthStencilStateManager->Cleanup();
			m_rasteriserStateManager->Cleanup();
			m_blendStateManager->Cleanup();
			m_samplerManager->Cleanup();
			m_meshManager->Cleanup();
			m_overlayManager->Cleanup();
			m_materialManager->Cleanup();
			m_shaderManager->Cleanup();

			if ( m_defaultBlendState )
			{
				m_listenerManager->PostEvent( MakeCleanupEvent( *m_defaultBlendState ) );
			}

			if ( m_lightsSampler )
			{
				m_listenerManager->PostEvent( MakeCleanupEvent( *m_lightsSampler ) );
			}

			if ( m_defaultSampler )
			{
				m_listenerManager->PostEvent( MakeCleanupEvent( *m_defaultSampler ) );
			}

			m_windowManager->Cleanup();
			m_renderLoop.reset();
			m_renderSystem->Cleanup();

			m_targetManager->Clear();
			m_samplerManager->Clear();
			m_meshManager->Clear();
			m_shaderManager->Clear();
			m_overlayManager->Clear();
			m_materialManager->Clear();
			m_sceneManager->Clear();
			m_blendStateManager->Clear();
			m_animationManager->Clear();
			m_fontManager.clear();
			m_imageManager.clear();
			m_shaderManager->Clear();
			m_depthStencilStateManager->Clear();
			m_rasteriserStateManager->Clear();
			m_blendStateManager->Clear();
			m_windowManager->Clear();
		}
	}

	bool Engine::LoadRenderer( eRENDERER_TYPE p_type )
	{
		bool l_return = false;
		m_renderSystem = m_pluginManager->LoadRenderer( p_type );

		if ( m_renderSystem )
		{
			l_return = true;
		}

		return l_return;
	}

	void Engine::PostEvent( FrameEventSPtr p_event )
	{
		m_listenerManager->PostEvent( p_event );
	}

	Path Engine::GetPluginsDirectory()
	{
		Path l_pathReturn;
		Path l_pathBin = File::GetWorkingDirectory();
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
		Path l_pathBin = File::GetWorkingDirectory();
		Path l_pathUsr = l_pathBin.GetPath();
		l_pathReturn = l_pathUsr / cuT( "share" );
		return l_pathReturn;
	}

	RenderTechniqueBaseSPtr Engine::CreateTechnique( Castor::String const & p_name, RenderTarget & p_renderTarget, Parameters const & p_params )
	{
		return m_techniqueFactory.Create( p_name, p_renderTarget, m_renderSystem, p_params );
	}

	bool Engine::IsCleaned()
	{
		return m_cleaned;
	}

	void Engine::SetCleaned()
	{
		m_cleaned = true;
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
		}
	}
}

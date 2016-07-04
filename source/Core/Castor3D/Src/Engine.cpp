#include "Engine.hpp"

#include "BlendStateCache.hpp"
#include "DepthStencilStateCache.hpp"
#include "ListenerCache.hpp"
#include "MaterialCache.hpp"
#include "MeshCache.hpp"
#include "OverlayCache.hpp"
#include "PluginCache.hpp"
#include "RasteriserStateCache.hpp"
#include "SamplerCache.hpp"
#include "SceneCache.hpp"
#include "ShaderCache.hpp"
#include "TargetCache.hpp"
#include "TechniqueCache.hpp"
#include "WindowCache.hpp"

#include "Event/Frame/CleanupEvent.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Overlay/DebugOverlays.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Miscellaneous/VersionException.hpp"
#include "Plugin/PostFxPlugin.hpp"
#include "Plugin/RendererPlugin.hpp"
#include "Plugin/TechniquePlugin.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderLoopAsync.hpp"
#include "Render/RenderLoopSync.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/SceneFileParser.hpp"

#include <DynamicLibrary.hpp>
#include <Factory.hpp>
#include <File.hpp>
#include <Image.hpp>
#include <Logger.hpp>
#include <PreciseTimer.hpp>
#include <Templates.hpp>
#include <UniqueObjectPool.hpp>
#include <Utils.hpp>

using namespace Castor;

//*************************************************************************************************

namespace Castor3D
{
	static const char * C3D_NO_RENDERSYSTEM = "No RenderSystem loaded, call Castor3D::Engine::LoadRenderer before Castor3D::Engine::Initialise";
	static const char * C3D_MAIN_LOOP_EXISTS = "Render loop is already started";

	Engine::Engine()
		: Unique< Engine >( this )
		, m_renderSystem( nullptr )
		, m_cleaned( true )
		, m_perObjectLighting( true )
		, m_threaded( false )
	{
		std::locale::global( std::locale() );
		Image::InitialiseImageLib();

		// m_listenerCache *MUST* be the first created.
		m_listenerCache = MakeCache< FrameListener, String >( EngineGetter{ *this }, ListenerProducer{} );

		m_shaderCache = MakeCache( *this );
		m_samplerCache = MakeCache< Sampler, String >( EngineGetter{ *this }, SamplerProducer{ *this } );
		m_depthStencilStateCache = MakeCache< DepthStencilState, String >( EngineGetter{ *this }, DepthStencilStateProducer{ *this } );
		m_rasteriserStateCache = MakeCache< RasteriserState, String >( EngineGetter{ *this }, RasteriserStateProducer{ *this } );
		m_blendStateCache = MakeCache< BlendState, String >( EngineGetter{ *this }, BlendStateProducer{ *this } );
		m_materialCache = MakeCache( EngineGetter{ *this }, MaterialProducer{} );
		m_pluginCache = MakeCache( EngineGetter{ *this }, PluginProducer{} );
		m_overlayCache = MakeCache( EngineGetter{ *this }, OverlayProducer{ *this } );
		m_sceneCache = MakeCache< Scene, String >( EngineGetter{ *this }, SceneProducer{} );
		m_targetCache = std::make_unique< TargetCache >( *this );
		m_techniqueCache = MakeCache( EngineGetter{ *this }, RenderTechniqueProducer{} );

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
		m_samplerCache->Clear();
		m_shaderCache->Clear();
		m_depthStencilStateCache->Clear();
		m_rasteriserStateCache->Clear();
		m_blendStateCache->Clear();
		m_overlayCache->Clear();
		m_fontCache.Clear();
		m_imageCache.clear();
		m_sceneCache->Clear();
		m_materialCache->Clear();
		m_listenerCache->Clear();
		m_techniqueCache->Clear();

		// Destroy the RenderSystem
		if ( m_renderSystem )
		{
			auto const & l_renderers = m_pluginCache->GetRenderersList();
			auto l_it = l_renderers.find( m_renderSystem->GetRendererType() );

			if ( l_it != l_renderers.end() )
			{
				l_it->second->DestroyRenderSystem( m_renderSystem );
				m_renderSystem = nullptr;
			}
			else
			{
				Logger::LogError( cuT( "RenderSystem still exists while it's plug-in doesn't exist anymore" ) );
			}
		}

		m_pluginCache->Clear();
		Image::CleanupImageLib();
	}

	void Engine::Initialise( uint32_t p_wanted, bool p_threaded )
	{
		m_threaded = p_threaded;

		if ( m_renderSystem )
		{
			m_targetCache->SetRenderSystem( m_renderSystem );
			m_samplerCache->SetRenderSystem( m_renderSystem );
			m_shaderCache->SetRenderSystem( m_renderSystem );
			m_overlayCache->SetRenderSystem( m_renderSystem );
			m_materialCache->SetRenderSystem( m_renderSystem );
			m_sceneCache->SetRenderSystem( m_renderSystem );
			m_blendStateCache->SetRenderSystem( m_renderSystem );
			m_shaderCache->SetRenderSystem( m_renderSystem );
			m_depthStencilStateCache->SetRenderSystem( m_renderSystem );
			m_rasteriserStateCache->SetRenderSystem( m_renderSystem );
			m_blendStateCache->SetRenderSystem( m_renderSystem );
			m_techniqueCache->SetRenderSystem( m_renderSystem );

			m_defaultBlendState = m_blendStateCache->Add( cuT( "Default" ) );
			m_defaultSampler = m_samplerCache->Add( cuT( "Default" ) );
			m_defaultSampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
			m_defaultSampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
			m_defaultSampler->SetInterpolationMode( InterpolationFilter::Mip, InterpolationMode::Linear );
			m_lightsSampler = m_samplerCache->Add( cuT( "LightsSampler" ) );
			m_lightsSampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Nearest );
			m_lightsSampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Nearest );

			DoLoadCoreData();
		}

		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( C3D_NO_RENDERSYSTEM );
		}

		if ( m_defaultBlendState )
		{
			m_listenerCache->PostEvent( MakeInitialiseEvent( *m_defaultBlendState ) );
		}

		if ( m_lightsSampler )
		{
			m_listenerCache->PostEvent( MakeInitialiseEvent( *m_lightsSampler ) );
		}

		if ( m_defaultSampler )
		{
			m_listenerCache->PostEvent( MakeInitialiseEvent( *m_defaultSampler ) );
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

			if ( m_threaded )
			{
				m_renderLoop->Pause();
			}

			m_listenerCache->Cleanup();
			m_sceneCache->Cleanup();
			m_depthStencilStateCache->Cleanup();
			m_rasteriserStateCache->Cleanup();
			m_blendStateCache->Cleanup();
			m_samplerCache->Cleanup();
			m_overlayCache->Cleanup();
			m_materialCache->Cleanup();
			m_shaderCache->Cleanup();

			if ( m_defaultBlendState )
			{
				m_listenerCache->PostEvent( MakeCleanupEvent( *m_defaultBlendState ) );
			}

			if ( m_lightsSampler )
			{
				m_listenerCache->PostEvent( MakeCleanupEvent( *m_lightsSampler ) );
			}

			if ( m_defaultSampler )
			{
				m_listenerCache->PostEvent( MakeCleanupEvent( *m_defaultSampler ) );
			}

			m_techniqueCache->Cleanup();
			m_renderLoop.reset();

			m_targetCache->Clear();
			m_samplerCache->Clear();
			m_shaderCache->Clear();
			m_overlayCache->Clear();
			m_materialCache->Clear();
			m_sceneCache->Clear();
			m_blendStateCache->Clear();
			m_fontCache.Clear();
			m_imageCache.clear();
			m_shaderCache->Clear();
			m_depthStencilStateCache->Clear();
			m_rasteriserStateCache->Clear();
			m_blendStateCache->Clear();
			m_techniqueCache->Clear();
		}
	}

	bool Engine::LoadRenderer( String const & p_type )
	{
		bool l_return = false;
		m_renderSystem = m_pluginCache->LoadRenderer( p_type );

		if ( m_renderSystem )
		{
			l_return = true;
		}

		return l_return;
	}

	void Engine::PostEvent( FrameEventSPtr p_event )
	{
		m_listenerCache->PostEvent( p_event );
	}

	Path Engine::GetPluginsDirectory()
	{
		Path l_pathReturn;
		Path l_pathBin = File::GetExecutableDirectory();
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
		Path l_pathBin = File::GetExecutableDirectory();
		Path l_pathUsr = l_pathBin.GetPath();
		l_pathReturn = l_pathUsr / cuT( "share" );
		return l_pathReturn;
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
			l_return = m_renderSystem->GetGpuInformations().CheckSupport( p_eShaderModel );
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

	void Engine::RegisterSections( Castor::String const & p_name, Castor::StrUIntMap const & p_sections )
	{
		auto && l_it = m_additionalSections.find( p_name );

		if ( l_it != m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "RegisterSections - Duplicate entry for " + p_name );
		}

		m_additionalSections.insert( std::make_pair( p_name, p_sections ) );
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

	void Engine::UnregisterSections( Castor::String const & p_name )
	{
		auto && l_it = m_additionalSections.find( p_name );

		if ( l_it == m_additionalSections.end() )
		{
			CASTOR_EXCEPTION( "UnregisterSections - Unregistered entry " + p_name );
		}

		m_additionalSections.erase( l_it );
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

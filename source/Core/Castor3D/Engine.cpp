#include "Castor3D/Engine.hpp"

#include "Castor3D/Cache/ListenerCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Cache/SceneCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/TechniqueCache.hpp"
#include "Castor3D/Cache/WindowCache.hpp"
#include "Castor3D/Event/Frame/CleanupEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/ImporterFactory.hpp"
#include "Castor3D/Model/Mesh/MeshFactory.hpp"
#include "Castor3D/Overlay/DebugOverlays.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Plugin/Plugin.hpp"
#include "Castor3D/Render/RenderLoopAsync.hpp"
#include "Castor3D/Render/RenderLoopSync.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/ExrImageLoader.hpp>
#include <CastorUtils/Graphics/FreeImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageWriter.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Graphics/XpmImageLoader.hpp>
#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>
#include <CastorUtils/Pool/UniqueObjectPool.hpp>

#include "Castor3D/Shader/GlslToSpv.hpp"

#include <string_view>

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	namespace
	{
		static constexpr bool C3D_EnableAPITrace = false;
		static const char * C3D_NO_RENDERSYSTEM = "No RenderSystem loaded, call castor3d::Engine::loadRenderer before castor3d::Engine::Initialise";
		static const char * C3D_MAIN_LOOP_EXISTS = "Render loop is already started";

		castor::LoggerInstancePtr createLogger( castor::LogType type
			, castor::Path const & filePath )
		{
			auto result = castor::Logger::createInstance( type );
			result->setFileName( filePath );
			return result;
		}
	}

	Engine::Engine( castor::String const & appName
		, Version const & appVersion
		, bool enableValidation
		, castor::LoggerInstancePtr ownedLogger
		, castor::LoggerInstance * logger )
		: Unique< Engine >( this )
		, m_ownedLogger{ std::move( ownedLogger ) }
		, m_logger{ log::initialise( *( logger ? logger : m_ownedLogger.get() ) ) }
		, m_appName{ appName }
		, m_appVersion{ appVersion }
		, m_enableValidation{ enableValidation }
		, m_fontCache{ *m_logger }
		, m_imageCache{ *m_logger, m_imageLoader }
		, m_meshFactory{ std::make_shared< MeshFactory >() }
		, m_subdividerFactory{ std::make_shared< MeshSubdividerFactory >() }
		, m_importerFactory{ std::make_shared< MeshImporterFactory >() }
		, m_particleFactory{ std::make_shared< ParticleFactory >() }
		, m_enableApiTrace{ C3D_EnableAPITrace }
		, m_jobs{ castor::CpuInformations{}.getCoreCount() }
	{
		auto dummy = []( auto element )
		{
		};
		auto jobInit = [this]( auto element )
		{
			this->pushGpuJob( [element]( RenderDevice const & device )
				{
					element->initialise( device );
				} );
		};
		auto gpuEventInit = [this]( auto element )
		{
			this->postEvent( makeGpuInitialiseEvent( *element ) );
		};
		auto cpuEvtClean = [this]( auto element )
		{
			this->postEvent( makeCpuCleanupEvent( *element ) );
		};
		auto instantInit = [this]( auto element )
		{
			element->initialise();
		};
		auto instantClean = [this]( auto element )
		{
			element->cleanup();
		};
		auto listenerClean = [this]( auto element )
		{
			element->flush();
		};
		auto mergeResource = []( auto const & source
			, auto & destination
			, auto element )
		{
		};
		initialiseGlslang();
		GliImageLoader::registerLoader( m_imageLoader );
		StbImageLoader::registerLoader( m_imageLoader );
		ExrImageLoader::registerLoader( m_imageLoader );
		XpmImageLoader::registerLoader( m_imageLoader );
		FreeImageLoader::registerLoader( m_imageLoader );
		StbImageWriter::registerWriter( m_imageWriter );
		GliImageWriter::registerWriter( m_imageWriter );

		// m_listenerCache *MUST* be the first created.
		m_listenerCache = makeCache< FrameListener, String >( *this
			, []( String const & name )
			{
				return std::make_shared< FrameListener >( name );
			}
			, dummy
			, listenerClean
			, mergeResource );
		m_defaultListener = m_listenerCache->add( cuT( "Default" ) );

		m_shaderCache = makeCache( *this );
		m_samplerCache = makeCache< Sampler, String >( *this
			, [this]( String const & name )
			{
				return std::make_shared< Sampler >( *this, name );
			}
			, gpuEventInit
			, cpuEvtClean
			, mergeResource );
		m_materialCache = std::make_unique< MaterialCache >( *this
			, [this]( String const & name, MaterialType type )
			{
				return std::make_shared< Material >( name, *this, type );
			}
			, gpuEventInit
			, cpuEvtClean
			, mergeResource );
		m_pluginCache = std::make_unique< PluginCache >( *this
			, []( String const & name, PluginType type, castor::DynamicLibrarySPtr library )
			{
				return nullptr;
			}
			, dummy
			, dummy
			, mergeResource );
		m_overlayCache = makeCache< Overlay, String >( *this
			, [this]( String const & name, OverlayType type, SceneSPtr scene, OverlaySPtr parent )
			{
				auto result = std::make_shared< Overlay >( *this, type, scene, parent );
				result->setName( name );
				return result;
			}
			, dummy
			, dummy
			, mergeResource );
		m_sceneCache = makeCache< Scene, String >( *this
			, [this]( castor::String const & name )
			{
				return std::make_shared< Scene >( name, *this );
			}
			, instantInit
			, instantClean
			, mergeResource );
		m_targetCache = std::make_unique< RenderTargetCache >( *this );
		m_techniqueCache = makeCache< RenderTechnique, String >( *this
			, [this]( String const & name
				, String const & type
				, RenderTarget & renderTarget
				, RenderDevice const & device
				, Parameters const & parameters
				, SsaoConfig const & ssaoConfig )
			{
				return std::make_shared< RenderTechnique >( name
					, renderTarget
					, device
					, parameters
					, ssaoConfig );
			}
			, dummy
			, dummy
			, mergeResource );

		if ( !File::directoryExists( getEngineDirectory() ) )
		{
			File::directoryCreate( getEngineDirectory() );
		}

		log::info << cuT( "Castor3D - Core engine version : " ) << Version{} << std::endl;
		log::info << m_cpuInformations << std::endl;
	}

	Engine::Engine( castor::String const & appName
		, Version const & appVersion
		, bool enableValidation )
		: Engine{ appName
			, appVersion
			, enableValidation
			, createLogger( castor::Logger::getLevel(), getEngineDirectory() / cuT( "Castor3D.log" ) )
			, nullptr }
	{
	}
	
	Engine::Engine( castor::String const & appName
		, Version const & appVersion
		, bool enableValidation
		, castor::LoggerInstance & logger )
		: Engine{ appName
			, appVersion
			, enableValidation
			, nullptr
			, &logger }
	{
	}

	Engine::~Engine()
	{
		m_lightsSampler.reset();
		m_defaultSampler.reset();

		// To destroy before RenderSystem, since it contain elements instantiated in Renderer plug-in
		m_samplerCache->clear();
		m_shaderCache->clear();
		m_overlayCache->clear();
		m_fontCache.clear();
		m_imageCache.clear();
		m_sceneCache->clear();
		m_materialCache->clear();
		m_listenerCache->clear();
		m_techniqueCache->clear();

		// Destroy the RenderSystem.
		if ( m_renderSystem )
		{
			m_renderSystem.reset();
		}

		// and eventually the  plug-ins.
		m_pluginCache->clear();
		GliImageLoader::unregisterLoader( m_imageLoader );
		StbImageLoader::unregisterLoader( m_imageLoader );
		ExrImageLoader::unregisterLoader( m_imageLoader );
		XpmImageLoader::unregisterLoader( m_imageLoader );
		FreeImageLoader::unregisterLoader( m_imageLoader );
		StbImageWriter::unregisterWriter( m_imageWriter );
		GliImageWriter::unregisterWriter( m_imageWriter );
		cleanupGlslang();

		m_logger = nullptr;
		log::cleanup();
	}

	void Engine::initialise( uint32_t wanted, bool threaded )
	{
		Debug::initialise();
		m_threaded = threaded;

		if ( m_renderSystem )
		{
			m_defaultSampler = m_samplerCache->add( cuT( "Default" ) );
			m_defaultSampler->setMinFilter( VK_FILTER_LINEAR );
			m_defaultSampler->setMagFilter( VK_FILTER_LINEAR );
			m_defaultSampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_defaultSampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_defaultSampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_REPEAT );

			m_lightsSampler = m_samplerCache->add( cuT( "LightsSampler" ) );
			m_lightsSampler->setMinFilter( VK_FILTER_NEAREST );
			m_lightsSampler->setMagFilter( VK_FILTER_NEAREST );
			m_lightsSampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			m_lightsSampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			m_lightsSampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

			doLoadCoreData();
		}

		if ( !m_renderSystem )
		{
			CU_Exception( C3D_NO_RENDERSYSTEM );
		}

		if ( m_lightsSampler )
		{
			postEvent( makeGpuInitialiseEvent( *m_lightsSampler ) );
		}

		if ( m_defaultSampler )
		{
			postEvent( makeGpuInitialiseEvent( *m_defaultSampler ) );
		}

		if ( threaded )
		{
			m_renderLoop.reset( new RenderLoopAsync{ *this, wanted } );
		}
		else
		{
			m_renderLoop.reset( new RenderLoopSync{ *this, wanted } );
		}

		m_cleaned = false;
	}

	void Engine::cleanup()
	{
		if ( !isCleaned() )
		{
			setCleaned();

			if ( m_threaded
				&& !static_cast< RenderLoopAsync const & >( *m_renderLoop ).isPaused() )
			{
				m_renderLoop->pause();
			}

			m_listenerCache->cleanup();
			m_sceneCache->cleanup();
			m_samplerCache->cleanup();
			m_overlayCache->cleanup();
			m_materialCache->cleanup();

			if ( m_lightsSampler )
			{
				postEvent( makeCpuCleanupEvent( *m_lightsSampler ) );
			}

			if ( m_defaultSampler )
			{
				postEvent( makeCpuCleanupEvent( *m_defaultSampler ) );
			}

			m_techniqueCache->cleanup();

			m_renderLoop.reset();

			m_targetCache->clear();
			m_samplerCache->clear();
			m_shaderCache->clear();
			m_overlayCache->clear();
			m_materialCache->clear();
			m_sceneCache->clear();
			m_fontCache.clear();
			m_imageCache.clear();
			m_shaderCache->clear();
			m_techniqueCache->clear();
		}

		Debug::cleanup();
	}

	bool Engine::loadRenderer( String const & type )
	{
		auto it = m_rendererList.find( type );

		if ( it != m_rendererList.end() )
		{
			m_renderSystem = castor::makeUnique< RenderSystem >( *this
				, *it );
		}

		return m_renderSystem != nullptr;
	}

	void Engine::postEvent( CpuFrameEventUPtr event )
	{
		auto lock( castor::makeUniqueLock( *m_listenerCache ) );
		FrameListenerSPtr listener = m_defaultListener.lock();

		if ( listener )
		{
			listener->postEvent( std::move( event ) );
		}
	}

	void Engine::sendEvent( GpuFrameEventUPtr event )
	{
		if ( m_renderSystem && m_renderSystem->hasCurrentRenderDevice() )
		{
			event->apply( m_renderSystem->getCurrentRenderDevice() );
		}
		else
		{
			postEvent( std::move( event ) );
		}
	}

	void Engine::postEvent( GpuFrameEventUPtr event )
	{
		auto lock( castor::makeUniqueLock( *m_listenerCache ) );
		FrameListenerSPtr listener = m_defaultListener.lock();

		if ( listener )
		{
			listener->postEvent( std::move( event ) );
		}
	}

	bool Engine::fireMouseMove( castor::Position const & position )
	{
		for ( auto & listener : m_windowInputListeners )
		{
			listener.second->fireMouseMove( position );
		}

		auto inputListener = getUserInputListener();
		return inputListener && inputListener->fireMouseMove( position );
	}

	void Engine::update( CpuUpdater & updater )
	{
		for ( auto & window : m_renderWindows )
		{
			window.second->update( updater );
		}

		getSceneCache().forEach( [&updater]( Scene & scene )
			{
				scene.update( updater );
			} );
		getRenderTargetCache().update( updater );
		getRenderTechniqueCache().forEach( [&updater]( RenderTechnique & technique )
			{
				TechniqueQueues techniqueQueues;
				updater.queues = &techniqueQueues.queues;
				technique.update( updater );
				techniqueQueues.shadowMaps = technique.getShadowMaps();
				updater.techniquesQueues.push_back( techniqueQueues );
			} );
	}

	void Engine::update( GpuUpdater & updater )
	{
		getMaterialCache().update( updater );
		getRenderTargetCache().update( updater );
	}

	Path Engine::getPluginsDirectory()
	{
		Path binDir = File::getExecutableDirectory();

		while ( binDir.getFileName() != cuT( "bin" ) )
		{
			binDir = binDir.getPath();
		}

		Path usrDir = binDir.getPath();

#if defined( CU_PlatformWindows )
		static std::basic_string_view< xchar > constexpr pluginsSubdir = cuT( "bin" );
#else
		static std::basic_string_view< xchar > constexpr pluginsSubdir = cuT( "lib" );
#endif
		return usrDir / pluginsSubdir.data() / cuT( "Castor3D" );
	}

	castor::Path Engine::getEngineDirectory()
	{
		return File::getUserDirectory() / cuT( ".Castor3D" );
	}

	Path Engine::getDataDirectory()
	{
		Path pathReturn;
		Path pathBin = File::getExecutableDirectory();

		while ( pathBin.getFileName() != cuT( "bin" ) )
		{
			pathBin = pathBin.getPath();
		}

		Path pathUsr = pathBin.getPath();
		pathReturn = pathUsr / cuT( "share" );
		return pathReturn;
	}

	std::locale const & Engine::getLocale()
	{
		static std::locale const loc{ "C" };
		return loc;
	}

	RgbaColour Engine::getNextRainbowColour()const
	{
		static float currentColourHue{ 0.0f };
		currentColourHue += 0.05f;

		if ( currentColourHue > 1.0f )
		{
			currentColourHue = 0.0f;
		}

		return RgbaColour::fromHSB( currentColourHue, 1.0f, 1.0f );
	}

	uint32_t Engine::registerTimer( RenderPassTimer & timer )
	{
		if ( hasRenderLoop() )
		{
			return m_renderLoop->registerTimer( timer );
		}

		return 0u;
	}

	uint32_t Engine::registerTimer( castor::String const & category
		, crg::FramePassTimer & timer )
	{
		if ( hasRenderLoop() )
		{
			return m_renderLoop->registerTimer( category, timer );
		}

		return 0u;
	}

	void Engine::unregisterTimer( RenderPassTimer & timer )
	{
		if ( hasRenderLoop() )
		{
			m_renderLoop->unregisterTimer( timer );
		}
	}

	void Engine::unregisterTimer( castor::String const & category
		, crg::FramePassTimer & timer )
	{
		if ( hasRenderLoop() )
		{
			m_renderLoop->unregisterTimer( category, timer );
		}
	}

	bool Engine::isCleaned()
	{
		return m_cleaned;
	}

	void Engine::setCleaned()
	{
		m_cleaned = true;
	}

	void Engine::registerWindow( RenderWindow & window )
	{
		auto result = m_renderWindows.emplace( window.getName(), &window ).second;
		CU_Assert( result, "Duplicate window." );
		m_windowInputListeners.emplace( &window
			, std::make_shared< RenderWindow::InputListener >( *this, window ) );
		auto listener = m_windowInputListeners.find( &window )->second;
		log::trace << "Created InputListener [0x" << std::hex << listener.get() << "] - " << window.getName() << std::endl;
	}

	void Engine::unregisterWindow( RenderWindow const & window )
	{
		auto it = m_windowInputListeners.find( &window );

		if ( it != m_windowInputListeners.end() )
		{
			auto listener = m_windowInputListeners.find( &window )->second;
			listener->cleanup();
			auto plistener = listener.get();
			auto pwindow = &window;
			log::trace << "Removing InputListener [0x" << std::hex << plistener << "]" << std::endl;
			listener->getFrameListener().postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this, plistener, pwindow]()
				{
					m_windowInputListeners.erase( pwindow );
					log::trace << "Removed InputListener [0x" << std::hex << plistener << "]" << std::endl;
				} ) );
		}

		m_renderWindows.erase( window.getName() );
	}

	void Engine::registerParsers( castor::String const & name, castor::AttributeParsersBySection const & parsers )
	{
		auto && it = m_additionalParsers.find( name );

		if ( it != m_additionalParsers.end() )
		{
			CU_Exception( "registerParsers - Duplicate entry for " + name );
		}

		m_additionalParsers.emplace( name, parsers );
	}

	void Engine::registerSections( castor::String const & name, castor::StrUInt32Map const & sections )
	{
		auto && it = m_additionalSections.find( name );

		if ( it != m_additionalSections.end() )
		{
			CU_Exception( "registerSections - Duplicate entry for " + name );
		}

		m_additionalSections.emplace( name, sections );
	}

	void Engine::unregisterParsers( castor::String const & name )
	{
		auto && it = m_additionalParsers.find( name );

		if ( it == m_additionalParsers.end() )
		{
			CU_Exception( "unregisterParsers - Unregistered entry " + name );
		}

		m_additionalParsers.erase( it );
	}

	void Engine::unregisterSections( castor::String const & name )
	{
		auto && it = m_additionalSections.find( name );

		if ( it == m_additionalSections.end() )
		{
			CU_Exception( "unregisterSections - Unregistered entry " + name );
		}

		m_additionalSections.erase( it );
	}

	void Engine::prepareTextures( Pass & pass )
	{
		pushJob( [&pass]()
			{
				pass.prepareTextures();
			} );
	}

	void Engine::pushJob( castor::AsyncJobQueue::Job job )
	{
		m_jobs.pushJob( job );
	}

	void Engine::pushGpuJob( std::function< void( RenderDevice const & ) > job )
	{
		pushJob( [this, job]()
			{
				job( *m_renderSystem->getMainRenderDevice() );
			} );
	}

	void Engine::doLoadCoreData()
	{
		Path path = Engine::getDataDirectory() / cuT( "Castor3D" );

		if ( File::fileExists( path / cuT( "Core.zip" ) ) )
		{
			SceneFileParser parser( *this );

			if ( !parser.parseFile( path / cuT( "Core.zip" ) ) )
			{
				log::error << cuT( "Can't read Core.zip data file" ) << std::endl;
			}
		}
	}
}

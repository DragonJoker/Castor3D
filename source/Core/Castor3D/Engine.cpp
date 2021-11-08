#include "Castor3D/Engine.hpp"

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
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
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
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

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static constexpr bool C3D_EnableAPITrace = false;
		static const char * C3D_NO_RENDERSYSTEM = "No RenderSystem loaded, call castor3d::Engine::loadRenderer before castor3d::Engine::Initialise";

		static castor::String const defaultName = cuT( "C3D_Default" );
		static castor::String const samplerName = cuT( "C3D_Lights" );

		castor::LoggerInstancePtr createLogger( castor::LogType type
			, castor::Path const & filePath )
		{
			auto result = castor::Logger::createInstance( type );
			result->setFileName( filePath );
			return result;
		}
	}

	//*********************************************************************************************

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
		, m_fontCache{ *m_logger }
		, m_imageCache{ *m_logger, m_imageLoader }
		, m_meshFactory{ castor::makeUnique< MeshFactory >() }
		, m_subdividerFactory{ castor::makeUnique< MeshSubdividerFactory >() }
		, m_importerFactory{ castor::makeUnique< MeshImporterFactory >() }
		, m_particleFactory{ castor::makeUnique< ParticleFactory >() }
		, m_enableValidation{ enableValidation }
		, m_enableApiTrace{ C3D_EnableAPITrace }
		, m_cpuJobs{ std::min( 4u, castor::CpuInformations{}.getCoreCount() / 2u ) }
		, m_gpuJobs{ std::min( 2u, castor::CpuInformations{}.getCoreCount() / 2u ) }
	{
		m_passFactory = castor::makeUnique< PassFactory >( *this );
		m_passesType = m_passFactory->listRegisteredTypes().begin()->second;

		auto listenerClean = []( auto & element )
		{
			element.flush();
		};
		initialiseGlslang();
		castor::GliImageLoader::registerLoader( m_imageLoader );
		castor::StbImageLoader::registerLoader( m_imageLoader );
		castor::ExrImageLoader::registerLoader( m_imageLoader );
		castor::XpmImageLoader::registerLoader( m_imageLoader );
		castor::FreeImageLoader::registerLoader( m_imageLoader );
		castor::StbImageWriter::registerWriter( m_imageWriter );
		castor::GliImageWriter::registerWriter( m_imageWriter );

		// m_listenerCache *MUST* be the first created.
		m_listenerCache = castor::makeCache< FrameListener, castor::String, FrameListenerCacheTraits >( getLogger()
			, castor::DummyFunctorT< FrameListenerCache >{}
			, listenerClean );
		m_defaultListener = m_listenerCache->add( defaultName );

		m_shaderCache = makeCache( *this );
		m_samplerCache = castor::makeCache< Sampler, castor::String, SamplerCacheTraits >( getLogger()
			, GpuEventInitialiserT< SamplerCache >{ *m_defaultListener.lock() }
			, CpuEventCleanerT< SamplerCache >{ *m_defaultListener.lock() } );
		m_materialCache = castor::makeCache< Material, castor::String, MaterialCacheTraits >( *this );
		m_pluginCache = castor::makeCache< Plugin, castor::String, PluginCacheTraits >( *this );
		m_overlayCache = castor::makeCache< Overlay, castor::String, OverlayCacheTraits >( *this );
		m_sceneCache = castor::makeCache< Scene, castor::String, SceneCacheTraits >( getLogger()
			, castor::ResourceInitialiserT< SceneCache >{}
			, castor::ResourceCleanerT< SceneCache >{} );
		m_targetCache = std::make_unique< RenderTargetCache >( *this );

		if ( !castor::File::directoryExists( getEngineDirectory() ) )
		{
			castor::File::directoryCreate( getEngineDirectory() );
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
		m_lightsSampler = {};
		m_defaultSampler = {};

		// To destroy before RenderSystem, since it contain elements instantiated in Renderer plug-in
		m_samplerCache->clear();
		m_shaderCache->clear();
		m_overlayCache->clear();
		m_fontCache.clear();
		m_imageCache.clear();
		m_sceneCache->clear();
		m_loadingScene.reset();
		m_materialCache->clear();
		m_listenerCache->clear();

		// Destroy the RenderSystem.
		if ( m_renderSystem )
		{
			m_renderSystem.reset();
		}

		// and eventually the  plug-ins.
		m_pluginCache->clear();
		castor::GliImageLoader::unregisterLoader( m_imageLoader );
		castor::StbImageLoader::unregisterLoader( m_imageLoader );
		castor::ExrImageLoader::unregisterLoader( m_imageLoader );
		castor::XpmImageLoader::unregisterLoader( m_imageLoader );
		castor::FreeImageLoader::unregisterLoader( m_imageLoader );
		castor::StbImageWriter::unregisterWriter( m_imageWriter );
		castor::GliImageWriter::unregisterWriter( m_imageWriter );
		cleanupGlslang();

		m_logger = nullptr;
		log::cleanup();
	}

	void Engine::initialise( uint32_t wanted, bool threaded )
	{
		castor::Debug::initialise();
		m_threaded = threaded;

		if ( m_renderSystem )
		{
			if ( auto created = m_samplerCache->create( cuT( "Default" ), *this ) )
			{
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_REPEAT );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_REPEAT );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_REPEAT );
				m_defaultSampler = m_samplerCache->add( created->getName(), created, true );
			}

			if ( auto created = m_samplerCache->create( samplerName, *this ) )
			{
				created->setMinFilter( VK_FILTER_NEAREST );
				created->setMagFilter( VK_FILTER_NEAREST );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				m_lightsSampler = m_samplerCache->add( created->getName(), created, true );
			}

			doLoadCoreData();
		}

		if ( !m_renderSystem )
		{
			CU_Exception( C3D_NO_RENDERSYSTEM );
		}

		if ( m_lightsSampler.lock() )
		{
			postEvent( makeGpuInitialiseEvent( **m_lightsSampler.lock() ) );
		}

		if ( m_defaultSampler.lock() )
		{
			postEvent( makeGpuInitialiseEvent( **m_defaultSampler.lock() ) );
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

			if ( m_loadingScene )
			{
				m_loadingScene->cleanup();
			}

			m_samplerCache->cleanup();
			m_overlayCache->cleanup();
			m_materialCache->cleanup();

			if ( m_lightsSampler.lock() )
			{
				postEvent( makeCpuCleanupEvent( *m_lightsSampler.lock() ) );
			}

			if ( m_defaultSampler.lock() )
			{
				postEvent( makeCpuCleanupEvent( *m_defaultSampler.lock() ) );
			}

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
		}

		castor::Debug::cleanup();
	}

	bool Engine::loadRenderer( castor::String const & type )
	{
		auto it = m_rendererList.find( type );

		if ( it != m_rendererList.end() )
		{
			m_renderSystem = castor::makeUnique< RenderSystem >( *this, *it );
		}

		return m_renderSystem != nullptr;
	}

	void Engine::loadRenderer( Renderer renderer )
	{
		m_renderSystem = castor::makeUnique< RenderSystem >( *this
			, std::move( renderer ) );
	}

	CpuFrameEvent * Engine::postEvent( CpuFrameEventUPtr event )
	{
		CpuFrameEvent * result = nullptr;
		FrameListenerSPtr listener = m_defaultListener.lock();

		if ( listener )
		{
			result = event.get();
			listener->postEvent( std::move( event ) );
		}

		return result;
	}

	void Engine::sendEvent( GpuFrameEventUPtr event )
	{
		auto & device = m_renderSystem->getRenderDevice();
		auto data = device.graphicsData();
		event->apply( device, *data );
	}

	GpuFrameEvent * Engine::postEvent( GpuFrameEventUPtr event )
	{
		GpuFrameEvent * result = nullptr;
		FrameListenerSPtr listener = m_defaultListener.lock();

		if ( listener )
		{
			result = event.get();
			listener->postEvent( std::move( event ) );
		}

		return result;
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
		getMaterialCache().update( updater );
		getSceneCache().forEach( [&updater]( Scene & scene )
			{
				scene.update( updater );
			} );
		getRenderTargetCache().forEach( [&updater]( RenderTarget & target )
			{
				if ( target.getTargetType() != TargetType::eWindow )
				{
					TechniqueQueues techniqueQueues;
					updater.queues = &techniqueQueues.queues;
					target.update( updater );

					if ( !techniqueQueues.queues.empty() )
					{
						techniqueQueues.shadowMaps = target.getShadowMaps();
						updater.techniquesQueues.push_back( techniqueQueues );
					}
				}
			} );

		for ( auto & window : m_renderWindows )
		{
			TechniqueQueues techniqueQueues;
			updater.queues = &techniqueQueues.queues;
			window.second->update( updater );

			if ( !techniqueQueues.queues.empty() )
			{
				techniqueQueues.shadowMaps = window.second->getShadowMaps();
				updater.techniquesQueues.push_back( techniqueQueues );
			}
		}
	}

	void Engine::update( GpuUpdater & updater )
	{
		getMaterialCache().update( updater );
		getRenderTargetCache().update( updater );

		for ( auto & window : m_renderWindows )
		{
			window.second->update( updater );
		}
	}

	castor::Path Engine::getPluginsDirectory()
	{
		castor::Path binDir = castor::File::getExecutableDirectory();

		while ( binDir.getFileName() != cuT( "bin" ) )
		{
			binDir = binDir.getPath();
		}

		castor::Path usrDir = binDir.getPath();

#if defined( CU_PlatformWindows )
		static std::basic_string_view< xchar > constexpr pluginsSubdir = cuT( "bin" );
#else
		static std::basic_string_view< xchar > constexpr pluginsSubdir = cuT( "lib" );
#endif
		return usrDir / pluginsSubdir.data() / cuT( "Castor3D" );
	}

	castor::Path Engine::getEngineDirectory()
	{
		return castor::File::getUserDirectory() / cuT( ".Castor3D" );
	}

	castor::Path Engine::getDataDirectory()
	{
		castor::Path pathReturn;
		castor::Path pathBin = castor::File::getExecutableDirectory();

		while ( pathBin.getFileName() != cuT( "bin" ) )
		{
			pathBin = pathBin.getPath();
		}

		castor::Path pathUsr = pathBin.getPath();
		pathReturn = pathUsr / cuT( "share" );
		return pathReturn;
	}

	std::locale const & Engine::getLocale()
	{
		static std::locale const loc{ "C" };
		return loc;
	}

	castor::String Engine::getPassesName()const
	{
		return m_passFactory->getIdName( m_passesType );
	}

	ToneMappingFactory const & Engine::getToneMappingFactory()const
	{
		return getRenderTargetCache().getToneMappingFactory();
	}

	ToneMappingFactory & Engine::getToneMappingFactory()
	{
		return getRenderTargetCache().getToneMappingFactory();
	}

	PostEffectFactory const & Engine::getPostEffectFactory()const
	{
		return getRenderTargetCache().getPostEffectFactory();
	}

	PostEffectFactory & Engine::getPostEffectFactory()
	{
		return getRenderTargetCache().getPostEffectFactory();
	}

	castor::RgbaColour Engine::getNextRainbowColour()const
	{
		static float currentColourHue{ 0.0f };
		currentColourHue += 0.05f;

		if ( currentColourHue > 1.0f )
		{
			currentColourHue = 0.0f;
		}

		return castor::RgbaColour::fromHSB( currentColourHue, 1.0f, 1.0f );
	}

	uint32_t Engine::registerTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		if ( hasRenderLoop() )
		{
			return m_renderLoop->registerTimer( category, timer );
		}

		return 0u;
	}

	void Engine::unregisterTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		if ( hasRenderLoop() )
		{
			m_renderLoop->unregisterTimer( category, timer );
		}
	}

	void Engine::registerLightingModel( castor::String const & name
		, shader::LightingModelCreator creator )
	{
		m_lightingModelFactory.registerType( name, creator );
	}

	void Engine::unregisterLightingModel( castor::String const & name )
	{
		m_lightingModelFactory.unregisterType( name );
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
#if !defined( NDEBUG )
		auto result = m_renderWindows.emplace( window.getName(), &window ).second;
		CU_Assert( result, "Duplicate window." );
#else
		m_renderWindows.emplace( window.getName(), &window );
#endif
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

	void Engine::registerParsers( castor::String const & name, castor::AttributeParsers const & parsers )
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
		pushCpuJob( [&pass]()
			{
				pass.prepareTextures();
			} );
	}

	void Engine::pushCpuJob( castor::AsyncJobQueue::Job job )
	{
		m_cpuJobs.pushJob( job );
	}

	void Engine::pushGpuJob( std::function< void( RenderDevice const &, QueueData const & ) > job )
	{
		m_gpuJobs.pushJob( [this, job]()
			{
				auto & device = m_renderSystem->getRenderDevice();
				auto data = device.graphicsData();
				job( device, *data );
			} );
	}

	void Engine::setLoadingScene( SceneSPtr scene )
	{
		m_loadingScene = std::move( scene );
		m_loadingScene->initialise();
	}

	void Engine::doLoadCoreData()
	{
		castor::Path path = Engine::getDataDirectory() / cuT( "Castor3D" );

		if ( castor::File::fileExists( path / cuT( "Core.zip" ) ) )
		{
			SceneFileParser parser( *this );

			if ( !parser.parseFile( path / cuT( "Core.zip" ) ) )
			{
				log::error << cuT( "Can't read Core.zip data file" ) << std::endl;
			}
		}
	}
}

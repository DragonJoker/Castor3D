#include "Castor3D/Engine.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshFactory.hpp"
#include "Castor3D/Overlay/DebugOverlays.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Plugin/Plugin.hpp"
#include "Castor3D/Render/PBR/BrdfPrefilter.hpp"
#include "Castor3D/Render/RenderLoopAsync.hpp"
#include "Castor3D/Render/RenderLoopSync.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslIblBackground.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslImgBackground.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslNoIblBackground.hpp"
#include "Castor3D/Shader/GlslToSpv.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/DataImageLoader.hpp>
#include <CastorUtils/Graphics/ExrImageLoader.hpp>
#include <CastorUtils/Graphics/FreeImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageWriter.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Graphics/XpmImageLoader.hpp>
#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>
#include <CastorUtils/Pool/UniqueObjectPool.hpp>

#include <ashespp/Image/StagingTexture.hpp>

#include <string_view>

namespace castor3d
{
	//*********************************************************************************************

	namespace eng
	{
		static constexpr bool C3D_EnableAPITrace = false;
		static const char * C3D_NO_RENDERSYSTEM = "No RenderSystem loaded, call castor3d::Engine::loadRenderer before castor3d::Engine::Initialise";

		static castor::String const defaultName = cuT( "C3D_Default" );
		static castor::String const samplerName = cuT( "C3D_Lights" );

		static castor::LoggerInstancePtr createLogger( castor::LogType type
			, castor::Path const & filePath
			, castor::Path const & debugFilePath )
		{
			auto result = castor::Logger::createInstance( type );
			result->setFileName( filePath, castor::LogType::eError );
			result->setFileName( filePath, castor::LogType::eWarning );
			result->setFileName( filePath, castor::LogType::eInfo );
			result->setFileName( debugFilePath, castor::LogType::eDebug );
			result->setFileName( debugFilePath, castor::LogType::eTrace );
			return result;
		}

		static Texture doCreatePrefilteredBrdf( Engine & engine
			, RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::Size const & size )
		{
#if !C3D_GenerateBRDFIntegration
			Texture result{ device
				, resources
				, "BrdfLUT"
				, 0u
				, { size[0], size[1], 1u }
				, 1u
				, 1u
				, VK_FORMAT_R8G8B8A8_UNORM
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT ) };
			result.create();
			auto image = std::make_unique< ashes::Image >( *device, result.image, result.imageId.data->info );
			auto imagePath = Engine::getEngineDirectory() / cuT( "Core" ) / cuT( "brdf.png" );
			castor::ImageResPtr created;
			castor::ImageResPtr img = engine.tryAddImage( cuT( "BRDF" )
				, true
				, created
				, castor::ImageCreateParams{ imagePath, { false, false, false } } );
			auto buffer = img.lock()->getPixels();
			buffer = castor::PxBufferBase::create( buffer->getDimensions()
				, castor::PixelFormat::eR8G8B8A8_UNORM
				, buffer->getConstPtr()
				, buffer->getFormat() );
			auto view = image->createView( VK_IMAGE_VIEW_TYPE_2D, result.getFormat() );
			auto staging = device->createStagingTexture( VK_FORMAT_R8G8B8A8_UNORM
				, makeExtent2D( buffer->getDimensions() ) );
			auto data = device.graphicsData();;
			staging->uploadTextureData( *data->queue
				, *data->commandPool
				, VK_FORMAT_R8G8B8A8_UNORM
				, buffer->getConstPtr()
				, view );
#else
			Texture result{ device
				, handler
				, "BrdfLUT"
				, 0u
				, { size[0], size[1], 1u }
				, 1u
				, 1u
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT ) };
			result.create();
			BrdfPrefilter filter{ engine
				, device
				, size
				, result };
			auto queueData = device.graphicsData();
			filter.render( *queueData );
			device->waitIdle();
#endif
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
		, m_importerFileFactory{ castor::makeUnique< ImporterFileFactory >() }
		, m_particleFactory{ castor::makeUnique< ParticleFactory >() }
		, m_enableValidation{ enableValidation }
		, m_enableApiTrace{ eng::C3D_EnableAPITrace }
		, m_cpuJobs{ std::max( 8u, std::min( 4u, castor::CpuInformations{}.getCoreCount() / 2u ) ) }
		, m_resources{ m_resourceHandler }
	{
		m_passFactory = castor::makeUnique< PassFactory >( *this );
		m_passComponents = castor::makeUnique< PassComponentRegister >( *this );

		auto listenerClean = []( auto & element )
		{
			element.flush();
		};
		initialiseGlslang();
		castor::DataImageLoader::registerLoader( m_imageLoader );
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
		m_defaultListener = m_listenerCache->add( eng::defaultName );

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
		m_textureCache = std::make_unique< TextureUnitCache >( *this );

		if ( !castor::File::directoryExists( getEngineDirectory() ) )
		{
			castor::File::directoryCreate( getEngineDirectory() );
		}

		m_lightingModelFactory = castor::makeUnique< LightingModelFactory >();

		registerBackgroundModel( shader::ImgBackgroundModel::Name
			, shader::ImgBackgroundModel::create );
		registerBackgroundModel( shader::NoIblBackgroundModel::Name
			, shader::NoIblBackgroundModel::create );
		registerBackgroundModel( shader::IblBackgroundModel::Name
			, shader::IblBackgroundModel::create );

		registerPassModels( { PhongPass::LightingModel
				, PhongPass::create
				, &shader::PhongLightingModel::create
				, false } );
		registerPassModels( { PbrPass::LightingModel
				, PbrPass::create
				, &shader::PbrLightingModel::create
				, true } );
		m_lightingModelId = getPassFactory().listRegisteredTypes().begin()->key;

		log::info << cuT( "Castor3D - Core engine version : " ) << Version{} << std::endl;
		log::info << m_cpuInformations << std::endl;
	}

	Engine::Engine( castor::String const & appName
		, Version const & appVersion
		, bool enableValidation )
		: Engine{ appName
			, appVersion
			, enableValidation
			, eng::createLogger( castor::Logger::getLevel()
				, getEngineDirectory() / cuT( "Castor3D.log" )
				, getEngineDirectory() / cuT( "Castor3D-Debug.log" ) )
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

		m_passComponents.reset();
		m_passFactory.reset();

		m_logger = nullptr;
		log::cleanup();
	}

	void Engine::initialise( uint32_t wanted, bool threaded )
	{
		castor::Debug::initialise();
		m_cpuJobs.reset();
		m_threaded = threaded;

		if ( !m_renderSystem )
		{
			CU_Exception( eng::C3D_NO_RENDERSYSTEM );
		}

		if ( auto created = m_samplerCache->create( cuT( "Default" ), *this ) )
		{
			created->setMinFilter( VK_FILTER_LINEAR );
			created->setMagFilter( VK_FILTER_LINEAR );
			created->setWrapS( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			created->setWrapT( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			created->setWrapR( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_defaultSampler = m_samplerCache->add( created->getName(), created, true );
		}

		if ( auto created = m_samplerCache->create( eng::samplerName, *this ) )
		{
			created->setMinFilter( VK_FILTER_NEAREST );
			created->setMagFilter( VK_FILTER_NEAREST );
			created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			m_lightsSampler = m_samplerCache->add( created->getName(), created, true );
		}

		if ( m_maxImageSize == 0xFFFFFFFF )
		{
			m_maxImageSize = m_renderSystem->getGpuInformations().getValue( GpuMax::eTexture2DSize );
		}

		m_textureCache->initialise( m_renderSystem->getRenderDevice() );

		if ( m_lightsSampler.lock() )
		{
			postEvent( makeGpuInitialiseEvent( **m_lightsSampler.lock() ) );
		}

		if ( m_defaultSampler.lock() )
		{
			postEvent( makeGpuInitialiseEvent( **m_defaultSampler.lock() ) );
		}

		doLoadCoreData();

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
			m_textureCache->stopLoad();
			m_cpuJobs.finish();

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
			m_textureCache->cleanup();

			postEvent( makeGpuCleanupEvent( *m_targetCache ) );

			if ( m_lightsSampler.lock() )
			{
				postEvent( makeCpuCleanupEvent( *m_lightsSampler.lock() ) );
			}

			if ( m_defaultSampler.lock() )
			{
				postEvent( makeCpuCleanupEvent( *m_defaultSampler.lock() ) );
			}

			postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					m_textureCache->clear();
				} ) );

			m_brdf.destroy();
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
		getSceneCache().forEach( [&updater]( Scene & scene )
			{
				scene.update( updater );
			} );
		getRenderTargetCache().update( updater );
		m_textureCache->update( updater );

		for ( auto & window : m_renderWindows )
		{
			window.second->update( updater );
		}
	}

	void Engine::upload( ashes::CommandBuffer const & commandBuffer )
	{
		getMaterialCache().upload( commandBuffer );
		getSceneCache().forEach( [&commandBuffer]( Scene & scene )
			{
				scene.getLightCache().upload( commandBuffer );
			} );
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

	castor::String Engine::getDefaultLightingModelName()const
	{
		return getPassFactory().getIdName( getDefaultLightingModel() );
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

	uint32_t Engine::getWantedFps()const
	{
		return m_renderLoop->getWantedFps();
	}

	castor3d::MaterialRPtr Engine::getDefaultMaterial()const
	{
		return m_materialCache->getDefaultMaterial();
	}

	bool Engine::hasMeshShaders()const
	{
		return m_renderSystem->getGpuInformations().hasShaderType( VK_SHADER_STAGE_MESH_BIT_NV );
	}

	uint32_t Engine::getMaxPassTypeCount()const
	{
		return m_materialCache->getMaxPassTypeCount();
	}

	RenderDevice * Engine::getRenderDevice()const
	{
		return m_renderSystem
			? &m_renderSystem->getRenderDevice()
			: nullptr;
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

	LightingModelID Engine::registerLightingModel( castor::String const & name
		, shader::LightingModelCreator creator
		, BackgroundModelID backgroundModelId )
	{
		return getLightingModelFactory().registerType( name, backgroundModelId, creator );
	}

	void Engine::unregisterLightingModel( castor::String const & name
		, BackgroundModelID backgroundModelId )
	{
		getLightingModelFactory().unregisterType( name, backgroundModelId );
	}

	void Engine::unregisterLightingModel( LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId )
	{
		getLightingModelFactory().unregisterType( lightingModelId, backgroundModelId );
	}

	BackgroundModelID Engine::registerBackgroundModel( castor::String const & name
		, shader::BackgroundModelCreator creator )
	{
		return BackgroundModelID( getBackgroundModelFactory().registerType( name, creator ).id );
	}

	BackgroundModelID Engine::unregisterBackgroundModel( castor::String const & name )
	{
		auto result = getBackgroundModelFactory().getTypeId( name );
		getBackgroundModelFactory().unregisterType( name );
		return BackgroundModelID( result );
	}

	void Engine::registerBuffer( ShaderBuffer const & buffer )
	{
		if ( hasRenderLoop() )
		{
			m_renderLoop->registerBuffer( buffer );
		}
	}

	void Engine::unregisterBuffer( ShaderBuffer const & buffer )
	{
		if ( hasRenderLoop() )
		{
			m_renderLoop->unregisterBuffer( buffer );
		}
	}

	void Engine::registerPassModel( BackgroundModelID backgroundModelId
		, PassRegisterInfo info )
	{
		auto lightingModelId = registerLightingModel( info.lightingModel
			, info.lightingModelCreator
			, backgroundModelId );
		getPassFactory().registerType( lightingModelId
			, std::move( info ) );
	}

	void Engine::registerPassModels( PassRegisterInfo info )
	{
		std::set< LightingModelID > lightingModels;

		for ( auto entry : getBackgroundModelFactory().listRegisteredTypes() )
		{
			lightingModels.emplace( registerLightingModel( info.lightingModel
				, info.lightingModelCreator
				, entry.id ) );
		}

		for ( auto lightingModelId : lightingModels )
		{
			getPassFactory().registerType( lightingModelId, info );
		}
	}

	void Engine::unregisterPassModel( BackgroundModelID backgroundModelId
		, LightingModelID lightingModelId )
	{
		getPassFactory().unregisterType( lightingModelId );
		unregisterLightingModel( lightingModelId
			, backgroundModelId );
	}

	void Engine::unregisterPassModels( castor::String const & type ) try
	{
		auto lightingModelId = getPassFactory().getNameId( type );
		getPassFactory().unregisterType( lightingModelId );

		for ( auto entry : getBackgroundModelFactory().listRegisteredTypes() )
		{
			unregisterLightingModel( lightingModelId
				, entry.id );
		}
	}
	catch ( std::exception & exc )
	{
		log::error << "Engine::unregisterPassModels - " << exc.what() << std::endl;
	}

	void Engine::registerSpecificsBuffer( std::string const & name
		, SpecificsBuffer buffer )
	{
		m_materialCache->registerSpecificsBuffer( name, buffer );
	}

	void Engine::unregisterSpecificsBuffer( std::string const & name )
	{
		m_materialCache->unregisterSpecificsBuffer( name );
	}

	void Engine::addSpecificsBuffersBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		m_materialCache->addSpecificsBuffersBindings( bindings, shaderStages, index );
	}

	void Engine::addSpecificsBuffersDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		m_materialCache->addSpecificsBuffersDescriptors( descriptorWrites, index );
	}

	void Engine::createSpecificsBuffersPassBindings( crg::FramePass & pass
		, uint32_t & index )const
	{
		m_materialCache->createSpecificsBuffersPassBindings( pass, index );
	}

	void Engine::declareSpecificsShaderBuffers( sdw::ShaderWriter & writer
		, std::map< std::string, shader::BufferBaseUPtr > & buffers
		, uint32_t & binding
		, uint32_t set )const
	{
		m_materialCache->declareSpecificsShaderBuffers( writer, buffers, binding, set );
	}

	PassComponentID Engine::registerPassComponent( castor::String const & type
		, PassComponentPluginUPtr componentPlugin )
	{
		return m_passComponents->registerComponent( type
			, std::move( componentPlugin ) );
	}

	void Engine::unregisterPassComponent( castor::String const & type )
	{
		m_passComponents->unregisterComponent( type );
	}

	void Engine::registerRenderPassType( castor::String const & renderPassType
		, castor::UniquePtr< RenderPassRegisterInfo > info )
	{
		if ( info )
		{
			auto & ninfo = *m_passRenderPassTypes.emplace( renderPassType, std::move( info ) ).first->second;
			auto ires = m_renderPassTypes.emplace( renderPassType, std::make_pair( RenderPassTypeID{}, Parameters{} ) );

			if ( ires.second )
			{
				ires.first->second.first = RenderPassTypeID( m_renderPassTypes.size() );
			}

			ninfo.id = ires.first->second.first;
		}
	}

	RenderPassTypeID Engine::getRenderPassTypeID( castor::String const & renderPassType )const
	{
		auto it = m_renderPassTypes.find( renderPassType );

		if ( it == m_renderPassTypes.end() )
		{
			return RenderPassTypeID{};
		}

		return it->second.first;
	}

	void Engine::setRenderPassTypeConfiguration( castor::String const & renderPassType
		, Parameters parameters )
	{
		auto it = m_renderPassTypes.find( renderPassType );

		if ( it != m_renderPassTypes.end() )
		{
			it->second.second = std::move( parameters );
		}
	}

	Parameters Engine::getRenderPassTypeConfiguration( castor::String const & renderPassType )const
	{
		auto it = m_renderPassTypes.find( renderPassType );

		if ( it != m_renderPassTypes.end() )
		{
			return it->second.second;
		}

		return Parameters{};
	}

	RenderPassRegisterInfo * Engine::getRenderPassInfo( castor::String const & renderPassType )const
	{
		auto it = m_passRenderPassTypes.find( renderPassType );

		if ( it == m_passRenderPassTypes.end() )
		{
			log::error << cuT( "Can't find render pass info for [" ) << renderPassType << "]" << std::endl;
			return nullptr;
		}

		return it->second.get();
	}

	std::vector< RenderPassRegisterInfo * > Engine::getRenderPassInfos( TechniquePassEvent event )const
	{
		std::vector< RenderPassRegisterInfo * > result;
		std::set< castor::String > inserted;

		for ( auto & renderPass : m_passRenderPassTypes )
		{
			if ( renderPass.second->event == event )
			{
				auto ires = inserted.insert( renderPass.second->name );

				if ( ires.second )
				{
					result.push_back( renderPass.second.get() );
				}
			}
		}

		return result;
	}

	void Engine::unregisterRenderPassType( castor::String const & renderPassType )
	{
		auto it = m_passRenderPassTypes.find( renderPassType );

		if ( it != m_passRenderPassTypes.end() )
		{
			m_renderPassTypes.erase( renderPassType );
			m_passRenderPassTypes.erase( it );
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

	void Engine::registerParsers( castor::String const & name
		, castor::AttributeParsers const & parsers
		, castor::StrUInt32Map const & sections
		, castor::UserContextCreator contextCreator )
	{
		auto && it = m_additionalParsers.find( name );

		if ( it != m_additionalParsers.end() )
		{
			CU_Exception( "registerParsers - Duplicate entry for " + name );
		}

		m_additionalParsers.emplace( name
			, castor::AdditionalParsers{ std::move( parsers )
				, std::move( sections )
				, std::move( contextCreator ) } );
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

	void Engine::pushCpuJob( castor::AsyncJobQueue::Job job )
	{
		m_cpuJobs.pushJob( job );
	}

	void Engine::setLoadingScene( SceneUPtr scene )
	{
		bool hadLoadingScene = false;

		if ( m_loadingScene )
		{
			for ( auto & windowIt : m_renderWindows )
			{
				windowIt.second->destroyLoadingScreen();
			}

			m_loadingScene->cleanup();
			auto pending = m_loadingScene.release();
			hadLoadingScene = true;
			postEvent( makeCpuFunctorEvent( EventType::ePreRender
				, [pending]()
				{
					delete pending;
				} ) );
		}

		m_loadingScene = std::move( scene );
		m_loadingScene->initialise();

		if ( hadLoadingScene )
		{
			for ( auto & windowIt : m_renderWindows )
			{
				windowIt.second->createLoadingScreen();
			}
		}
	}

	void Engine::doLoadCoreData()
	{
		castor::Path path = Engine::getDataDirectory() / cuT( "Castor3D" );

		if ( castor::File::fileExists( path / cuT( "Core.zip" ) ) )
		{
			SceneFileParser parser( *this );

			if ( !parser.parseFile( "Castor3D", path / cuT( "Core.zip" ) ) )
			{
				log::error << cuT( "Can't read Core.zip data file" ) << std::endl;
			}
		}
		
		auto & device = m_renderSystem->getRenderDevice();
		m_brdf = eng::doCreatePrefilteredBrdf( *this
			, device
			, m_resources
			, { PrefilteredBrdfMapSize, PrefilteredBrdfMapSize } );
		m_brdf.create();

	}
}

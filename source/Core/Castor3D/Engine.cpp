#include "Castor3D/Engine.hpp"

#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/LoadingScreen.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshFactory.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Overlay/DebugOverlays.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Plugin/Plugin.hpp"
#include "Castor3D/Render/PBR/BrdfPrefilter.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderLoopAsync.hpp"
#include "Castor3D/Render/RenderLoopSync.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Scene/CscnImporterFile.hpp"
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
#include <CastorUtils/Graphics/Ktx2ImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Graphics/WebPImageLoader.hpp>
#include <CastorUtils/Graphics/XpmImageLoader.hpp>
#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

#include <ashespp/Image/StagingTexture.hpp>

#include <string_view>

CU_ImplementSmartPtr( castor3d, Engine )

namespace castor3d
{
	//*********************************************************************************************

	namespace eng
	{
		static bool constexpr C3D_GenerateBRDFIntegration = true;
		static castor::StringView constexpr noRenderSystem{ cuT( "No RenderSystem loaded, call castor3d::Engine::loadRenderer before castor3d::Engine::Initialise" ) };
		static castor::StringView constexpr defaultName{ cuT( "C3D_Default" ) };
		static castor::StringView constexpr samplerName{ cuT( "C3D_Lights" ) };
		static std::locale const globalLocale{ "C" };

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
			if constexpr ( !C3D_GenerateBRDFIntegration )
			{
				Texture result{ device
					, resources
					, cuT( "BrdfLUT" )
					, 0u
					, { size[0], size[1], 1u }
					, 1u
					, 1u
					, VK_FORMAT_R8G8B8A8_UNORM
					, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
						| VK_IMAGE_USAGE_TRANSFER_DST_BIT
						| VK_IMAGE_USAGE_SAMPLED_BIT ) };
				result.create();
				castor::PxBufferBase const * bufferRG;
				castor::PxBufferBase const * bufferB;
				{
					auto imagePath = Engine::getEngineDirectory() / cuT( "Core" ) / cuT( "brdf_ggx.png" );
					castor::ImageResPtr created;
					auto img = engine.addNewImage( cuT( "BRDFLutGGX" )
						, castor::ImageCreateParams{ imagePath, { false, false, false } } );
					bufferRG = img->getPixels();
				}
				{
					auto imagePath = Engine::getEngineDirectory() / cuT( "Core" ) / cuT( "brdf_charlie.png" );
					auto img = engine.addNewImage( cuT( "BRDFLutCharlie" )
						, castor::ImageCreateParams{ imagePath, { false, false, false } } );
					bufferB = img->getPixels();
				}

				auto buffer = castor::PxBufferBase::create( bufferRG->getDimensions()
					, castor::PixelFormat::eR8G8B8A8_UNORM
					, bufferRG->getConstPtr()
					, bufferRG->getFormat() );
				copyBufferComponents( castor::PixelComponent::eBlue
					, castor::PixelComponent::eBlue
					, *bufferB
					, *buffer );

				auto image = result.image.get();
				auto view = image->createView( VK_IMAGE_VIEW_TYPE_2D, result.getFormat() );
				auto staging = device->createStagingTexture( VK_FORMAT_R8G8B8A8_UNORM
					, makeExtent2D( buffer->getDimensions() ) );
				auto data = device.graphicsData();
				staging->uploadTextureData( *data->queue
					, *data->commandPool
					, VK_FORMAT_R8G8B8A8_UNORM
					, buffer->getConstPtr()
					, view );
				return result;
			}
			else
			{
				Texture result{ device
					, resources
					, cuT( "GeneratedBrdfLUT" )
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
				return result;
			}
		}

		static CU_ImplementAttributeParserNewBlock( parserScene, RootContext, SceneContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				newBlockContext->root = blockContext;
				newBlockContext->scene = newBlockContext->root->engine->tryFindScene( name );

				if ( !newBlockContext->scene )
				{
					newBlockContext->ownScene = castor::makeUnique< Scene >( name
						, *newBlockContext->root->engine );
					newBlockContext->scene = newBlockContext->ownScene.get();
				}

				newBlockContext->root->mapScenes.try_emplace( name, newBlockContext->scene );
				newBlockContext->overlays = castor::makeUnique< OverlayContext >();
				newBlockContext->overlays->root = blockContext;
				newBlockContext->overlays->scene = newBlockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eScene )

		static CU_ImplementAttributeParserNewBlock( parserLoadingScreen, RootContext, SceneContext )
		{
			newBlockContext->root = blockContext;
			newBlockContext->ownScene = castor::makeUnique< Scene >( LoadingScreen::SceneName
				, *newBlockContext->root->engine );
			newBlockContext->scene = newBlockContext->ownScene.get();
			newBlockContext->overlays = castor::makeUnique< OverlayContext >();
			newBlockContext->overlays->root = blockContext;
			newBlockContext->overlays->scene = newBlockContext;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eScene )

		static CU_ImplementAttributeParserNewBlock( parserFont, RootContext, FontContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( newBlockContext->name );
				newBlockContext->root = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eFont )

		static CU_ImplementAttributeParserNewBlock( parserSdfFont, RootContext, FontContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( newBlockContext->name );
				newBlockContext->root = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSdfFont )

		static CU_ImplementAttributeParserBlock( parserPanelOverlay, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->engine->tryFindOverlay( name );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *blockContext->engine
						, OverlayType::ePanel
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::ePanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlay, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->engine->tryFindOverlay( name );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *blockContext->engine
						, OverlayType::eBorderPanel
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eBorderPanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserTextOverlay, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->engine->tryFindOverlay( name );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *blockContext->engine
						, OverlayType::eText
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eTextOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserNewBlock( parserSamplerState, RootContext, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );
				newBlockContext->sampler = getEngine( *blockContext )->tryFindSampler( name );

				if ( !newBlockContext->sampler )
				{
					newBlockContext->ownSampler = getEngine( *blockContext )->createSampler( name
						, *getEngine( *blockContext ) );
					newBlockContext->sampler = newBlockContext->ownSampler.get();
				}
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSampler )

		static CU_ImplementAttributeParserBlock( parserDebugOverlays, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->engine->getRenderLoop().showDebugOverlays( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDebugTargets, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->engine->enableDebugTargets( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserWindow, RootContext, WindowContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->root = blockContext;
				params[0]->get( newBlockContext->window.name );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eWindow )

		static CU_ImplementAttributeParserBlock( parserMaxImageSize, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [count] parameter." ) );
			}
			else
			{
				blockContext->engine->setMaxImageSize( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDebugMaxImageSize, RootContext )
		{
	#if !defined( NDEBUG )
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [count] parameter." ) );
			}
			else
			{
				blockContext->engine->setMaxImageSize( params[0]->get< uint32_t >() );
			}
	#endif
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLpvGridSize, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [count] parameter." ) );
			}
			else
			{
				blockContext->engine->setLpvGridSize( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDefaultUnit, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [unit] parameter." ) );
			}
			else
			{
				blockContext->engine->setLengthUnit( castor::LengthUnit( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFullLoading, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [enable] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->enableFullLoading );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	EngineCounts::EngineCounts( Engine const & engine )
		: device{ *engine.getRenderDevice() }
		, fontCount{ engine.getFontsCount() }
		, materialCount{ engine.getMaterialsCount() }
		, overlayCount{ engine.getOverlaysCount() }
		, samplerCount{ engine.getSamplersCount() }
	{
	}

	//*********************************************************************************************

	Engine::Engine( EngineConfig config
		, castor::LoggerInstancePtr ownedLogger
		, castor::LoggerInstance * logger )
		: Unique< Engine >( this )
		, m_ownedLogger{ castor::move( ownedLogger ) }
		, m_logger{ log::initialise( *( logger ? logger : m_ownedLogger.get() ) ) }
		, m_config{ castor::move( config ) }
		, m_fontCache{ *m_logger }
		, m_imageCache{ *m_logger, m_imageLoader }
		, m_meshFactory{ castor::makeUnique< MeshFactory >() }
		, m_importerFileFactory{ castor::makeUnique< ImporterFileFactory >() }
		, m_particleFactory{ castor::makeUnique< ParticleFactory >() }
		, m_cpuJobs{ std::max( 8u, std::min( 4u, castor::CpuInformations{}.getCoreCount() / 2u ) ) }
		, m_resources{ m_resourceHandler }
	{
		m_passFactory = castor::makeUnique< PassFactory >( *this );
		m_passComponents = castor::makeUnique< PassComponentRegister >( *this );
		m_submeshComponents = castor::makeUnique< SubmeshComponentRegister >( *this );

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
		castor::WebPImageLoader::registerLoader( m_imageLoader );
		castor::Ktx2ImageLoader::registerLoader( m_imageLoader );
		castor::StbImageWriter::registerWriter( m_imageWriter );
		castor::GliImageWriter::registerWriter( m_imageWriter );

		// m_listenerCache *MUST* be the first created.
		m_listenerCache = castor::makeCache< FrameListener, castor::String, FrameListenerCacheTraits >( getLogger()
			, castor::DummyFunctorT< FrameListenerCache >{}
			, listenerClean );
		m_defaultListener = addNewFrameListener( castor::String{ eng::defaultName } );

		m_shaderCache = makeCache( *this );
		m_samplerCache = castor::makeCache< Sampler, castor::String, SamplerCacheTraits >( getLogger()
			, GpuEventInitialiserT< SamplerCache >{ *m_defaultListener }
			, CpuEventCleanerT< SamplerCache >{ *m_defaultListener } );
		m_materialCache = castor::makeCache< Material, castor::String, MaterialCacheTraits >( *this );
		m_pluginCache = castor::makeCache< Plugin, castor::String, PluginCacheTraits >( *this );
		m_overlayCache = castor::makeCache< Overlay, castor::String, OverlayCacheTraits >( *this );
		m_sceneCache = castor::makeCache< Scene, castor::String, SceneCacheTraits >( getLogger()
			, castor::ResourceInitialiserT< SceneCache >{}
			, castor::ResourceCleanerT< SceneCache >{} );
		m_targetCache = castor::makeUnique< RenderTargetCache >( *this );
		m_textureCache = castor::makeUnique< TextureUnitCache >( *this, m_resources );

		if ( !castor::File::directoryExists( getEngineDirectory() ) )
		{
			castor::File::directoryCreate( getEngineDirectory() );
		}

		getImporterFileFactory().registerType( cuT( "cscn" )
			, cuT( "cscn" )
			, &CscnImporterFile::create );

		m_lightingModelFactory = castor::makeUnique< LightingModelFactory >();

		registerBackgroundModel( shader::ImgBackgroundModel::Name
			, shader::ImgBackgroundModel::create );
		registerBackgroundModel( shader::NoIblBackgroundModel::Name
			, shader::NoIblBackgroundModel::create );
		registerBackgroundModel( shader::IblBackgroundModel::Name
			, shader::IblBackgroundModel::create );

		for ( auto & desc : PhongPass::DiffuseBrdfs )
		{
			registerDiffuseBrdf( desc );
		}
		for ( auto & desc : PhongPass::SpecularBrdfs )
		{
			registerSpecularBrdf( desc );
		}
		for ( auto & desc : PhongPass::SheenBrdfs )
		{
			registerSheenBrdf( desc );
		}
		for ( auto & desc : PhongPass::ClearcoatBrdfs )
		{
			registerClearcoatBrdf( desc );
		}
		for ( auto & desc : PhongPass::ScatteringModels )
		{
			registerScatteringModel( desc );
		}

		for ( auto & desc : PbrPass::DiffuseBrdfs )
		{
			registerDiffuseBrdf( desc );
		}
		for ( auto & desc : PbrPass::SpecularBrdfs )
		{
			registerSpecularBrdf( desc );
		}
		for ( auto & desc : PbrPass::SheenBrdfs )
		{
			registerSheenBrdf( desc );
		}
		for ( auto & desc : PbrPass::ClearcoatBrdfs )
		{
			registerClearcoatBrdf( desc );
		}
		for ( auto & desc : PbrPass::ScatteringModels )
		{
			registerScatteringModel( desc );
		}

		registerLightingModel( castor::String{ PhongPass::LightingModel }
			, { PhongPass::DefaultDiffuseBrdf
				, PhongPass::DefaultSpecularBrdf
				, PhongPass::DefaultSheenBrdf
				, PhongPass::DefaultClearcoatBrdf
				, PhongPass::DefaultScatteringModel }
			, shader::PhongLightingModel::create );
		registerLightingModel( castor::String{ PbrPass::LightingModel }
			, { PbrPass::DefaultDiffuseBrdf
				, PbrPass::DefaultSpecularBrdf
				, PbrPass::DefaultSheenBrdf
				, PbrPass::DefaultClearcoatBrdf
				, PbrPass::DefaultScatteringModel }
			, shader::PbrLightingModel::create );

		registerPassModel( { castor::String{ PhongPass::LightingModel }
			, PhongPass::create } );
		registerPassModel( { castor::String{ PbrPass::LightingModel }
			, PbrPass::create } );

		auto & model = getLightingModelFactory().getModel( castor::String{ PbrPass::LightingModel } );
		m_lightingModelId = getLightingModelFactory().getLightingModelId( model.name
			, { model.defaultDesc.diffuse.name
				, model.defaultDesc.specular.name
				, model.defaultDesc.sheen.name
				, model.defaultDesc.clearcoat.name
				, model.defaultDesc.scattering.name } );

		registerParsers( ControlsManager::Name
			, ControlsManager::createParsers()
			, ControlsManager::createSections()
			, nullptr );
		setUserInputListenerT( castor::makeUnique< ControlsManager >( *this ) );

		log::info << cuT( "Castor3D - Core engine version : " ) << Version{} << std::endl;
		log::info << m_cpuInformations << std::endl;
	}

	Engine::Engine( EngineConfig config )
		: Engine{ castor::move( config )
			, eng::createLogger( castor::Logger::getLevel()
				, getEngineDirectory() / cuT( "Castor3D.log" )
				, getEngineDirectory() / cuT( "Castor3D-Debug.log" ) )
			, nullptr }
	{
	}
	
	Engine::Engine( EngineConfig config
		, castor::LoggerInstance & logger )
		: Engine{ castor::move( config )
			, nullptr
			, &logger }
	{
	}

	Engine::~Engine()noexcept
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
		castor::GliImageWriter::unregisterWriter( m_imageWriter );
		castor::StbImageWriter::unregisterWriter( m_imageWriter );
		castor::FreeImageLoader::unregisterLoader( m_imageLoader );
		castor::XpmImageLoader::unregisterLoader( m_imageLoader );
		castor::ExrImageLoader::unregisterLoader( m_imageLoader );
		castor::StbImageLoader::unregisterLoader( m_imageLoader );
		castor::GliImageLoader::unregisterLoader( m_imageLoader );
		castor::DataImageLoader::unregisterLoader( m_imageLoader );
		cleanupGlslang();

		m_submeshComponents.reset();
		m_passComponents.reset();
		m_passFactory.reset();

		m_logger = nullptr;
		log::cleanup();
	}

	void Engine::initialise( uint32_t wanted, bool threaded )
	{
		castor::debug::initialise();
		m_cpuJobs.reset();
		m_threaded = threaded;

		if ( !m_renderSystem )
		{
			CU_Exception( castor::toUtf8( eng::noRenderSystem ) );
		}

		if ( auto created = m_samplerCache->create( cuT( "Default" ), *this ) )
		{
			created->setMinFilter( VK_FILTER_LINEAR );
			created->setMagFilter( VK_FILTER_LINEAR );
			created->setWrapS( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			created->setWrapT( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			created->setWrapR( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			created->setSerialisable( false );
			m_defaultSampler = addSampler( created->getName(), created, true );
		}

		if ( auto created = m_samplerCache->create( castor::String{ eng::samplerName }, *this ) )
		{
			created->setMinFilter( VK_FILTER_NEAREST );
			created->setMagFilter( VK_FILTER_NEAREST );
			created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			created->setSerialisable( false );
			m_lightsSampler = addSampler( created->getName(), created, true );
		}

		if ( m_maxImageSize == 0xFFFFFFFF )
		{
			m_maxImageSize = m_renderSystem->getGpuInformations().getValue( GpuMax::eTexture2DSize );
		}

		m_textureCache->initialise( m_renderSystem->getRenderDevice() );

		if ( m_lightsSampler )
		{
			postEvent( makeGpuInitialiseEvent( *m_lightsSampler ) );
		}

		if ( m_defaultSampler )
		{
			postEvent( makeGpuInitialiseEvent( *m_defaultSampler ) );
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

		m_renderLoop->initialise();
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

			if ( m_lightsSampler )
			{
				postEvent( makeCpuCleanupEvent( *m_lightsSampler ) );
			}

			if ( m_defaultSampler )
			{
				postEvent( makeCpuCleanupEvent( *m_defaultSampler ) );
			}

			postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
				, [this]()
				{
					m_textureCache->clear();
				} ) );

			m_brdf.destroy();
			m_renderLoop.reset();

			if ( auto userInputs = getUserInputListener() )
			{
				userInputs->cleanup();
			}

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

		castor::debug::cleanup();
	}

	bool Engine::loadRenderer( castor::String const & type )
	{
		if ( auto it = m_rendererList.find( castor::toUtf8( type ) );
			it != m_rendererList.end() )
		{
			m_renderSystem = castor::makeUnique< RenderSystem >( *this, *it );
		}

		return m_renderSystem != nullptr;
	}

	void Engine::loadRenderer( Renderer renderer )
	{
		m_renderSystem = castor::makeUnique< RenderSystem >( *this
			, castor::move( renderer ) );
	}

	CpuFrameEvent * Engine::postEvent( CpuFrameEventUPtr event )const
	{
		CpuFrameEvent * result = nullptr;

		if ( auto listener = m_defaultListener )
		{
			result = event.get();
			listener->postEvent( castor::move( event ) );
		}

		return result;
	}

	void Engine::sendEvent( GpuFrameEventUPtr event )const
	{
		auto const & device = m_renderSystem->getRenderDevice();
		auto data = device.graphicsData();
		event->apply( device, *data );
	}

	GpuFrameEvent * Engine::postEvent( GpuFrameEventUPtr event )const
	{
		GpuFrameEvent * result = nullptr;

		if ( auto listener = m_defaultListener )
		{
			result = event.get();
			listener->postEvent( castor::move( event ) );
		}

		return result;
	}

	bool Engine::fireMouseMove( castor::Position const & position
		, bool ctrl
		, bool alt
		, bool shift )
	{
		for ( auto const & [_, listener] : m_windowInputListeners )
		{
			listener->fireMouseMove( position, ctrl, alt, shift );
		}

		auto inputListener = getUserInputListener();
		return inputListener && inputListener->fireMouseMove( position, ctrl, alt, shift );
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
						techniqueQueues.shadowBuffer = target.getShadowBuffer();
						updater.techniquesQueues.push_back( techniqueQueues );
					}
				}
			} );

		for ( auto const & [_, window] : m_renderWindows )
		{
			TechniqueQueues techniqueQueues;
			updater.queues = &techniqueQueues.queues;
			window->update( updater );

			if ( !techniqueQueues.queues.empty() )
			{
				techniqueQueues.shadowMaps = window->getShadowMaps();
				techniqueQueues.shadowBuffer = window->getShadowBuffer();
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

		for ( auto const & [_, window] : m_renderWindows )
		{
			window->update( updater );
		}
	}

	void Engine::upload( UploadData & uploader )
	{
		getMaterialCache().upload( uploader );
		getOverlayCache().upload( uploader );
		getSceneCache().forEach( [&uploader]( Scene & scene )
			{
				scene.upload( uploader );
			} );

		if ( m_loadingScene )
		{
			m_loadingScene->upload( uploader );
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
		return eng::globalLocale;
	}

	void Engine::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< RootContext > context{ result, CSCNSection::eRoot };

		context.addParser( cuT( "debug_overlays" ), eng::parserDebugOverlays, { makeParameter< ParameterType::eBool >() } );
		context.addParser( cuT( "debug_targets" ), eng::parserDebugTargets, { makeParameter< ParameterType::eBool >() } );
		context.addParser( cuT( "max_image_size" ), eng::parserMaxImageSize, { makeParameter< ParameterType::eUInt32 >() } );
		context.addParser( cuT( "debug_max_image_size" ), eng::parserDebugMaxImageSize, { makeParameter< ParameterType::eUInt32 >() } );
		context.addParser( cuT( "lpv_grid_size" ), eng::parserLpvGridSize, { makeParameter< ParameterType::eUInt32 >() } );
		context.addParser( cuT( "default_unit" ), eng::parserDefaultUnit, { makeParameter< ParameterType::eCheckedText, castor::LengthUnit >() } );
		context.addParser( cuT( "enable_full_loading" ), eng::parserFullLoading, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		context.addPushParser( cuT( "scene" ), CSCNSection::eScene, eng::parserScene, { makeParameter< ParameterType::eName >() } );
		context.addPushParser( cuT( "loading_screen" ), CSCNSection::eScene, eng::parserLoadingScreen, {} );
		context.addPushParser( cuT( "font" ), CSCNSection::eFont, eng::parserFont, { makeParameter< ParameterType::eName >() } );
		context.addPushParser( cuT( "sdf_font" ), CSCNSection::eSdfFont, eng::parserSdfFont, { makeParameter< ParameterType::eName >() } );
		context.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, eng::parserPanelOverlay, { makeParameter< ParameterType::eName >() } );
		context.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, eng::parserBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		context.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, eng::parserTextOverlay, { makeParameter< ParameterType::eName >() } );
		context.addPushParser( cuT( "sampler" ), CSCNSection::eSampler, eng::parserSamplerState, { makeParameter< ParameterType::eName >() } );
		context.addPushParser( cuT( "window" ), CSCNSection::eWindow, eng::parserWindow, { makeParameter< ParameterType::eName >() } );
	}

	castor::String Engine::getDefaultLightingModelName()const
	{
		return getLightingModelFactory().getBaseName( getDefaultLightingModel() );
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

	castor3d::MaterialObs Engine::getDefaultMaterial()const
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

	ControlsManager * Engine::getControlsManager()const
	{
		return m_userInputListener
			? &static_cast< ControlsManager & >( *m_userInputListener )
			: nullptr;
	}

	UploadData & Engine::getUploadData()const noexcept
	{
		return m_renderLoop->getUploadData();
	}

	ast::ShaderAllocator & Engine::getShaderAllocator()
	{
		auto lock = castor::makeUniqueLock( m_allocMutex );
		auto [it, res] = m_shaderAllocators.try_emplace( std::this_thread::get_id() );

		if ( res )
		{
			it->second = castor::make_unique< ast::ShaderAllocator >( ast::AllocationMode::eFragmented );
		}

		return *it->second;
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

	void Engine::registerTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		if ( hasRenderLoop() )
		{
			m_renderLoop->registerTimer( category, timer );
		}
	}

	void Engine::unregisterTimer( castor::String const & category
		, FramePassTimer & timer )noexcept
	{
		if ( hasRenderLoop() )
		{
			m_renderLoop->unregisterTimer( category, timer );
		}
	}

	void Engine::registerLightingModel( castor::String const & baseName
		, shader::LightingModelDesc const & defaultDesc
		, shader::LightingModelCreator const & creator )const
	{
		getLightingModelFactory().registerType( baseName
			, defaultDesc
			, creator );
	}

	void Engine::unregisterLightingModel( castor::String const & baseName )const
	{
		getLightingModelFactory().unregisterType( baseName );
	}

	void Engine::registerDiffuseBrdf( shader::DiffuseBrdfDesc const & desc )const
	{
		getLightingModelFactory().registerDiffuseBrdf( desc );
	}

	void Engine::unregisterDiffuseBrdf( castor::String const & name )const
	{
		getLightingModelFactory().unregisterDiffuseBrdf( name );
	}

	void Engine::registerSpecularBrdf( shader::SpecularBrdfDesc const & desc )const
	{
		getLightingModelFactory().registerSpecularBrdf( desc );
	}

	void Engine::unregisterSpecularBrdf( castor::String const & name )const
	{
		getLightingModelFactory().unregisterSpecularBrdf( name );
	}

	void Engine::registerSheenBrdf( shader::SheenBrdfDesc const & desc )const
	{
		getLightingModelFactory().registerSheenBrdf( desc );
	}

	void Engine::unregisterSheenBrdf( castor::String const & name )const
	{
		getLightingModelFactory().unregisterSheenBrdf( name );
	}

	void Engine::registerClearcoatBrdf( shader::ClearcoatBrdfDesc const & desc )const
	{
		getLightingModelFactory().registerClearcoatBrdf( desc );
	}

	void Engine::unregisterClearcoatBrdf( castor::String const & name )const
	{
		getLightingModelFactory().unregisterClearcoatBrdf( name );
	}

	void Engine::registerScatteringModel( shader::ScatteringModelDesc const & desc )const
	{
		getLightingModelFactory().registerScatteringModel( desc );
	}

	void Engine::unregisterScatteringModel( castor::String const & name )const
	{
		getLightingModelFactory().unregisterScatteringModel( name );
	}

	BackgroundModelID Engine::registerBackgroundModel( castor::String const & name
		, shader::BackgroundModelCreator creator )
	{
		return BackgroundModelID( getBackgroundModelFactory().registerType( name, castor::move( creator ) ).id );
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

	void Engine::registerPassModel( PassRegisterInfo const & info )const
	{
		getPassFactory().registerType( info );
	}

	void Engine::unregisterPassModel( castor::String const & baseName )const
	{
		getPassFactory().unregisterType( baseName );
	}

	void Engine::registerSpecificsBuffer( castor::String const & name
		, SpecificsBuffer buffer )const
	{
		m_materialCache->registerSpecificsBuffer( name, castor::move( buffer ) );
	}

	void Engine::unregisterSpecificsBuffer( castor::String const & name )const
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
		, castor::StringMap< shader::BufferBaseUPtr > & buffers
		, uint32_t & binding
		, uint32_t set )const
	{
		m_materialCache->declareSpecificsShaderBuffers( writer, buffers, binding, set );
	}

	PassComponentID Engine::registerPassComponent( castor::String const & type
		, PassComponentPluginUPtr componentPlugin )const
	{
		return m_passComponents->registerComponent( type
			, castor::move( componentPlugin ) );
	}

	void Engine::unregisterPassComponent( castor::String const & type )const
	{
		m_passComponents->unregisterComponent( type );
	}

	SubmeshComponentID Engine::registerSubmeshComponent( castor::String const & type
		, SubmeshComponentPluginUPtr componentPlugin )const
	{
		return m_submeshComponents->registerComponent( type
			, castor::move( componentPlugin ) );
	}

	void Engine::unregisterSubmeshComponent( castor::String const & type )const
	{
		m_submeshComponents->unregisterComponent( type );
	}

	void Engine::registerRenderPassType( castor::String const & renderPassType
		, castor::UniquePtr< RenderPassRegisterInfo > info )
	{
		if ( info )
		{
			auto & ninfo = *m_passRenderPassTypes.try_emplace( renderPassType, castor::move( info ) ).first->second;
			auto [it, res] = m_renderPassTypes.try_emplace( renderPassType );

			if ( res )
			{
				it->second.first = RenderPassTypeID( m_renderPassTypes.size() );
			}

			ninfo.id = it->second.first;
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
			it->second.second = castor::move( parameters );
		}
	}

	Parameters Engine::getRenderPassTypeConfiguration( castor::String const & renderPassType )const
	{
		if ( auto it = m_renderPassTypes.find( renderPassType );
			it != m_renderPassTypes.end() )
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

	castor::Vector< RenderPassRegisterInfo * > Engine::getRenderPassInfos( TechniquePassEvent event )const
	{
		castor::Vector< RenderPassRegisterInfo * > result;
		castor::Set< castor::String > inserted;

		for ( auto const & [_, renderPass] : m_passRenderPassTypes )
		{
			if ( renderPass->event == event
				&& inserted.insert( renderPass->name ).second )
			{
				result.push_back( renderPass.get() );
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

	std::default_random_engine Engine::createRandomEngine()const
	{
		if ( !isRandomisationEnabled() )
		{
			return std::default_random_engine{};
		}

		std::random_device r;
		return std::default_random_engine{ r() };
	}

	bool Engine::isCleaned()const noexcept
	{
		return m_cleaned;
	}

	void Engine::setCleaned()noexcept
	{
		m_cleaned = true;
	}

	void Engine::registerWindow( RenderWindow & window )
	{
#if !defined( NDEBUG )
		auto result = m_renderWindows.try_emplace( window.getName(), &window ).second;
		CU_Assert( result, "Duplicate window." );
#else
		m_renderWindows.emplace( window.getName(), &window );
#endif
		m_windowInputListeners.try_emplace( &window
			, castor::makeUniqueDerived< UserInputListener, RenderWindow::InputListener >( *this, window ) );
		auto listener = m_windowInputListeners.find( &window )->second.get();
		log::trace << "Created InputListener [0x" << std::hex << listener << "] - " << window.getName() << std::endl;
	}

	void Engine::unregisterWindow( RenderWindow const & window )
	{
		if ( auto it = m_windowInputListeners.find( &window );
			it != m_windowInputListeners.end() )
		{
			auto listener = m_windowInputListeners.find( &window )->second.get();
			listener->cleanup();
			auto pwindow = &window;
			log::trace << "Removing InputListener [0x" << std::hex << listener << "]" << std::endl;
			listener->getFrameListener().postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
				, [this, listener, pwindow]()
				{
					m_windowInputListeners.erase( pwindow );
					log::trace << "Removed InputListener [0x" << std::hex << listener << "]" << std::endl;
				} ) );
		}

		m_renderWindows.erase( window.getName() );
	}

	void Engine::registerParsers( castor::String name
		, castor::AttributeParsers parsers
		, castor::StrUInt32Map sections
		, castor::UserContextCreator contextCreator )
	{
		if ( auto it = m_additionalParsers.find( name );
			it != m_additionalParsers.end() )
		{
			CU_Exception( "registerParsers - Duplicate entry for " + castor::toUtf8( name ) );
		}

		m_additionalParsers.try_emplace( castor::move( name )
			, castor::move( parsers )
			, castor::move( sections )
			, castor::move( contextCreator ) );
	}

	void Engine::unregisterParsers( castor::String const & name )noexcept
	{
		auto && it = m_additionalParsers.find( name );

		if ( it == m_additionalParsers.end() )
		{
			CU_Failure( "unregisterParsers - Unregistered entry " + name );
			return;
		}

		m_additionalParsers.erase( it );
	}

	void Engine::pushCpuJob( castor::AsyncJobQueue::Job job )
	{
		m_cpuJobs.pushJob( castor::move( job ) );
	}

	void Engine::setLoadingScene( SceneUPtr scene )
	{
		bool hadLoadingScene = false;

		if ( m_loadingScene )
		{
			for ( auto const & [_, window] : m_renderWindows )
			{
				window->destroyLoadingScreen();
			}

			m_loadingScene->cleanup();
			auto pending = m_loadingScene.release();
			hadLoadingScene = true;
			postEvent( makeCpuFunctorEvent( CpuEventType::ePreGpuStep
				, [pending]()
				{
					delete pending;
				} ) );
		}

		m_loadingScene = castor::move( scene );
		m_loadingScene->initialise();

		if ( hadLoadingScene )
		{
			for ( auto const & [_, window] : m_renderWindows )
			{
				window->createLoadingScreen();
			}
		}
	}

	void Engine::doLoadCoreData()
	{
		if ( castor::Path path = Engine::getDataDirectory() / cuT( "Castor3D" );
			castor::File::fileExists( path / cuT( "Core.zip" ) ) )
		{
			if ( SceneFileParser parser( *this );
				!parser.parseFile( cuT( "Castor3D" ), path / cuT( "Core.zip" ) ) )
			{
				log::error << cuT( "Can't read Core.zip data file" ) << std::endl;
			}

			{
				auto lock( castor::makeUniqueLock( getMaterialCache() ) );

				for ( auto const & [_, material] : getMaterialCache() )
				{
					material->setSerialisable( false );
				}

				for ( auto const & [_, texture] : getTextureUnitCache() )
				{
					texture->sourceInfo.setSerialisable( false );
				}
			}

			{
				auto lock( castor::makeUniqueLock( m_fontCache ) );

				for ( auto const & [_, font] : m_fontCache )
				{
					font->setSerialisable( false );
				}
			}
		}
		
		auto const & device = m_renderSystem->getRenderDevice();
		m_brdf = eng::doCreatePrefilteredBrdf( *this
			, device
			, m_resources
			, { PrefilteredBrdfMapSize, PrefilteredBrdfMapSize } );
		m_brdf.create();
	}
}

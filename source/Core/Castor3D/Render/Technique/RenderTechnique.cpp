#include "Castor3D/Render/Technique/RenderTechnique.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/BackgroundPass.hpp"
#include "Castor3D/Render/Passes/ComputeDepthRange.hpp"
#include "Castor3D/Render/Passes/DepthPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/SsaoPass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/Voxelizer.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/FramePassTimer.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderTechnique )

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
#if C3D_UseWeightedBlendedRendering
		using TransparentPassType = TransparentPass;
#else
		using TransparentPassType = ForwardRenderTechniquePass;
#endif
#if C3D_UseDeferredRendering
		using OpaquePassType = OpaquePass;
#else
		using OpaquePassType = ForwardRenderTechniquePass;
#endif

		std::map< double, LightRPtr > doSortLights( LightCache const & cache
			, LightType type
			, Camera const & camera )
		{
			auto lock( castor::makeUniqueLock( cache ) );
			std::map< double, LightRPtr > lights;

			for ( auto & light : cache.getLights( type ) )
			{
				light->setShadowMap( nullptr );

				if ( light->isShadowProducer()
					&& ( light->getLightType() == LightType::eDirectional
						|| camera.isVisible( light->getBoundingBox()
							, light->getParent()->getDerivedTransformationMatrix() ) ) )
				{
					lights.emplace( castor::point::distanceSquared( camera.getParent()->getDerivedPosition()
							, light->getParent()->getDerivedPosition() )
						, light );
				}
			}

			return lights;
		}

		void doPrepareShadowMap( LightCache const & cache
			, LightType type
			, ShadowMap & shadowMap
			, ShadowMapLightTypeArray & activeShadowMaps
			, LightPropagationVolumesLightType const & lightPropagationVolumes
			, LightPropagationVolumesGLightType const & lightPropagationVolumesG
			, LayeredLightPropagationVolumesLightType const & layeredLightPropagationVolumes
			, LayeredLightPropagationVolumesGLightType const & layeredLightPropagationVolumesG
			, CpuUpdater & updater )
		{
			auto lights = doSortLights( cache, type, *updater.camera );
			size_t count = std::min( shadowMap.getCount(), uint32_t( lights.size() ) );

			if ( count > 0 )
			{
				uint32_t index = 0u;
				auto lightIt = lights.begin();
				activeShadowMaps[size_t( type )].emplace_back( std::ref( shadowMap ), UInt32Array{} );
				auto & active = activeShadowMaps[size_t( type )].back();

				for ( auto i = 0u; i < count; ++i )
				{
					auto & light = *lightIt->second;
					light.setShadowMap( &shadowMap, index );
					active.second.push_back( index );
					updater.light = &light;
					updater.index = index;
					shadowMap.update( updater );

					switch ( lightIt->second->getGlobalIlluminationType() )
					{
					case GlobalIlluminationType::eLpv:
						lightPropagationVolumes[size_t( type )]->registerLight( updater.light );
						break;
					case GlobalIlluminationType::eLpvG:
						lightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
						break;
					case GlobalIlluminationType::eLayeredLpv:
						if ( shadowMap.getEngine()->getRenderSystem()->hasLLPV() )
						{
							layeredLightPropagationVolumes[size_t( type )]->registerLight( updater.light );
						}
						else
						{
							lightPropagationVolumes[size_t( type )]->registerLight( updater.light );
						}
						break;
					case GlobalIlluminationType::eLayeredLpvG:
						if ( shadowMap.getEngine()->getRenderSystem()->hasLLPV() )
						{
							layeredLightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
						}
						else
						{
							lightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
						}
						break;
					default:
						break;
					}

					++index;
					++lightIt;
				}
			}
		}

		TextureLayoutSPtr doCreateTexture( Engine & engine
			, RenderDevice const & device
			, VkExtent3D const & size
			, VkFormat format
			, VkImageUsageFlags usage
			, castor::String debugName )
		{
			ashes::ImageCreateInfo image{ 0u
				, VK_IMAGE_TYPE_2D
				, format
				, size
				, 1u
				, 1u
				, VK_SAMPLE_COUNT_1_BIT
				, VK_IMAGE_TILING_OPTIMAL
				, usage | VK_IMAGE_USAGE_SAMPLED_BIT };
			auto result = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, std::move( debugName ) );
			return result;
		}

		TextureUnitUPtr doCreateTextureUnit( RenderDevice const & device
			, QueueData const & queueData
			, crg::ImageId const & image )
		{
			VkImageSubresourceRange range{ 0u, 0u, 1u, 0u, 1u };
			auto & engine = *device.renderSystem.getEngine();
			auto colourTexture = doCreateTexture( engine
				, device
				, image.data->info.extent
				, image.data->info.format
				, image.data->info.usage
				, image.data->name );
			auto sampler = createSampler( engine
				, image.data->name
				, VK_FILTER_LINEAR
				, &range );
			auto result = castor::makeUnique< TextureUnit >( engine
				, TextureSourceInfo{ sampler.lock(), colourTexture->getCreateInfo() } );
			result->setTexture( colourTexture );
			result->setSampler( sampler );
			result->initialise( device, queueData );
			return result;
		}

		LightVolumePassResultArray doCreateLLPVResult( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & prefix )
		{
			LightVolumePassResultArray result;
			auto & engine = *device.renderSystem.getEngine();

			for ( uint32_t i = 0u; i < shader::LpvMaxCascadesCount; ++i )
			{
				result.emplace_back( castor::makeUnique< LightVolumePassResult >( handler
					, device
					, prefix + castor::string::toString( i )
					, engine.getLpvGridSize() ) );
			}

			return result;
		}

		crg::FrameGraph doCreateClearLpvCommands( crg::ResourceHandler & handler
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, LightVolumePassResult  const & lpvResult
			, LightVolumePassResultArray const & llpvResult )
		{
			class LpvClear
				: public crg::RunnablePass
			{
			public:
				LpvClear( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
					: crg::RunnablePass{ pass
						, context
						, graph
						, { [](){}
							, GetSemaphoreWaitFlagsCallback( [](){ return VK_PIPELINE_STAGE_TRANSFER_BIT; } )
							, [this]( VkCommandBuffer cb, uint32_t i ){ doRecordInto( cb, i ); } } }
				{
				}

			protected:
				void doRecordInto( VkCommandBuffer commandBuffer
					, uint32_t index )
				{
					auto clearValue = transparentBlackClearColor.color;

					for ( auto & attach : m_pass.images )
					{
						auto view = attach.view();
						auto image = m_graph.createImage( view.data->image );
						assert( attach.isTransferOutputView() );
						m_context.vkCmdClearColorImage( commandBuffer
							, image
							, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
							, &clearValue
							, 1u
							, &view.data->info.subresourceRange );
					}
				}
			};

			stepProgressBar( progress, "Creating clear LPV commands" );
			crg::FrameGraph result{ handler, name + "ClearLpv" };
			auto & pass = result.createPass( name + "LpvClear"
				, [progress]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					stepProgressBar( progress, "Initialising clear LPV commands" );
					return std::make_unique< LpvClear >( framePass
						, context
						, runnableGraph );
				} );

			for ( auto & texture : lpvResult )
			{
				pass.addTransferOutputView( texture->wholeViewId
					, VK_IMAGE_LAYOUT_UNDEFINED );
			}

			for ( auto & textures : llpvResult )
			{
				for ( auto & texture : *textures )
				{
					pass.addTransferOutputView( texture->wholeViewId
						, VK_IMAGE_LAYOUT_UNDEFINED );
				}
			}

			return result;
		}
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( castor::String const & name
		, RenderTarget & renderTarget
		, RenderDevice const & device
		, QueueData const & queueData
		, Parameters const & parameters
		, SsaoConfig const & ssaoConfig
		, ProgressBar * progress )
		: castor::OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, castor::Named{ name }
		, m_renderTarget{ renderTarget }
		, m_device{ device }
		, m_targetSize{ m_renderTarget.getSize() }
		, m_rawSize{ getSafeBandedSize( m_targetSize ) }
		, m_colour{ device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechCol"
			, 0u
			, makeExtent3D( m_rawSize )
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_colourTexture{ doCreateTextureUnit( m_device
			, queueData
			, m_colour.imageId ) }
		, m_depth{ std::make_shared< Texture >( device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechDpt"
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
				| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK ) }
		, m_depthBuffer{ doCreateTextureUnit( m_device
			, queueData
			, m_depth->imageId ) }
		, m_depthObj{ std::make_shared< Texture >( device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechData0"
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, getFormat( DsTexture::eData0 )
			, getUsageFlags( DsTexture::eData0 )
			, getBorderColor( DsTexture::eData0 ) ) }
		, m_normal{ std::make_shared< Texture >( device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechData1"
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, getFormat( DsTexture::eData1 )
			, getUsageFlags( DsTexture::eData1 )
			, getBorderColor( DsTexture::eData1 ) ) }
		, m_matrixUbo{ m_device }
		, m_sceneUbo{ m_device }
		, m_gpInfoUbo{ m_device }
		, m_lpvConfigUbo{ m_device }
		, m_llpvConfigUbo{ m_device }
		, m_vctConfigUbo{ m_device }
		, m_depthPassDecl{ &doCreateDepthPass( progress ) }
		, m_depthRange{ makeBuffer< int32_t >( device
			, 2u
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, getName() + "DepthRange" ) }
		, m_computeDepthRangeDesc{ &doCreateComputeDepthRange( progress ) }
		, m_ssao{ castor::makeUnique< SsaoPass >( m_renderTarget.getGraph()
			, device
			, progress
			, *m_depthPassDecl
			, makeSize( m_colour.getExtent() )
			, getSsaoConfig()
			, *m_depth
			, *m_normal
			, m_gpInfoUbo ) }
		, m_voxelizer{ castor::makeUnique< Voxelizer >( getOwner()->getGraphResourceHandler()
			, device
			, progress
			, *m_renderTarget.getScene()
			, *m_renderTarget.getCamera()
			, m_matrixUbo
			, m_vctConfigUbo
			, m_renderTarget.getScene()->getVoxelConeTracingConfig() ) }
		, m_lpvResult{ castor::makeUnique< LightVolumePassResult >( getOwner()->getGraphResourceHandler()
			, m_device
			, getName()
			, getEngine()->getLpvGridSize() ) }
		, m_llpvResult{ doCreateLLPVResult( getOwner()->getGraphResourceHandler()
			, m_device
			, getName() ) }
		, m_backgroundRenderer{ castor::makeUnique< BackgroundRenderer >( m_renderTarget.getGraph()
			, m_computeDepthRangeDesc
			, m_device
			, progress
			, name
			, *m_renderTarget.getScene()->getBackground()
			, m_renderTarget.getHdrConfigUbo()
			, m_sceneUbo
			, m_colour.targetViewId
			, true
			, m_depth->targetViewId ) }
#if !C3D_DebugDisableShadowMaps
		, m_directionalShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapDirectional >( getOwner()->getGraphResourceHandler()
			, m_device
			, *m_renderTarget.getScene()
			, progress ) }
		, m_pointShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapPoint >( getOwner()->getGraphResourceHandler()
			, m_device
			, *m_renderTarget.getScene()
			, progress ) }
		, m_spotShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapSpot >( getOwner()->getGraphResourceHandler()
			, m_device
			, *m_renderTarget.getScene()
			, progress ) }
#endif
#if C3D_UseDeferredRendering
		, m_opaquePassResult{ castor::makeUnique< OpaquePassResult >( getOwner()->getGraphResourceHandler()
			, device
			, m_depthObj
			, m_normal
			, m_renderTarget.getVelocity() ) }
		, m_opaquePassDesc{ &doCreateOpaquePass( progress ) }
		, m_deferredRendering{ castor::makeUnique< DeferredRendering >( m_renderTarget.getGraph()
			, *m_opaquePassDesc
			, m_device
			, progress
			, *m_depth
			, *m_opaquePassResult
			, m_colour
			, m_directionalShadowMap->getShadowPassResult()
			, m_pointShadowMap->getShadowPassResult()
			, m_spotShadowMap->getShadowPassResult()
			, *m_lpvResult
			, m_llpvResult
			, m_voxelizer->getFirstBounce()
			, m_voxelizer->getSecondaryBounce()
			, m_ssao->getResult()
			, m_rawSize
			, *m_renderTarget.getScene()
			, m_sceneUbo
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_vctConfigUbo
			, getSsaoConfig() ) }
#else
		, m_opaquePassDesc{ &doCreateOpaquePass( progress ) }
#endif
#if C3D_UseWeightedBlendedRendering
		, m_transparentPassResult{ castor::makeUnique< TransparentPassResult >( getOwner()->getGraphResourceHandler()
			, m_device
			, m_depth
			, m_renderTarget.getVelocity() ) }
		, m_transparentPassDesc{ &doCreateTransparentPass( progress ) }
		, m_weightedBlendRendering{ castor::makeUnique< WeightedBlendRendering >( m_renderTarget.getGraph()
			, m_device
			, progress
			, *m_transparentPassDesc
			, *m_transparentPassResult
			, m_colour.wholeViewId
			, m_rawSize
			, m_sceneUbo
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo )}
#else
		, m_transparentPassDesc{ &doCreateTransparentPass( progress ) }
#endif
		, m_signalFinished{ m_device->createSemaphore( "RenderTechnique" ) }
		, m_clearLpvGraph{ doCreateClearLpvCommands( getOwner()->getGraphResourceHandler(), device, progress, getName(), *m_lpvResult, m_llpvResult ) }
		, m_clearLpvRunnable{ m_clearLpvGraph.compile( m_device.makeContext() ) }
		, m_particleTimer{ castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Particles" ) ) }
	{
		m_colour.create();
		m_depth->create();
		auto runnable = m_clearLpvRunnable.get();
		m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [runnable]( RenderDevice const &
				, QueueData const & )
			{
				runnable->record();
			} ) );
#if C3D_UseWeightedBlendedRendering
		m_transparentPassResult->create();
#endif
#if !C3D_DebugDisableShadowMaps
		m_allShadowMaps[size_t( LightType::eDirectional )].emplace_back( std::ref( *m_directionalShadowMap ), UInt32Array{} );
		m_allShadowMaps[size_t( LightType::eSpot )].emplace_back( std::ref( *m_spotShadowMap ), UInt32Array{} );
		m_allShadowMaps[size_t( LightType::ePoint )].emplace_back( std::ref( *m_pointShadowMap ), UInt32Array{} );
		doInitialiseLpv();
#endif
	}

	RenderTechnique::~RenderTechnique()
	{
		m_particleTimer.reset();
#if C3D_UseWeightedBlendedRendering
		m_weightedBlendRendering.reset();
		m_transparentPassResult.reset();
#endif
#if C3D_UseDeferredRendering
		m_deferredRendering.reset();
		m_opaquePassResult.reset();
#endif
		m_llpvResult.clear();
		m_lpvResult.reset();
		m_voxelizer.reset();
		m_colour.destroy();
		m_depth->destroy();
		m_depthBuffer->cleanup();
		m_colourTexture->cleanup();
		getEngine()->getSamplerCache().remove( cuT( "RenderTechnique_Colour" ) );
		getEngine()->getSamplerCache().remove( cuT( "RenderTechnique_Depth" ) );
		m_signalFinished.reset();

		for ( auto & array : m_activeShadowMaps )
		{
			array.clear();
		}

		m_directionalShadowMap.reset();
		m_pointShadowMap.reset();
		m_spotShadowMap.reset();
	}

	uint32_t RenderTechnique::countInitialisationSteps()
	{
		uint32_t result = 0u;
		result += 2;// m_depthPass;
		result += 2;// m_computeDepthRange;
		result += SsaoPass::countInitialisationSteps();
		result += Voxelizer::countInitialisationSteps();
		result += 2;// m_backgroundRenderer;
		++result;// m_directionalShadowMap;
		++result;// m_pointShadowMap;
		++result;// m_spotShadowMap;
		result += 2;// m_opaquePass;
		result += DeferredRendering::countInitialisationSteps();
		result += 2;// m_transparentPass;
		result += WeightedBlendRendering::countInitialisationSteps();
		result += 2;// m_clearLpv;
		result += LightPropagationVolumes::countInitialisationSteps();
		result += LightPropagationVolumesG::countInitialisationSteps();
		result += LayeredLightPropagationVolumes::countInitialisationSteps();
		result += LayeredLightPropagationVolumesG::countInitialisationSteps();
		return result;
	}

	void RenderTechnique::update( CpuUpdater & updater )
	{
		if ( !m_depthPass )
		{
			return;
		}

		auto & scene = *m_renderTarget.getScene();
		auto & camera = *m_renderTarget.getCamera();
		updater.scene = &scene;
		updater.camera = &camera;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

		if ( getSsaoConfig().enabled )
		{
			m_ssao->update( updater );
		}

		doUpdateShadowMaps( updater );
		doUpdateLpv( updater );
		doUpdateParticles( updater );

		if ( m_renderTarget.getTargetType() == TargetType::eWindow )
		{
			scene.getEnvironmentMap().update( updater );
		}

		m_depthPass->update( updater );
		m_backgroundRenderer->update( updater );

		if ( updater.voxelConeTracing )
		{
			m_voxelizer->update( updater );
		}

		if ( m_opaquePass )
		{
			static_cast< OpaquePassType & >( *m_opaquePass ).update( updater );
		}

		if ( m_transparentPass )
		{
			static_cast< TransparentPassType & >( *m_transparentPass ).update( updater );
		}

#if C3D_UseDeferredRendering
		m_deferredRendering->update( updater );
#endif

		auto jitter = m_renderTarget.getJitter();
		auto jitterProjSpace = jitter * 2.0f;
		jitterProjSpace[0] /= float( camera.getWidth() );
		jitterProjSpace[1] /= float( camera.getHeight() );
		m_matrixUbo.cpuUpdate( camera.getView()
			, camera.getProjection( true )
			, jitterProjSpace );
		m_sceneUbo.cpuUpdate( &camera
			, camera.getScene()->getFog() );
		m_gpInfoUbo.cpuUpdate( makeSize( m_colour.getExtent() )
			, camera );
	}

	void RenderTechnique::update( GpuUpdater & updater )
	{
		if ( !m_depthPass )
		{
			return;
		}

		auto pscene = m_renderTarget.getScene();

		if ( !pscene )
		{
			CU_Exception( "No scene set for RenderTarget." );
		}

		auto & scene = *pscene;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

		doInitialiseLpv();
		doUpdateShadowMaps( updater );
		doUpdateLpv( updater );
		doUpdateParticles( updater );

		if ( m_renderTarget.getTargetType() == TargetType::eWindow )
		{
			scene.getEnvironmentMap().update( updater );
		}

		m_depthPass->update( updater );
		m_backgroundRenderer->update( updater );

		if ( updater.voxelConeTracing )
		{
			m_voxelizer->update( updater );
		}

		if ( m_opaquePass )
		{
			static_cast< OpaquePassType & >( *m_opaquePass ).update( updater );
		}

		if ( m_transparentPass )
		{
			static_cast< TransparentPassType & >( *m_transparentPass ).update( updater );
		}
	}

	crg::SemaphoreWait RenderTechnique::preRender( crg::SemaphoreWait const & toWait
		, ashes::Queue const & queue )
	{
		auto result = toWait;
		result = doRenderShadowMaps( result, queue );
		result = doRenderLPV( result, queue );
		result = doRenderEnvironmentMaps( result, queue );
		result = doRenderVCT( result, queue );
		return result;
	}

	bool RenderTechnique::writeInto( castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Technique Colour"
			, m_colour
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Technique Depth"
			, *m_depth
			, m_renderTarget.getGraph().getFinalLayout( m_depth->sampledViewId ).layout
			, TextureFactors{}.invert( true ) );

		m_depthPass->accept( visitor );
		m_voxelizer->accept( visitor );
		m_ssao->accept( visitor );

		if ( checkFlag( visitor.getFlags().passFlags, PassFlag::eAlphaBlending ) )
		{
			if ( m_transparentPass )
			{
				m_transparentPass->accept( visitor );
			}

#if C3D_UseWeightedBlendedRendering
			if ( m_weightedBlendRendering )
			{
				m_weightedBlendRendering->accept( visitor );
			}
#endif
		}
		else
		{
			if ( m_opaquePass )
			{
				m_opaquePass->accept( visitor );
			}

#if C3D_UseDeferredRendering
			if ( m_deferredRendering )
			{
				m_deferredRendering->accept( visitor );
			}
#endif
		}

#if !C3D_DebugDisableShadowMaps
		m_directionalShadowMap->accept( visitor );
		m_spotShadowMap->accept( visitor );
		m_pointShadowMap->accept( visitor );
#endif

		for ( auto & lpv : m_lightPropagationVolumes )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}

		for ( auto & lpv : m_lightPropagationVolumesG )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}

		for ( auto & lpv : m_layeredLightPropagationVolumes )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}

		for ( auto & lpv : m_layeredLightPropagationVolumesG )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}
	}

	crg::FramePass const & RenderTechnique::getLastPass()const
	{
#if C3D_UseWeightedBlendedRendering
		return m_weightedBlendRendering->getLastPass();
#else
		return *m_transparentPassDesc;
#endif
	}

	SsaoConfig const & RenderTechnique::getSsaoConfig()const
	{
		return m_renderTarget.getSsaoConfig();
	}

	SsaoConfig & RenderTechnique::getSsaoConfig()
	{
		return m_renderTarget.getSsaoConfig();
	}

	crg::FramePass & RenderTechnique::doCreateDepthPass( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating depth pass" );
		auto & result = m_renderTarget.getGraph().createPass( "Depth"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising depth pass" );
				auto res = std::make_unique< DepthPass >( framePass
					, context
					, runnableGraph
					, m_device
					, getSsaoConfig()
					, SceneRenderPassDesc{ m_depth->getExtent(), m_matrixUbo, m_renderTarget.getCuller() }.safeBand( true ) );
				m_depthPass = res.get();
				getEngine()->registerTimer( runnableGraph.getName() + "/Depth"
					, res->getTimer() );
				return res;
			} );
		result.addOutputDepthStencilView( m_depth->targetViewId
			, defaultClearDepthStencil );
		result.addOutputColourView( m_depthObj->targetViewId
			, getClearValue( DsTexture::eData0 ) );
		result.addOutputColourView( m_normal->targetViewId
			, getClearValue( DsTexture::eData1 ) );
		result.addOutputColourView( m_renderTarget.getVelocity()->targetViewId );
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateComputeDepthRange( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating compute depth range pass" );
		auto & result = m_renderTarget.getGraph().createPass( "DepthRange"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising compute depth range pass" );
				auto res = std::make_unique< ComputeDepthRange >( framePass
					, context
					, runnableGraph
					, m_device );
				getEngine()->registerTimer( runnableGraph.getName() + "/DepthRange"
					, res->getTimer() );
				return res;
			} );
		result.addDependency( *m_depthPassDecl );
		result.addInputStorageView( m_depthObj->sampledViewId
			, ComputeDepthRange::eInput
			, VK_IMAGE_LAYOUT_UNDEFINED );
		result.addOutputStorageBuffer( { m_depthRange->getBuffer(), "DepthRange" }
			, ComputeDepthRange::eOutput
			, 0u
			, m_depthRange->getBuffer().getSize() );
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateOpaquePass( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating opaque pass" );
#if C3D_UseDeferredRendering
		castor::String name = cuT( "Geometry" );
#else
		castor::String name = cuT( "Forward" );
#endif
		auto & result = m_renderTarget.getGraph().createPass( "Opaque"
			, [this, progress, name]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising opaque pass" );
				auto res = std::make_unique< OpaquePassType >( framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Opaque" )
					, name
					, SceneRenderPassDesc{ m_colour.getExtent(), m_matrixUbo, m_renderTarget.getCuller() }.safeBand( true )
					, RenderTechniquePassDesc{ false, getSsaoConfig() }
						.ssao( m_ssao->getResult() )
						.lpvConfigUbo( m_lpvConfigUbo )
						.llpvConfigUbo( m_llpvConfigUbo )
						.vctConfigUbo( m_vctConfigUbo )
						.lpvResult( *m_lpvResult )
						.llpvResult( m_llpvResult )
						.vctFirstBounce( m_voxelizer->getFirstBounce() )
						.vctSecondaryBounce( m_voxelizer->getSecondaryBounce() ) );
				m_opaquePass = res.get();
				getEngine()->registerTimer( runnableGraph.getName() + "/Opaque"
					, res->getTimer() );
				return res;
			} );
		result.addDependency( m_backgroundRenderer->getPass() );
		result.addDependency( m_ssao->getLastPass() );
		result.addSampledView( m_ssao->getResult().sampledViewId, 0u, VK_IMAGE_LAYOUT_UNDEFINED );
		result.addInOutDepthStencilView( m_depth->targetViewId );
#if C3D_UseDeferredRendering
		auto & opaquePassResult = *m_opaquePassResult;
		result.addOutputColourView( opaquePassResult[DsTexture::eData2].targetViewId
			, getClearValue( DsTexture::eData2 ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eData3].targetViewId
			, getClearValue( DsTexture::eData3 ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eData4].targetViewId
			, getClearValue( DsTexture::eData4 ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eData5].targetViewId
			, getClearValue( DsTexture::eData5 ) );
#else
		result.addInOutColourView( m_colour.targetViewId );
#endif
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateTransparentPass( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating transparent pass" );
		auto & result = m_renderTarget.getGraph().createPass( "Transparent"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising transparent pass" );
#if C3D_UseWeightedBlendedRendering
				castor::String name = cuT( "Accumulation" );
				static constexpr bool isOit = true;
				static constexpr bool hasVelocity = true;
#else
				castor::String name = cuT( "Forward" );
				static constexpr bool isOit = false;
				static constexpr bool hasVelocity = false;
#endif
				auto res = std::make_unique< TransparentPassType >( framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Transparent" )
					, name
					, SceneRenderPassDesc{ m_colour.getExtent(), m_matrixUbo, m_renderTarget.getCuller(), isOit }.safeBand( true )
					, RenderTechniquePassDesc{ false, getSsaoConfig() }
						.ssao( m_ssao->getResult() )
						.lpvConfigUbo( m_lpvConfigUbo )
						.lpvConfigUbo( m_lpvConfigUbo )
						.llpvConfigUbo( m_llpvConfigUbo )
						.vctConfigUbo( m_vctConfigUbo )
						.lpvResult( *m_lpvResult )
						.llpvResult( m_llpvResult )
						.vctFirstBounce( m_voxelizer->getFirstBounce() )
						.vctSecondaryBounce( m_voxelizer->getSecondaryBounce() )
						.hasVelocity( hasVelocity ) );
				m_transparentPass = res.get();
				getEngine()->registerTimer( runnableGraph.getName() + "/Transparent"
					, res->getTimer() );
				return res;
			} );
#if C3D_UseDeferredRendering
		result.addDependency( m_deferredRendering->getLastPass() );
		result.addInOutDepthStencilView( m_depth->targetViewId, {} );
#else
		result.addDependency( *m_opaquePassDesc );
		result.addInOutDepthStencilView( m_depth->targetViewId );
#endif

#if C3D_UseWeightedBlendedRendering
		auto & transparentPassResult = *m_transparentPassResult;
		result.addOutputColourView( transparentPassResult[WbTexture::eAccumulation].targetViewId
			, getClearValue( WbTexture::eAccumulation ) );
		result.addOutputColourView( transparentPassResult[WbTexture::eRevealage].targetViewId
			, getClearValue( WbTexture::eRevealage ) );
		result.addInOutColourView( transparentPassResult[WbTexture::eVelocity].targetViewId );
#else
		result.addInOutColourView( m_colour.targetViewId );
#endif

		return result;
	}

	void RenderTechnique::doInitialiseLpv()
	{
		auto & scene = *m_renderTarget.getScene();

		for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
		{
			auto needLpv = scene.needsGlobalIllumination( LightType( i )
				, GlobalIlluminationType::eLpv );
			auto needLpvG = scene.needsGlobalIllumination( LightType( i )
				, GlobalIlluminationType::eLpvG );
			auto needLLpv = scene.needsGlobalIllumination( LightType( i )
				, GlobalIlluminationType::eLayeredLpv );
			auto needLLpvG = scene.needsGlobalIllumination( LightType( i )
				, GlobalIlluminationType::eLayeredLpvG );

			if ( needLpv || needLpvG )
			{
				m_lpvResult->create();
			}

			if ( needLLpv || needLLpvG )
			{
				for ( auto & lpvResult : m_llpvResult )
				{
					lpvResult->create();
				}
			}

			if ( needLpv && !m_lightPropagationVolumes[i] )
			{
				m_lightPropagationVolumes[i] = castor::makeUnique< LightPropagationVolumes >( getOwner()->getGraphResourceHandler()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult()
					, *m_lpvResult
					, m_lpvConfigUbo );
				m_lightPropagationVolumes[i]->initialise();
			}

			if ( needLpvG && !m_lightPropagationVolumesG[i] )
			{
				m_lightPropagationVolumesG[i] = castor::makeUnique< LightPropagationVolumesG >( getOwner()->getGraphResourceHandler()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult()
					, *m_lpvResult
					, m_lpvConfigUbo );
				m_lightPropagationVolumesG[i]->initialise();
			}

			if ( needLLpv && !m_layeredLightPropagationVolumes[i] )
			{
				m_layeredLightPropagationVolumes[i] = castor::makeUnique< LayeredLightPropagationVolumes >( getOwner()->getGraphResourceHandler()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult()
					, m_llpvResult
					, m_llpvConfigUbo );
				m_layeredLightPropagationVolumes[i]->initialise();
			}

			if ( needLLpvG && !m_layeredLightPropagationVolumesG[i] )
			{
				m_layeredLightPropagationVolumesG[i] = castor::makeUnique< LayeredLightPropagationVolumesG >( getOwner()->getGraphResourceHandler()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult()
					, m_llpvResult
					, m_llpvConfigUbo );
				m_layeredLightPropagationVolumesG[i]->initialise();
			}
		}
	}

	void RenderTechnique::doUpdateShadowMaps( CpuUpdater & updater )
	{
#if !C3D_DebugDisableShadowMaps
		auto & scene = *m_renderTarget.getScene();

		if ( scene.hasShadows() )
		{
			for ( auto & array : m_activeShadowMaps )
			{
				array.clear();
			}

			auto & cache = scene.getLightCache();
			doPrepareShadowMap( cache
				, LightType::eDirectional
				, *m_directionalShadowMap
				, m_activeShadowMaps
				, m_lightPropagationVolumes
				, m_lightPropagationVolumesG
				, m_layeredLightPropagationVolumes
				, m_layeredLightPropagationVolumesG
				, updater );
			doPrepareShadowMap( cache
				, LightType::ePoint
				, *m_pointShadowMap
				, m_activeShadowMaps
				, m_lightPropagationVolumes
				, m_lightPropagationVolumesG
				, m_layeredLightPropagationVolumes
				, m_layeredLightPropagationVolumesG
				, updater );
			doPrepareShadowMap( cache
				, LightType::eSpot
				, *m_spotShadowMap
				, m_activeShadowMaps
				, m_lightPropagationVolumes
				, m_lightPropagationVolumesG
				, m_layeredLightPropagationVolumes
				, m_layeredLightPropagationVolumesG
				, updater );
		}
#endif
	}

	void RenderTechnique::doUpdateShadowMaps( GpuUpdater & updater )
	{
		for ( auto & maps : m_activeShadowMaps )
		{
			for ( auto & map : maps )
			{
				for ( auto & id : map.second )
				{
					updater.index = id;
					map.first.get().update( updater );
				}
			}
		}
	}

	void RenderTechnique::doUpdateLpv( CpuUpdater & updater )
	{
		for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
		{
			if ( m_lightPropagationVolumes[i] )
			{
				m_lightPropagationVolumes[i]->update( updater );
			}

			if ( m_lightPropagationVolumesG[i] )
			{
				m_lightPropagationVolumesG[i]->update( updater );
			}

			if ( m_layeredLightPropagationVolumes[i] )
			{
				m_layeredLightPropagationVolumes[i]->update( updater );
			}

			if ( m_layeredLightPropagationVolumesG[i] )
			{
				m_layeredLightPropagationVolumesG[i]->update( updater );
			}
		}
	}

	void RenderTechnique::doUpdateLpv( GpuUpdater & updater )
	{
	}
	
	void RenderTechnique::doUpdateParticles( CpuUpdater & updater )
	{
		auto & cache = updater.camera->getScene()->getParticleSystemCache();
		auto lock( castor::makeUniqueLock( cache ) );
		updater.index = 0u;

		for ( auto & particleSystem : cache )
		{
			particleSystem.second->update( updater );
		}
	}
	
	void RenderTechnique::doUpdateParticles( GpuUpdater & updater )
	{
		auto & cache = updater.scene->getParticleSystemCache();
		auto lock( castor::makeUniqueLock( cache ) );
		auto count = 2u * cache.getObjectCountNoLock();

		if ( m_particleTimer->getCount() < count )
		{
			m_particleTimer->updateCount( count );
		}

		auto timerBlock( m_particleTimer->start() );
		updater.index = 0u;
		updater.timer = m_particleTimer.get();

		for ( auto & particleSystem : cache )
		{
			particleSystem.second->update( updater );
			updater.info.m_particlesCount += particleSystem.second->getParticlesCount();
		}
	}

	crg::SemaphoreWait RenderTechnique::doRenderLPV( crg::SemaphoreWait const & semaphore
		, ashes::Queue const & queue )
	{
		crg::SemaphoreWait result = semaphore;

		if ( m_renderTarget.getScene()->needsGlobalIllumination() )
		{
			result = m_clearLpvRunnable->run( result, queue );

			for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
			{
				if ( m_lightPropagationVolumes[i] )
				{
					result = m_lightPropagationVolumes[i]->render( result, queue );
				}

				if ( m_lightPropagationVolumesG[i] )
				{
					result = m_lightPropagationVolumesG[i]->render( result, queue );
				}

				if ( m_layeredLightPropagationVolumes[i] )
				{
					result = m_layeredLightPropagationVolumes[i]->render( result, queue );
				}

				if ( m_layeredLightPropagationVolumesG[i] )
				{
					result = m_layeredLightPropagationVolumesG[i]->render( result, queue );
				}
			}
		}

		return result;
	}

	crg::SemaphoreWait RenderTechnique::doRenderShadowMaps( crg::SemaphoreWait const & semaphore
		, ashes::Queue const & queue )const
	{
		crg::SemaphoreWait result = semaphore;
		auto & scene = *m_renderTarget.getScene();

		if ( scene.hasShadows() )
		{
			for ( auto & array : m_activeShadowMaps )
			{
				for ( auto & shadowMap : array )
				{
					for ( auto & index : shadowMap.second )
					{
						result = shadowMap.first.get().render( result, queue, index );
					}
				}
			}
		}

		return result;
	}

	crg::SemaphoreWait RenderTechnique::doRenderEnvironmentMaps( crg::SemaphoreWait const & semaphore
		, ashes::Queue const & queue )const
	{
		crg::SemaphoreWait result = semaphore;

		if ( m_renderTarget.getTargetType() == TargetType::eWindow )
		{
			auto scene = m_renderTarget.getScene();

			if ( scene )
			{
				result = scene->getEnvironmentMap().render( result, queue );
			}
		}

		return result;
	}

	crg::SemaphoreWait RenderTechnique::doRenderVCT( crg::SemaphoreWait const & semaphore
		, ashes::Queue const & queue )const
	{
		crg::SemaphoreWait result = semaphore;
		auto scene = m_renderTarget.getScene();

		if ( scene
			&& scene->getVoxelConeTracingConfig().enabled )
		{
			result = m_voxelizer->render( result, queue );
		}

		return result;
	}
}

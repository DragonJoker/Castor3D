#include "Castor3D/Render/Technique/RenderTechnique.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"
#include "Castor3D/Render/Passes/ComputeDepthRange.hpp"
#include "Castor3D/Render/Passes/DepthPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/SsaoPass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"
#include "Castor3D/Render/Technique/Visibility/VisibilityPass.hpp"
#include "Castor3D/Render/Technique/Visibility/VisibilityReorderPass.hpp"
#include "Castor3D/Render/Technique/Visibility/VisibilityResolvePass.hpp"
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

	namespace rendtech
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

		static std::map< double, LightRPtr > doSortLights( LightCache const & cache
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

		static void doPrepareShadowMap( LightCache const & cache
			, LightType type
			, ShadowMap & shadowMap
			, ShadowMapLightArray & activeShadowMaps
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
				activeShadowMaps[size_t( type )].push_back( { std::ref( shadowMap ), LightIdArray{} } );
				auto & active = activeShadowMaps[size_t( type )].back();

				for ( auto i = 0u; i < count; ++i )
				{
					auto & light = *lightIt->second;
					light.setShadowMap( &shadowMap, index );
					active.ids.push_back( { &light, index } );
					updater.light = &light;
					updater.index = index;
					shadowMap.update( updater );

					switch ( lightIt->second->getGlobalIlluminationType() )
					{
					case GlobalIlluminationType::eLpv:
						if ( lightPropagationVolumes[size_t( type )] )
						{
							lightPropagationVolumes[size_t( type )]->registerLight( updater.light );
						}
						break;
					case GlobalIlluminationType::eLpvG:
						if ( lightPropagationVolumesG[size_t( type )] )
						{
							lightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
						}
						break;
					case GlobalIlluminationType::eLayeredLpv:
						if ( shadowMap.getEngine()->getRenderSystem()->hasLLPV() )
						{
							if ( layeredLightPropagationVolumes[size_t( type )] )
							{
								layeredLightPropagationVolumes[size_t( type )]->registerLight( updater.light );
							}
						}
						else if ( lightPropagationVolumes[size_t( type )] )
						{
							lightPropagationVolumes[size_t( type )]->registerLight( updater.light );
						}
						break;
					case GlobalIlluminationType::eLayeredLpvG:
						if ( shadowMap.getEngine()->getRenderSystem()->hasLLPV() )
						{
							if ( layeredLightPropagationVolumesG[size_t( type )] )
							{
								layeredLightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
							}
						}
						else if ( lightPropagationVolumesG[size_t( type )] )
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

		static TextureLayoutSPtr doCreateTexture( Engine & engine
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

		static TextureUnitUPtr doCreateTextureUnit( RenderDevice const & device
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

		static LightVolumePassResultArray doCreateLLPVResult( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & prefix )
		{
			LightVolumePassResultArray result;
			auto & engine = *device.renderSystem.getEngine();

			for ( uint32_t i = 0u; i < LpvMaxCascadesCount; ++i )
			{
				result.emplace_back( castor::makeUnique< LightVolumePassResult >( handler
					, device
					, prefix + castor::string::toString( i )
					, engine.getLpvGridSize() ) );
			}

			return result;
		}

		static crg::FrameGraph doCreateClearLpvCommands( crg::ResourceHandler & handler
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
							, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); } } }
				{
				}

			protected:
				void doRecordInto( crg::RecordContext & context
					, VkCommandBuffer commandBuffer
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
				pass.addTransferOutputView( texture->wholeViewId );
			}

			for ( auto & textures : llpvResult )
			{
				for ( auto & texture : *textures )
				{
					pass.addTransferOutputView( texture->wholeViewId );
				}
			}

			return result;
		}

		template< typename ActionT >
		static void applyAction( TechniquePassVector const & renderPasses
			, ActionT action )
		{
			for ( auto & renderPass : renderPasses )
			{
				action( *renderPass );
			}
		}

		static void countNodes( RenderNodesPass const & renderPass
			, RenderInfo & info )
		{
			for ( auto & pipelineNodes : renderPass.getCuller().getSubmeshNodes( renderPass ) )
			{
				for ( auto & bufferNodes : pipelineNodes.second )
				{
					for ( auto & sidedNode : bufferNodes.second )
					{
						if ( sidedNode.first.visible )
						{
							++info.visibleObjectsCount;
							info.visibleFaceCount += sidedNode.first.node->data.getFaceCount();
							info.visibleVertexCount += sidedNode.first.node->data.getPointsCount();
						}

						++info.totalObjectsCount;
						info.totalFaceCount += sidedNode.first.node->data.getFaceCount();
						info.totalVertexCount += sidedNode.first.node->data.getPointsCount();
					}
				}
			}

			for ( auto & pipelineNodes : renderPass.getCuller().getInstancedSubmeshNodes( renderPass ) )
			{
				for ( auto & bufferNodes : pipelineNodes.second )
				{
					for ( auto & passNodes : bufferNodes.second )
					{
						for ( auto & dataNodes : passNodes.second )
						{
							for ( auto & sidedNode : dataNodes.second )
							{
								if ( sidedNode.first.visible )
								{
									++info.visibleObjectsCount;
									info.visibleFaceCount += sidedNode.first.node->data.getFaceCount();
									info.visibleVertexCount += sidedNode.first.node->data.getPointsCount();
								}

								++info.totalObjectsCount;
								info.totalFaceCount += sidedNode.first.node->data.getFaceCount();
								info.totalVertexCount += sidedNode.first.node->data.getPointsCount();
							}
						}
					}
				}
			}
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
		, m_colour{ m_device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechCol"
			, 0u
			, makeExtent3D( m_rawSize )
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_colourTexture{ rendtech::doCreateTextureUnit( m_device
			, queueData
			, m_colour.imageId ) }
		, m_depth{ std::make_shared< Texture >( m_device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechDpt"
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, m_device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
				| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK ) }
		, m_depthBuffer{ rendtech::doCreateTextureUnit( m_device
			, queueData
			, m_depth->imageId ) }
		, m_depthObj{ std::make_shared< Texture >( m_device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechDeptObj"
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_STORAGE_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE ) }
		, m_normal{ std::make_shared< Texture >( m_device
			, getOwner()->getGraphResourceHandler()
			, getName() + "TechData1"
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, getFormat( m_device, DsTexture::eData1 )
			, getUsageFlags( DsTexture::eData1 )
			, getBorderColor( DsTexture::eData1 ) ) }
		, m_matrixUbo{ m_device }
		, m_sceneUbo{ m_device }
		, m_gpInfoUbo{ m_device }
		, m_lpvConfigUbo{ m_device }
		, m_llpvConfigUbo{ m_device }
		, m_vctConfigUbo{ m_device }
#if C3D_UseDeferredRendering
		, m_opaquePassResult{ castor::makeUnique< OpaquePassResult >( getOwner()->getGraphResourceHandler()
			, m_device
			, m_normal ) }
#	if C3D_UseVisibilityBuffer
		, m_materialsCounts1{ ( ( m_device.hasBindless() && VisibilityResolvePass::useCompute )
			? makeBuffer< uint32_t >( m_device
				, getOwner()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "MaterialsCounts1" )
			: nullptr ) }
		, m_materialsCounts2{ ( ( m_device.hasBindless() && VisibilityResolvePass::useCompute )
			? makeBuffer< uint32_t >( m_device
				, getOwner()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "MaterialsCounts2" )
			: nullptr ) }
		, m_materialsStarts{ ( ( m_device.hasBindless() && VisibilityResolvePass::useCompute )
			? makeBuffer< uint32_t >( m_device
				, getOwner()->getMaxPassTypeCount()
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "MaterialsStarts" )
			: nullptr ) }
		, m_pixelsXY{ ( ( m_device.hasBindless() && VisibilityResolvePass::useCompute )
			? makeBuffer< castor::Point2ui >( m_device
				, m_depth->getExtent().width * m_depth->getExtent().height
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "PixelsXY" )
			: nullptr ) }
		, m_visibilityPipelinesIds{ ( ( m_device.hasBindless() && VisibilityResolvePass::useCompute )
			? castor::makeUnique< ShaderBuffer >( *getEngine()
				, m_device
				, MaxObjectNodesCount * sizeof( uint32_t )
				, "MaterialsPipelinesIds" )
			: nullptr ) }
		, m_visibility{ ( m_device.hasBindless()
			? std::make_shared< Texture >( m_device
				, getOwner()->getGraphResourceHandler()
				, getName() + "TechVisibility"
				, 0u
				, m_colour.getExtent()
				, 1u
				, 1u
				, VK_FORMAT_R32G32_UINT
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_STORAGE_BIT )
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK )
			: nullptr ) }
		, m_visibilityPassDesc{ ( m_device.hasBindless()
			? &doCreateVisibilityPass( progress )
			: nullptr ) }
		, m_visibilityReorder{ ( ( m_device.hasBindless() && VisibilityResolvePass::useCompute )
			? castor::makeUnique< VisibilityReorderPass >( m_renderTarget.getGraph().createPassGroup( "Visibility" )
				, *m_visibilityPassDesc
				, m_device
				, m_visibility->sampledViewId
				, *m_visibilityPipelinesIds.get()
				, *m_materialsCounts1
				, *m_materialsCounts2
				, *m_materialsStarts
				, *m_pixelsXY )
			: nullptr ) }
		, m_visibilityResolveDesc{ ( m_device.hasBindless()
			? &doCreateVisibilityResolve( progress )
			: nullptr ) }
#	endif
#endif
		, m_depthPassDesc{ m_visibilityPassDesc 
			? m_visibilityPassDesc
			: &doCreateDepthPass( progress ) }
		, m_depthRange{ makeBuffer< int32_t >( m_device
			, 2u
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, getName() + "DepthRange" ) }
		, m_computeDepthRangeDesc{ &doCreateComputeDepthRange( progress ) }
		, m_voxelizer{ castor::makeUnique< Voxelizer >( getOwner()->getGraphResourceHandler()
			, m_device
			, progress
			, getName()
			, *m_renderTarget.getScene()
			, *m_renderTarget.getCamera()
			, m_vctConfigUbo
			, m_renderTarget.getScene()->getVoxelConeTracingConfig() ) }
		, m_lpvResult{ castor::makeUnique< LightVolumePassResult >( getOwner()->getGraphResourceHandler()
			, m_device
			, getName()
			, getEngine()->getLpvGridSize() ) }
		, m_llpvResult{ rendtech::doCreateLLPVResult( getOwner()->getGraphResourceHandler()
			, m_device
			, getName() ) }
		, m_backgroundRenderer{ doCreateBackgroundPass( progress ) }
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
		, m_opaquePassDesc{ ( m_visibilityResolveDesc
			? m_visibilityResolveDesc 
			: &doCreateOpaquePass( progress ) ) }
		, m_ssao{ castor::makeUnique< SsaoPass >( m_renderTarget.getGraph()
			, m_device
			, progress
			, ( C3D_UseDeferredRendering
				? *m_opaquePassDesc
				: *m_depthPassDesc )
			, makeSize( m_colour.getExtent() )
			, getSsaoConfig()
			, *m_depth
			, *m_normal
			, m_gpInfoUbo ) }
#if C3D_UseDeferredRendering
		, m_deferredRendering{ castor::makeUnique< DeferredRendering >( m_renderTarget.getGraph().createPassGroup( "Opaque" )
			, crg::FramePassArray{ m_opaquePassDesc, &m_backgroundRenderer->getPass() }
			, m_ssao->getLastPass()
			, m_device
			, progress
			, m_device.renderSystem.getPrefilteredBrdfTexture()
			, *m_depth
			, *m_depthObj
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
#endif
#if C3D_UseWeightedBlendedRendering
		, m_transparentPassResult{ castor::makeUnique< TransparentPassResult >( getOwner()->getGraphResourceHandler()
			, m_device
			, m_depth ) }
		, m_transparentPassDesc{ &doCreateTransparentPass( progress ) }
		, m_weightedBlendRendering{ castor::makeUnique< WeightedBlendRendering >( m_renderTarget.getGraph().createPassGroup( "Transparent" )
			, m_device
			, progress
			, *m_transparentPassDesc
			, *m_transparentPassResult
			, m_colour.wholeViewId
			, m_rawSize
			, m_sceneUbo
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo ) }
#else
		, m_transparentPassDesc{ &doCreateTransparentPass( progress ) }
#endif
		, m_clearLpvGraph{ rendtech::doCreateClearLpvCommands( getOwner()->getGraphResourceHandler(), device, progress, getName(), *m_lpvResult, m_llpvResult ) }
		, m_clearLpvRunnable{ m_clearLpvGraph.compile( m_device.makeContext() ) }
	{
		m_renderTarget.getGraph().addDependency( m_voxelizer->getGraph() );
		doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforePostEffects
#if C3D_UseWeightedBlendedRendering
			, &m_weightedBlendRendering->getLastPass() );
#else
			, m_transparentPassDesc );
#endif

		m_colour.create();
		m_depth->create();
		m_normal->create();
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

		if ( m_visibilityPipelinesIds )
		{
			getEngine()->registerBuffer( *m_visibilityPipelinesIds );
		}
	}

	RenderTechnique::~RenderTechnique()
	{
		if ( m_visibilityPipelinesIds )
		{
			getEngine()->unregisterBuffer( *m_visibilityPipelinesIds );
		}

#if C3D_UseWeightedBlendedRendering
		m_weightedBlendRendering.reset();
		m_transparentPassResult.reset();
#endif
#if C3D_UseDeferredRendering
		m_deferredRendering.reset();
		m_opaquePassResult.reset();
#	if C3D_UseVisibilityBuffer
		m_visibilityReorder.reset();
		m_visibility->destroy();
#	endif
#endif
		m_llpvResult.clear();
		m_lpvResult.reset();
		m_voxelizer.reset();
		m_colour.destroy();
		m_depth->destroy();
		m_depthBuffer->cleanup();
		m_colourTexture->cleanup();
		getEngine()->removeSampler( cuT( "RenderTechnique_Colour" ) );
		getEngine()->removeSampler( cuT( "RenderTechnique_Depth" ) );

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
		if ( !m_depthPass && !m_visibilityPass )
		{
			return;
		}

		auto & scene = *updater.scene;
		auto & camera = *updater.camera;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

		if ( getSsaoConfig().enabled )
		{
			m_ssao->update( updater );
		}

		doUpdateShadowMaps( updater );
		doUpdateLpv( updater );

		if ( m_renderTarget.getTargetType() == TargetType::eWindow )
		{
			scene.getEnvironmentMap().update( updater );
		}

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeDepth )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );

		if ( m_visibilityPass )
		{
			m_visibilityPass->update( updater );
		}
		else
		{
			m_depthPass->update( updater );
		}

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeBackground )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
		m_backgroundRenderer->update( updater );

		if ( updater.voxelConeTracing )
		{
			m_voxelizer->update( updater );
		}

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeOpaque )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );

		if ( m_opaquePass )
		{
			m_opaquePass->update( updater );
		}

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeTransparent )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );

		if ( m_transparentPass )
		{
			static_cast< rendtech::TransparentPassType & >( *m_transparentPass ).update( updater );
		}

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforePostEffects )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );

#if C3D_UseDeferredRendering
		updater.camera = &camera;
		m_deferredRendering->update( updater );
#endif
#if C3D_UseWeightedBlendedRendering
		m_weightedBlendRendering->enable( m_transparentPass->hasNodes() );
#endif

		auto jitter = m_renderTarget.getJitter();
		auto jitterProjSpace = jitter * 2.0f;
		jitterProjSpace[0] /= float( camera.getWidth() );
		jitterProjSpace[1] /= float( camera.getHeight() );
		m_matrixUbo.cpuUpdate( camera.getView()
			, camera.getProjection( true )
			, camera.getFrustum()
			, jitterProjSpace );
		m_sceneUbo.cpuUpdate( scene, &camera );
		m_gpInfoUbo.cpuUpdate( makeSize( m_colour.getExtent() )
			, camera );
	}

	void RenderTechnique::update( GpuUpdater & updater )
	{
		if ( !m_depthPass && !m_visibilityPass )
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

		if ( m_renderTarget.getTargetType() == TargetType::eWindow )
		{
			scene.getEnvironmentMap().update( updater );
		}

#if C3D_UseDeferredRendering
		m_deferredRendering->update( updater );
#endif
		if ( m_visibilityPass )
		{
			rendtech::countNodes( *m_visibilityPass, updater.info );
		}
		else
		{
			rendtech::countNodes( *m_depthPass, updater.info );
		}

		rendtech::countNodes( *m_transparentPass, updater.info );
	}

	crg::SemaphoreWaitArray RenderTechnique::preRender( crg::SemaphoreWaitArray const & toWait
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
			, m_renderTarget.getGraph().getFinalLayoutState( m_colour.sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Technique Depth"
			, *m_depth
			, m_renderTarget.getGraph().getFinalLayoutState( m_depth->sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Technique DepthObj"
			, *m_depthObj
			, m_renderTarget.getGraph().getFinalLayoutState( m_depthObj->sampledViewId ).layout
			, TextureFactors{}.invert( true ) );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeDepth )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );

		if ( m_visibilityPass )
		{
			m_visibilityPass->accept( visitor );
		}
		else
		{
			m_depthPass->accept( visitor );
		}

		m_voxelizer->accept( visitor );
		m_ssao->accept( visitor );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeBackground )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );
		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeOpaque )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );

		if ( !checkFlag( visitor.getFlags().passFlags, PassFlag::eAlphaBlending ) )
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

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeTransparent )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );

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

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforePostEffects )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );

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
		if ( !m_renderPasses[size_t( TechniquePassEvent::eBeforePostEffects )].empty() )
		{
			return m_renderPasses[size_t( TechniquePassEvent::eBeforePostEffects )].back()->getPass();
		}

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

	Texture const & RenderTechnique::getSsaoResult()const
	{
		return m_ssao->getResult();
	}

	Texture const & RenderTechnique::getFirstVctBounce()const
	{
		return m_voxelizer->getFirstBounce();
	}

	Texture const & RenderTechnique::getSecondaryVctBounce()const
	{
		return m_voxelizer->getSecondaryBounce();
	}

	crg::ImageViewId const & RenderTechnique::getLightDepthImgView()const
	{
#if C3D_UseDeferredRendering
		return m_deferredRendering->getLightDepthImgView();
#else
		return getDepthSampledView();
#endif
	}

	TechniquePassVector RenderTechnique::getCustomRenderPasses()const
	{
		TechniquePassVector result;

		for ( auto & passes : m_renderPasses )
		{
			for ( auto pass : passes )
			{
				result.push_back( pass );
			}
		}

		return result;
	}

	Texture const & RenderTechnique::getDiffuseLightingResult()const
	{
		if ( m_deferredRendering )
		{
			return m_deferredRendering->getLightDiffuse();
		}

		return m_colour;
	}

	Texture const & RenderTechnique::getBaseColourResult()const
	{
		if ( m_opaquePassResult )
		{
			m_opaquePassResult->create();
			return ( *m_opaquePassResult )[DsTexture::eData2];
		}

		return m_colour;
	}

	crg::FramePassArray RenderTechnique::doCreateRenderPasses( ProgressBar * progress
		, TechniquePassEvent event
		, crg::FramePass const * previousPass )
	{
		crg::FramePassArray result;

		if ( previousPass )
		{
			result.push_back( previousPass );
		}

		for ( auto renderPassInfo : getEngine()->getRenderPassInfos( event ) )
		{
			auto passes = renderPassInfo->create( m_device
				, *this
				, m_renderPasses
				, result );

			if ( !passes.empty() )
			{
				result = passes;
			}
		}

		return result;
	}

	crg::FramePass & RenderTechnique::doCreateVisibilityPass( ProgressBar * progress )
	{
		auto & previous = m_renderTarget.createVertexTransformPass();
		auto previousPasses = doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforeDepth
			, &previous );
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Visibility" );
		stepProgressBar( progress, "Creating depth pass" );
		auto & result = graph.createPass( "NodesPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.images.begin();
				auto depthObjIt = std::next( depthIt );
				auto dataIt = std::next( depthObjIt );
				auto velocityIt = std::next( dataIt );
				stepProgressBar( progress, "Initialising depth pass" );
				auto res = std::make_unique< VisibilityPass >( this
					, framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Visibility" )
					, cuT( "NodesPass" )
					, RenderNodesPassDesc{ m_depth->getExtent(), m_matrixUbo, m_sceneUbo, m_renderTarget.getCuller() }
						.safeBand( true )
						.meshShading( true )
						.implicitAction( depthIt->view(), crg::RecordContext::clearAttachment( *depthIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *depthObjIt ) )
						.implicitAction( dataIt->view(), crg::RecordContext::clearAttachment( *dataIt ) )
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *velocityIt ) )
					, RenderTechniquePassDesc{ false, getSsaoConfig() } );
				m_visibilityPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addOutputDepthStencilView( m_depth->targetViewId
			, defaultClearDepthStencil );
		result.addOutputColourView( m_depthObj->targetViewId
			, makeClearValue( 1.0f, std::numeric_limits< float >::max(), 0.0f, 0.0f ) );
		result.addOutputColourView( m_visibility->targetViewId
			, opaqueBlackClearColor );
		result.addOutputColourView( m_renderTarget.getVelocity()->targetViewId );
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateVisibilityResolve( ProgressBar * progress )
	{
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Visibility" );
		stepProgressBar( progress, "Creating visibility resolve pass" );
		auto & result = graph.createPass( "Resolve"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising visibility resolve pass" );
				RenderNodesPassDesc renderPassDesc{ m_colour.getExtent(), m_matrixUbo, m_sceneUbo, m_renderTarget.getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true );

				if ( !VisibilityResolvePass::useCompute )
				{
					auto dataIt = framePass.images.begin();
					auto data1It = std::next( dataIt );
					auto data2It = std::next( data1It );
					auto data3It = std::next( data2It );
					auto data4It = std::next( data3It );
					renderPassDesc.implicitAction( data1It->view(), crg::RecordContext::clearAttachment( *data1It ) )
						.implicitAction( data2It->view(), crg::RecordContext::clearAttachment( *data2It ) )
						.implicitAction( data3It->view(), crg::RecordContext::clearAttachment( *data3It ) )
						.implicitAction( data4It->view(), crg::RecordContext::clearAttachment( *data4It ) );
				}

				auto res = std::make_unique< VisibilityResolvePass >( this
					, framePass
					, context
					, runnableGraph
					, m_device
					, cuT( "Visibility" )
					, cuT( "Resolve" )
					, *m_visibilityPass
					, m_visibilityPipelinesIds.get()
					, std::move( renderPassDesc )
					, RenderTechniquePassDesc{ false, getSsaoConfig() } );
				m_opaquePass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( *m_visibilityPassDesc );

		m_visibility->create();
		uint32_t index = 0u;
		result.addInputStorageView( m_visibility->targetViewId
			, index++ );

		auto & opaquePassResult = *m_opaquePassResult;
		opaquePassResult.create();

		if constexpr ( VisibilityResolvePass::useCompute )
		{
			result.addDependency( m_visibilityReorder->getLastPass() );
			result.addInputStorageBuffer( { m_materialsCounts1->getBuffer(), "MaterialsCount" }
				, index++
				, 0u
				, uint32_t( m_materialsCounts1->getBuffer().getSize() ) );
			result.addInputStorageBuffer( { m_materialsStarts->getBuffer(), "MaterialsStarts" }
				, index++
				, 0u
				, uint32_t( m_materialsStarts->getBuffer().getSize() ) );
			result.addInputStorageBuffer( { m_pixelsXY->getBuffer(), "PixelsXY" }
				, index++
				, 0u
				, uint32_t( m_pixelsXY->getBuffer().getSize() ) );
			result.addOutputStorageView( opaquePassResult[DsTexture::eData1].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eData2].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eData3].targetViewId
				, index++ );
			result.addOutputStorageView( opaquePassResult[DsTexture::eData4].targetViewId
				, index++ );
		}
		else
		{
			result.addOutputColourView( opaquePassResult[DsTexture::eData1].targetViewId
				, getClearValue( DsTexture::eData1 ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eData2].targetViewId
				, getClearValue( DsTexture::eData2 ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eData3].targetViewId
				, getClearValue( DsTexture::eData3 ) );
			result.addOutputColourView( opaquePassResult[DsTexture::eData4].targetViewId
				, getClearValue( DsTexture::eData4 ) );
		}

		return result;
	}

	crg::FramePass & RenderTechnique::doCreateDepthPass( ProgressBar * progress )
	{
		auto & previous = m_renderTarget.createVertexTransformPass();
		auto previousPasses = doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforeDepth
			, &previous );
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Base" );
		stepProgressBar( progress, "Creating depth pass" );
		auto & result = graph.createPass( "Depth"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto depthIt = framePass.images.begin();
				auto depthObjIt = std::next( depthIt );
				auto velocityIt = std::next( depthObjIt );
#if !C3D_UseDeferredRendering
				auto normalIt = std::next( velocityIt );
#endif
				stepProgressBar( progress, "Initialising depth pass" );
				auto res = std::make_unique< DepthPass >( this
					, framePass
					, context
					, runnableGraph
					, m_device
					, getSsaoConfig()
					, RenderNodesPassDesc{ m_depth->getExtent(), m_matrixUbo, m_sceneUbo, m_renderTarget.getCuller() }
						.safeBand( true )
						.meshShading( true )
						.implicitAction( depthIt->view(), crg::RecordContext::clearAttachment( *depthIt ) )
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *depthObjIt ) )
#if !C3D_UseDeferredRendering
						.implicitAction( depthObjIt->view(), crg::RecordContext::clearAttachment( *normalIt ) )
#endif
						.implicitAction( velocityIt->view(), crg::RecordContext::clearAttachment( *velocityIt ) ) );
				m_depthPass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );

		if ( m_visibilityPassDesc )
		{
			result.addInputDepthStencilView( m_depth->targetViewId );
		}
		else
		{
			result.addOutputDepthStencilView( m_depth->targetViewId
				, defaultClearDepthStencil );
		}

		result.addOutputColourView( m_depthObj->targetViewId
			, makeClearValue( 1.0f, std::numeric_limits< float >::max(), 0.0f, 0.0f ) );
		result.addOutputColourView( m_renderTarget.getVelocity()->targetViewId );
#if !C3D_UseDeferredRendering
		result.addOutputColourView( m_normal->targetViewId
			, getClearValue( DsTexture::eData1 ) );
#endif
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateComputeDepthRange( ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating compute depth range pass" );
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Base" );
		auto & result = graph.createPass( "ComputeDepthRange"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising compute depth range pass" );
				auto res = std::make_unique< ComputeDepthRange >( framePass
					, context
					, runnableGraph
					, m_device
					, m_needsDepthRange );
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( *m_depthPassDesc );
		result.addInputStorageView( m_depthObj->sampledViewId
			, ComputeDepthRange::eInput );
		result.addOutputStorageBuffer( { m_depthRange->getBuffer(), "DepthRange" }
			, ComputeDepthRange::eOutput
			, 0u
			, m_depthRange->getBuffer().getSize() );
		return result;
	}

	BackgroundRendererUPtr RenderTechnique::doCreateBackgroundPass( ProgressBar * progress )
	{
		auto previousPasses = doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforeBackground
			, m_computeDepthRangeDesc );
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Background" );
		auto result = castor::makeUnique< BackgroundRenderer >( graph
			, previousPasses
			, m_device
			, progress
			, getName()
			, *m_renderTarget.getScene()->getBackground()
			, m_renderTarget.getHdrConfigUbo()
			, m_sceneUbo
			, m_colour.targetViewId
			, true /*clearColour*/
			, m_depth->targetViewId );

		return result;
	}

	crg::FramePass & RenderTechnique::doCreateOpaquePass( ProgressBar * progress )
	{
		auto previousPasses = doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforeOpaque
#if C3D_UseDeferredRendering
			, m_depthPassDesc );
#else
			, &m_backgroundRenderer->getPass() );
#endif
		stepProgressBar( progress, "Creating opaque pass" );
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Opaque" );
#if C3D_UseDeferredRendering
		castor::String name = cuT( "Geometry" );
#else
		castor::String name = cuT( "Forward" );
#endif
		auto & result = graph.createPass( name
			, [this, progress, name]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising opaque pass" );
				RenderTechniquePassDesc techniquePassDesc{ false, getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ m_colour.getExtent(), m_matrixUbo, m_sceneUbo, m_renderTarget.getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true );
#if C3D_UseDeferredRendering
				auto data1It = std::next( framePass.images.begin(), 1u );
				auto data2It = std::next( data1It );
				auto data3It = std::next( data2It );
				auto data4It = std::next( data3It );
				renderPassDesc.implicitAction( data1It->view(), crg::RecordContext::clearAttachment( *data1It ) )
					.implicitAction( data2It->view(), crg::RecordContext::clearAttachment( *data2It ) )
					.implicitAction( data3It->view(), crg::RecordContext::clearAttachment( *data3It ) )
					.implicitAction( data4It->view(), crg::RecordContext::clearAttachment( *data4It ) );
#else
				techniquePassDesc.ssao( m_ssao->getResult() )
					.lpvConfigUbo( m_lpvConfigUbo )
					.llpvConfigUbo( m_llpvConfigUbo )
					.vctConfigUbo( m_vctConfigUbo )
					.lpvResult( *m_lpvResult )
					.llpvResult( m_llpvResult )
					.vctFirstBounce( m_voxelizer->getFirstBounce() )
					.vctSecondaryBounce( m_voxelizer->getSecondaryBounce() ) )
#endif
				auto res = std::make_unique< rendtech::OpaquePassType >( this
					, framePass
					, context
					, runnableGraph
					, m_device
#if !C3D_UseDeferredRendering
					, ForwardRenderTechniquePass::Type
#endif
					, cuT( "Opaque" )
					, name
#if !C3D_UseDeferredRendering
					, m_colour.imageId.data
#endif
					, std::move( renderPassDesc )
					, std::move( techniquePassDesc ) );
				m_opaquePass = res.get();
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
#if C3D_UseDeferredRendering
		auto & opaquePassResult = *m_opaquePassResult;
		result.addInOutDepthStencilView( m_depth->targetViewId );
		result.addOutputColourView( opaquePassResult[DsTexture::eData1].targetViewId
			, getClearValue( DsTexture::eData1 ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eData2].targetViewId
			, getClearValue( DsTexture::eData2 ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eData3].targetViewId
			, getClearValue( DsTexture::eData3 ) );
		result.addOutputColourView( opaquePassResult[DsTexture::eData4].targetViewId
			, getClearValue( DsTexture::eData4 ) );
#else
		result.addDependency( m_ssao->getLastPass() );
		result.addSampledView( m_ssao->getResult().sampledViewId, 0u );
		result.addInOutDepthStencilView( m_depth->targetViewId );
		result.addInOutColourView( m_colour.targetViewId );
#endif
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateTransparentPass( ProgressBar * progress )
	{
		auto previousPasses = doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforeTransparent
#if C3D_UseDeferredRendering
			, &m_deferredRendering->getLastPass() );
#else
			, m_opaquePassDesc );
#endif
		stepProgressBar( progress, "Creating transparent pass" );
		auto & graph = m_renderTarget.getGraph().createPassGroup( "Transparent" );
		auto & result = graph.createPass( "NodesPass"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising transparent pass" );
#if C3D_UseWeightedBlendedRendering
				castor::String name = cuT( "Accumulation" );
				static constexpr bool isOit = true;
				static constexpr bool hasVelocity = true;
				auto accumIt = std::next( framePass.images.begin() );
				auto revealIt = std::next( accumIt );
#else
				castor::String name = cuT( "Forward" );
				static constexpr bool isOit = false;
				static constexpr bool hasVelocity = false;
#endif
				auto res = std::make_unique< rendtech::TransparentPassType >( this
					, framePass
					, context
					, runnableGraph
					, m_device
#if !C3D_UseWeightedBlendedRendering
					, ForwardRenderTechniquePass::Type
#endif
					, cuT( "Transparent" )
					, name
					, m_colour.imageId.data
					, RenderNodesPassDesc{ m_colour.getExtent(), m_matrixUbo, m_sceneUbo, m_renderTarget.getCuller(), isOit }
						.safeBand( true )
						.meshShading( true )
#if C3D_UseWeightedBlendedRendering
						.implicitAction( accumIt->view(), crg::RecordContext::clearAttachment( *accumIt ) )
						.implicitAction( revealIt->view(), crg::RecordContext::clearAttachment( *revealIt ) )
#endif
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
				getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		result.addInOutDepthStencilView( m_depth->targetViewId );

#if C3D_UseWeightedBlendedRendering
		auto & transparentPassResult = *m_transparentPassResult;
		result.addOutputColourView( transparentPassResult[WbTexture::eAccumulation].targetViewId
			, getClearValue( WbTexture::eAccumulation ) );
		result.addOutputColourView( transparentPassResult[WbTexture::eRevealage].targetViewId
			, getClearValue( WbTexture::eRevealage ) );
		result.addInOutColourView( m_renderTarget.getVelocity()->targetViewId );
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
			rendtech::doPrepareShadowMap( cache
				, LightType::eDirectional
				, *m_directionalShadowMap
				, m_activeShadowMaps
				, m_lightPropagationVolumes
				, m_lightPropagationVolumesG
				, m_layeredLightPropagationVolumes
				, m_layeredLightPropagationVolumesG
				, updater );
			rendtech::doPrepareShadowMap( cache
				, LightType::ePoint
				, *m_pointShadowMap
				, m_activeShadowMaps
				, m_lightPropagationVolumes
				, m_lightPropagationVolumesG
				, m_layeredLightPropagationVolumes
				, m_layeredLightPropagationVolumesG
				, updater );
			rendtech::doPrepareShadowMap( cache
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
				for ( auto & id : map.ids )
				{
					updater.light = id.first;
					updater.index = id.second;
					map.shadowMap.get().update( updater );
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

	crg::SemaphoreWaitArray RenderTechnique::doRenderLPV( crg::SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )
	{
		crg::SemaphoreWaitArray result = semaphore;

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

	crg::SemaphoreWaitArray RenderTechnique::doRenderShadowMaps( crg::SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )const
	{
		crg::SemaphoreWaitArray result = semaphore;
		auto & scene = *m_renderTarget.getScene();

		if ( scene.hasShadows() )
		{
			for ( auto & array : m_activeShadowMaps )
			{
				for ( auto & shadowMap : array )
				{
					for ( auto & index : shadowMap.ids )
					{
						result = shadowMap.shadowMap.get().render( result, queue, index.second );
					}
				}
			}
		}

		return result;
	}

	crg::SemaphoreWaitArray RenderTechnique::doRenderEnvironmentMaps( crg::SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )const
	{
		crg::SemaphoreWaitArray result = semaphore;

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

	crg::SemaphoreWaitArray RenderTechnique::doRenderVCT( crg::SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )const
	{
		crg::SemaphoreWaitArray result = semaphore;
		auto scene = m_renderTarget.getScene();

		if ( scene
			&& scene->getVoxelConeTracingConfig().enabled )
		{
			result = m_voxelizer->render( result, queue );
		}

		return result;
	}
}

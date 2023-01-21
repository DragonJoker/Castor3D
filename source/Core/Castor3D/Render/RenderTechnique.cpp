#include "Castor3D/Render/RenderTechnique.hpp"

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
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/Voxelizer.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Opaque/OpaqueRendering.hpp"
#include "Castor3D/Render/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Opaque/DeferredRendering.hpp"
#include "Castor3D/Render/Opaque/VisibilityReorderPass.hpp"
#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"
#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"
#include "Castor3D/Render/Passes/ComputeDepthRange.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Prepass/DepthPass.hpp"
#include "Castor3D/Render/Prepass/VisibilityPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Ssao/SsaoPass.hpp"
#include "Castor3D/Render/Transparent/TransparentPass.hpp"
#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Render/Transparent/WeightedBlendRendering.hpp"
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

		static LightVolumePassResultArray doCreateLLPVResult( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String const & prefix )
		{
			LightVolumePassResultArray result;
			auto & engine = *device.renderSystem.getEngine();

			for ( uint32_t i = 0u; i < LpvMaxCascadesCount; ++i )
			{
				result.emplace_back( castor::makeUnique< LightVolumePassResult >( resources
					, device
					, prefix + castor::string::toString( i )
					, engine.getLpvGridSize() ) );
			}

			return result;
		}

		static crg::FrameGraph doCreateClearLpvCommands( crg::ResourcesCache & resources
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
							, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_TRANSFER_BIT ); } )
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
			crg::FrameGraph result{ resources.getHandler(), name + "ClearLpv" };
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
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( castor::String const & name
		, RenderTarget & renderTarget
		, RenderDevice const & device
		, QueueData const & queueData
		, Parameters const & parameters
		, SsaoConfig const & ssaoConfig
		, ProgressBar * progress
		, bool deferred
		, bool visbuffer
		, bool weightedBlended )
		: castor::OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, castor::Named{ name + cuT( "/Technique") }
		, m_renderTarget{ renderTarget }
		, m_device{ device }
		, m_targetSize{ m_renderTarget.getSize() }
		, m_rawSize{ getSafeBandedSize( m_targetSize ) }
		, m_colour{ m_device
			, m_renderTarget.getResources()
			, getName() + "/Colour"
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
		, m_depth{ std::make_shared< Texture >( m_device
			, m_renderTarget.getResources()
			, getName() + "/" + getTexName( PpTexture::eDepth )
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, getFormat( m_device, PpTexture::eDepth )
			, getUsageFlags( PpTexture::eDepth )
			, getBorderColor( PpTexture::eDepth ) ) }
		, m_normal{ std::make_shared< Texture >( m_device
			, m_renderTarget.getResources()
			, getName() + "/" + getTexName( DsTexture::eNmlOcc )
			, 0u
			, m_colour.getExtent()
			, 1u
			, 1u
			, getFormat( m_device, DsTexture::eNmlOcc )
			, getUsageFlags( DsTexture::eNmlOcc )
			, getBorderColor( DsTexture::eNmlOcc ) ) }
		, m_matrixUbo{ m_device }
		, m_sceneUbo{ m_device }
		, m_gpInfoUbo{ m_device }
		, m_lpvConfigUbo{ m_device }
		, m_llpvConfigUbo{ m_device }
		, m_vctConfigUbo{ m_device }
		, m_voxelizer{ castor::makeUnique< Voxelizer >( m_renderTarget.getResources()
			, m_device
			, progress
			, getName()
			, *m_renderTarget.getScene()
			, *m_renderTarget.getCamera()
			, m_vctConfigUbo
			, m_renderTarget.getScene()->getVoxelConeTracingConfig() ) }
		, m_lpvResult{ castor::makeUnique< LightVolumePassResult >( m_renderTarget.getResources()
			, m_device
			, getName()
			, getEngine()->getLpvGridSize() ) }
		, m_llpvResult{ rendtech::doCreateLLPVResult( m_renderTarget.getResources()
			, m_device
			, getName() ) }
#if !C3D_DebugDisableShadowMaps
		, m_directionalShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapDirectional >( m_renderTarget.getResources()
			, m_device
			, *m_renderTarget.getScene()
			, progress ) }
		, m_pointShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapPoint >( m_renderTarget.getResources()
			, m_device
			, *m_renderTarget.getScene()
			, progress ) }
		, m_spotShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapSpot >( m_renderTarget.getResources()
			, m_device
			, *m_renderTarget.getScene()
			, progress ) }
#endif
		, m_prepass{ *this
			, m_device
			, queueData
			, doCreateRenderPasses( progress , TechniquePassEvent::eBeforeDepth, &m_renderTarget.createVertexTransformPass() )
			, progress
			, deferred
			, visbuffer }
		, m_background{ doCreateBackgroundPass( progress ) }
		, m_opaque{ *this
			, m_device
			, queueData
			, m_prepass
			, doCreateRenderPasses( progress , TechniquePassEvent::eBeforeOpaque, &m_background->getPass() )
			, getSsaoConfig()
			, progress
			, m_normal
			, deferred }
		, m_transparent{ *this
			, m_device
			, queueData
			, m_opaque
			, doCreateRenderPasses( progress , TechniquePassEvent::eBeforeTransparent, nullptr )
			, getSsaoConfig()
			, progress
			, weightedBlended }
		, m_clearLpvGraph{ rendtech::doCreateClearLpvCommands( m_renderTarget.getResources(), device, progress, getName(), *m_lpvResult, m_llpvResult ) }
		, m_clearLpvRunnable{ m_clearLpvGraph.compile( m_device.makeContext() ) }
	{
		m_renderTarget.getGraph().addDependency( m_voxelizer->getGraph() );
		doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforePostEffects
			, &m_transparent.getLastPass() );

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

#if !C3D_DebugDisableShadowMaps
		m_allShadowMaps[size_t( LightType::eDirectional )].emplace_back( std::ref( *m_directionalShadowMap ), UInt32Array{} );
		m_allShadowMaps[size_t( LightType::eSpot )].emplace_back( std::ref( *m_spotShadowMap ), UInt32Array{} );
		m_allShadowMaps[size_t( LightType::ePoint )].emplace_back( std::ref( *m_pointShadowMap ), UInt32Array{} );
		doInitialiseLpv();
#endif
	}

	RenderTechnique::~RenderTechnique()
	{
		m_llpvResult.clear();
		m_lpvResult.reset();
		m_voxelizer.reset();
		m_normal->destroy();
		m_colour.destroy();
		m_depth->destroy();

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
		result += PrepassRendering::countInitialisationSteps();
		result += Voxelizer::countInitialisationSteps();
		result += 2;// m_backgroundRenderer;
		++result;// m_directionalShadowMap;
		++result;// m_pointShadowMap;
		++result;// m_spotShadowMap;
		result += OpaqueRendering::countInitialisationSteps();
		result += TransparentRendering::countInitialisationSteps();
		result += 2;// m_clearLpv;
		result += LightPropagationVolumes::countInitialisationSteps();
		result += LightPropagationVolumesG::countInitialisationSteps();
		result += LayeredLightPropagationVolumes::countInitialisationSteps();
		result += LayeredLightPropagationVolumesG::countInitialisationSteps();
		return result;
	}

	void RenderTechnique::update( CpuUpdater & updater )
	{
		auto & scene = *updater.scene;
		auto & camera = *updater.camera;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;

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
		m_prepass.update( updater );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeBackground )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
		m_background->update( updater );

		if ( updater.voxelConeTracing )
		{
			m_voxelizer->update( updater );
		}

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeOpaque )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
		m_opaque.update( updater );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeTransparent )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
		m_transparent.update( updater );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforePostEffects )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );

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

		m_prepass.update( updater );
		m_opaque.update( updater );
		m_transparent.update( updater );
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

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeDepth )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );
		m_prepass.accept( visitor );
		m_voxelizer->accept( visitor );

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
		m_opaque.accept( visitor );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeTransparent )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );
		m_transparent.accept( visitor );

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

		return m_transparent.getLastPass();
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
		return m_opaque.getSsaoResult();
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
		return m_opaque.getLightDepthImgView();
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
		return m_opaque.getDiffuseLightingResult();
	}

	Texture const & RenderTechnique::getScatteringLightingResult()const
	{
		return m_opaque.getScatteringLightingResult();
	}

	Texture const & RenderTechnique::getBaseColourResult()const
	{
		return m_opaque.getBaseColourResult();
	}

	crg::ResourcesCache & RenderTechnique::getResources()const
	{
		return m_renderTarget.getResources();
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

	BackgroundRendererUPtr RenderTechnique::doCreateBackgroundPass( ProgressBar * progress )
	{
		auto previousPasses = doCreateRenderPasses( progress
			, TechniquePassEvent::eBeforeBackground
			, &m_prepass.getLastPass() );
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
			, m_depth->targetViewId
			, &m_prepass.getDepthObj().sampledViewId );

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
				m_lightPropagationVolumes[i] = castor::makeUnique< LightPropagationVolumes >( getResources()
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
				m_lightPropagationVolumesG[i] = castor::makeUnique< LightPropagationVolumesG >( getResources()
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
				m_layeredLightPropagationVolumes[i] = castor::makeUnique< LayeredLightPropagationVolumes >( getResources()
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
				m_layeredLightPropagationVolumesG[i] = castor::makeUnique< LayeredLightPropagationVolumesG >( getResources()
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

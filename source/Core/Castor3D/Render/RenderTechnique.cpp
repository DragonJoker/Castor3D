#include "Castor3D/Render/RenderTechnique.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMaps.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/Voxelizer.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Opaque/ComputeDiffusionProfilesPass.hpp"
#include "Castor3D/Render/Opaque/OpaqueRendering.hpp"
#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Transparent/TransparentPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"

#include <RenderGraph/FramePassGroup.hpp>

CU_ImplementSmartPtr( c3d, RenderTechnique )

namespace c3d
{
	//*************************************************************************************************

	namespace rendtech
	{
		static Map< double, LightInstanceRPtr > doSortLights( LightCache const & cache
			, LightType type
			, Camera const & camera )
		{
			auto lock( makeUniqueLock( cache ) );
			Map< double, LightInstanceRPtr > lights;

			if ( cache.getLightsBufferCount( LightType::eDirectional ) <= 1u
				&& cache.getLightsBufferCount( LightType::ePoint ) <= MaxPointShadowMapCount
				&& cache.getLightsBufferCount( LightType::eSpot ) <= MaxSpotShadowMapCount )
			{
				double index{};

				for ( auto & light : cache.getLightInstances( type ) )
				{
					light->setShadowMap( nullptr );

					if ( light->isShadowProducer() )
					{
						lights.emplace( index, light );
					}

					++index;
				}

				return lights;
			}

			for ( auto & light : cache.getLightInstances( type ) )
			{
				light->setShadowMap( nullptr );

				if ( light->isShadowProducer()
					&& ( light->getLightType() == LightType::eDirectional
						|| camera.isVisible( light->getBoundingBox()
							, light->getNode().getDerivedTransformationMatrix() ) ) )
				{
					lights.emplace( point::distanceSquared( camera.getParent()->getDerivedPosition()
							, light->getNode().getDerivedPosition() )
						, light );
				}
			}

			return lights;
		}

		static void doPrepareShadowMap( LightCache const & cache
			, LightType type
			, ShadowBuffer & shadowBuffer
			, ShadowMap & shadowMap
			, ShadowMapLightArray & activeShadowMaps
			, LightPropagationVolumesLightType const & lightPropagationVolumes
			, LightPropagationVolumesGLightType const & lightPropagationVolumesG
			, LayeredLightPropagationVolumesLightType const & layeredLightPropagationVolumes
			, LayeredLightPropagationVolumesGLightType const & layeredLightPropagationVolumesG
			, ReflectiveShadowMapsRPtr reflectiveShadowMaps
			, CpuUpdater & updater )
		{
			auto lights = doSortLights( cache, type, *updater.camera );
			size_t count = std::min( shadowMap.getCount(), uint32_t( lights.size() ) );

			if ( count > 0 )
			{
				int32_t index = 0;
				auto lightIt = lights.begin();
				activeShadowMaps[size_t( type )].emplace_back( c3d::ref( shadowMap ) );
				auto & active = activeShadowMaps[size_t( type )].back();

				for ( auto i = 0u; i < count; ++i )
				{
					auto & light = *lightIt->second;
					light.setShadowMap( &shadowMap, index );
					active.ids.push_back( { &light, uint32_t( index ) } );
					updater.light = &light;
					updater.index = uint32_t( index );
					shadowMap.update( updater );
					light.fillShadowBuffer( shadowBuffer.getData() );

					switch ( updater.light->getGlobalIlluminationType() )
					{
					case GlobalIlluminationType::eRsm:
						if ( reflectiveShadowMaps )
							reflectiveShadowMaps->registerLight( updater.light );
						break;
					case GlobalIlluminationType::eLpv:
						if ( lightPropagationVolumes[size_t( type )] )
							lightPropagationVolumes[size_t( type )]->registerLight( updater.light );
						break;
					case GlobalIlluminationType::eLpvG:
						if ( lightPropagationVolumesG[size_t( type )] )
							lightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
						break;
					case GlobalIlluminationType::eLayeredLpv:
						if ( shadowMap.getEngine()->getRenderSystem()->hasLLPV() )
						{
							if ( layeredLightPropagationVolumes[size_t( type )] )
								layeredLightPropagationVolumes[size_t( type )]->registerLight( updater.light );
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
								layeredLightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
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
			, Scene const & scene
			, String const & prefix )
		{
			LightVolumePassResultArray result;

			for ( uint32_t i = 0u; i < LpvMaxCascadesCount; ++i )
			{
				result.emplace_back( makeUnique< LightVolumePassResult >( resources
					, device
					, prefix + string::toString( i )
					, scene.getLpvGridSize() ) );
			}

			return result;
		}

		static crg::FrameGraph doCreateClearLpvCommands( crg::ResourcesCache const & resources
			, ProgressBar * progress
			, String const & name
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
						, { crg::defaultV< InitialiseCallback >
							, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eTransfer ); } )
							, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); } } }
				{
				}

			protected:
				void doRecordInto( crg::RecordContext & recContext
					, VkCommandBuffer commandBuffer
					, uint32_t index )const
				{
					for ( auto [_, attach] : getPass().getOutputs() )
						recContext.clearAttachment( commandBuffer, attach->view( index ), transparentBlackClearColor, ImageLayout::eUndefined );
				}
			};

			stepProgressBarLocal( progress, cuT( "Creating clear LPV commands" ) );
			auto mbName = toUtf8( name );
			crg::FrameGraph result{ resources.getHandler(), mbName + "/ClearLpv" };
			auto & pass = result.createPass( mbName + "LpvClear"
				, [progress]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					stepProgressBarLocal( progress, cuT( "Initialising clear LPV commands" ) );
					return makeRawUnique< LpvClear >( framePass
						, context
						, runnableGraph );
				} );

			for ( auto & texture : lpvResult )
				texture->setLastAttach( pass.addOutputTransferImage( texture->getWholeViewId() ) );

			for ( auto & textures : llpvResult )
			{
				for ( auto & texture : *textures )
					texture->setLastAttach( pass.addOutputTransferImage( texture->getWholeViewId() ) );
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

		static ImageUsageFlags constexpr depthUsageFlags = ( ImageUsageFlags::eSampled
			| ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eTransferSrc
			| ImageUsageFlags::eDepthStencilAttachment );
		static ImageUsageFlags constexpr normalUsageFlags = ( ImageUsageFlags::eSampled
			| ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eColorAttachment
			| ImageUsageFlags::eTransferSrc
			| ImageUsageFlags::eStorage );
		static ImageUsageFlags constexpr scatteringUsageFlags = ( ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eColorAttachment
			| ImageUsageFlags::eStorage
			| ImageUsageFlags::eSampled );
		static ImageUsageFlags constexpr diffuseUsageFlags = ( ImageUsageFlags::eTransferSrc
			| ImageUsageFlags::eTransferDst
			| ImageUsageFlags::eColorAttachment
			| ImageUsageFlags::eStorage
			| ImageUsageFlags::eSampled );
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( String const & name
		, RenderTarget & renderTarget
		, RenderDevice const & device
		, Texture & colour
		, ProgressBar * progress
		, bool visbuffer
		, bool weightedBlended )
		: OwnedBy< Engine >{ c3d::getEngine( device ) }
		, Named{ name + cuT( "/Technique") }
		, m_renderTarget{ renderTarget }
		, m_device{ device }
		, m_rawSize{ getSafeBandedSize( m_renderTarget.getRenderSize() ) }
		, m_colour{ &colour }
		, m_depth{ m_device
			, m_renderTarget.getResources()
			, getName() + cuT( "/Depth" )
			, { ImageCreateFlags::eNone
				, m_colour->getExtent(), 1u, 1u
				, m_device.selectSuitableDepthStencilFormat( getFeatureFlags( rendtech::depthUsageFlags ) )
				, rendtech::depthUsageFlags }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_normal{ m_device
			, m_renderTarget.getResources()
			, getName() + cuT( "/Normal" )
			, { ImageCreateFlags::eNone
				, m_colour->getExtent(), 1u, 1u
				, PixelFormat::eR16G16B16A16_SFLOAT
				, rendtech::normalUsageFlags }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_scattering{ m_device
			, m_renderTarget.getResources()
			, getName() + cuT( "/Scattering" )
			, { ImageCreateFlags::eNone
				, m_colour->getExtent(), 1u, 1u
				, device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( rendtech::scatteringUsageFlags ) )
				, rendtech::scatteringUsageFlags }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_diffuse{ m_device
			, m_renderTarget.getResources()
			, getName() + cuT( "/Diffuse" )
			, { ImageCreateFlags::eNone
				, m_colour->getExtent(), 1u, 1u
				, PixelFormat::eR16G16B16A16_SFLOAT
				, rendtech::diffuseUsageFlags | ( C3D_UseVisibilityBuffer ? ImageUsageFlags::eNone : ImageUsageFlags::eColorAttachment ) }
			, { BorderColour::eFloatOpaqueBlack } }
		, m_lpvConfigUbo{ m_device }
		, m_llpvConfigUbo{ m_device }
		, m_vctConfigUbo{ m_device }
		, m_graph{ m_renderTarget.getGraph().createPassGroup( "Technique" ) }
#if !C3D_DebugDisableShadowMaps
		, m_shadowBuffer{ ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->hasShadows() )
			? makeUnique< ShadowBuffer >( device )
			: nullptr ) }
		, m_directionalShadowMap{ ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->hasShadows() )
			? makeUniqueDerived< ShadowMap, ShadowMapDirectional >( m_renderTarget.getResources()
				, m_device
				, *m_renderTarget.getScene()
				, progress )
			: nullptr ) }
		, m_pointShadowMap{ ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->hasShadows() )
			? makeUniqueDerived< ShadowMap, ShadowMapPoint >( m_renderTarget.getResources()
				, m_device
				, *m_renderTarget.getScene()
				, progress )
			: nullptr ) }
		, m_spotShadowMap{ ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->hasShadows() )
			? makeUniqueDerived< ShadowMap, ShadowMapSpot >( m_renderTarget.getResources()
				, m_device
				, *m_renderTarget.getScene()
				, progress )
			: nullptr ) }
#endif
		, m_voxelizer{ ( ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->getVoxelConeTracingConfig().enabled ) && m_device.hasGeometryShader() )
			? makeUnique< Voxelizer >( m_renderTarget.getResources()
				, m_device
				, progress
				, getName()
				, *m_renderTarget.getScene()
				, *m_renderTarget.getCamera()
				, m_vctConfigUbo
				, m_renderTarget.getScene()->getVoxelConeTracingConfig() )
			: nullptr ) }
		, m_rsmResult{ ( m_shadowBuffer
			? makeUnique< Texture >( m_device
				, m_renderTarget.getResources()
				, getName() + "RSMResult"
				, TextureCreateInfo{ ImageCreateFlags::eNone
					, colour.getExtent(), 1u, 1u
					, PixelFormat::eR16G16B16A16_SFLOAT
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eSampled
						| ImageUsageFlags::eTransferDst ) }
				, TextureSamplerInfo{} )
			: nullptr ) }
		, m_lpvResult{ ( m_shadowBuffer
			? makeUnique< LightVolumePassResult >( m_renderTarget.getResources()
				, m_device
				, getName()
				, m_renderTarget.getScene()->getLpvGridSize() )
			: nullptr ) }
		, m_llpvResult{ ( m_shadowBuffer
			? rendtech::doCreateLLPVResult( m_renderTarget.getResources()
				, m_device
				, *renderTarget.getScene()
				, getName() )
			: LightVolumePassResultArray{} ) }
		, m_indirectLighting{ ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->needsGlobalIllumination() ) ? &m_lpvConfigUbo : nullptr )
			, ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->needsGlobalIllumination() ) ? &m_llpvConfigUbo : nullptr )
			, ( m_voxelizer ? &m_vctConfigUbo : nullptr )
			, ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->needsGlobalIllumination() ) ? m_rsmResult.get() : nullptr )
			, ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->needsGlobalIllumination() ) ? m_lpvResult.get() : nullptr )
			, ( ( m_renderTarget.isFullLoadingEnabled() || m_renderTarget.getScene()->needsGlobalIllumination() ) ? &m_llpvResult : nullptr )
			, ( m_voxelizer ? &m_voxelizer->getFirstBounce() : nullptr )
			, ( m_voxelizer ? &m_voxelizer->getSecondaryBounce() : nullptr ) }
		, m_clearLpvGraph{ ( m_shadowBuffer
			? rendtech::doCreateClearLpvCommands( m_renderTarget.getResources(), progress, getName(), *m_lpvResult, m_llpvResult )
			: crg::FrameGraph{ m_renderTarget.getResources().getHandler(), toUtf8( getName() ) + "/ClearLpv" } ) }
		, m_clearLpvRunnable{ ( m_shadowBuffer
			? m_clearLpvGraph.compile( m_device.makeContext() )
			: nullptr ) }
	{
		m_renderTarget.createVertexTransformPass( m_graph );
		doCreateRenderPasses( TechniquePassEvent::eBeforeDepth );
		m_prepass = makeRawUnique< PrepassRendering >( *this
			, m_device
			, progress
			, visbuffer );
		if ( m_renderTarget.getFrustumClusters() && m_renderTarget.getClustersConfig().enabled )
			m_renderTarget.getFrustumClusters()->createFramePasses( m_graph, getRenderUbo() );
		m_background = doCreateBackgroundPass( progress );
		createComputeDiffusionProfilesPass( m_graph
			, m_device
			, getEngine()->getMaterialCache().getSssProfileBuffer()
			, getEngine()->getMaterialCache().getSssProfileBuffer().getDiffusionProfilesImage() );
		m_opaque = makeRawUnique< OpaqueRendering >( *this
			, m_device
			, *m_prepass
			, progress );
		doCreateRenderPasses( TechniquePassEvent::eBeforeTransparent );
		m_transparent = makeRawUnique< TransparentRendering >( *this
			, m_device
			, progress
			, weightedBlended );
		m_renderTarget.getFrustumClusters()->createDebugDisplayPrograms( getCameraUbo() );

		if ( m_clearLpvRunnable )
		{
			getEngine()->registerTimer( makeString( m_clearLpvRunnable->getName() )
				, m_clearLpvRunnable->getTimer() );
		}

		if ( m_voxelizer )
		{
			m_renderTarget.getGraph().addDependency( m_voxelizer->getGraph() );
		}

		doCreateRenderPasses( TechniquePassEvent::eBeforePostEffects );

		m_depth.create();
		m_normal.create();
		m_scattering.create();
		m_diffuse.create();

		if ( auto runnable = m_clearLpvRunnable.get() )
		{
			c3d::getEngine( m_device ).postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [runnable]( RenderDevice const &
					, QueueData const & )
				{
					runnable->record();
				} ) );
		}

#if !C3D_DebugDisableShadowMaps
		if ( m_directionalShadowMap )
		{
			m_allShadowMaps[size_t( LightType::eDirectional )].emplace_back( c3d::ref( *m_directionalShadowMap ), UInt32Array{} );
		}

		if ( m_spotShadowMap )
		{
			m_allShadowMaps[size_t( LightType::eSpot )].emplace_back( c3d::ref( *m_spotShadowMap ), UInt32Array{} );
		}

		if ( m_pointShadowMap )
		{
			m_allShadowMaps[size_t( LightType::ePoint )].emplace_back( c3d::ref( *m_pointShadowMap ), UInt32Array{} );
		}

		doInitialiseRsm();
		doInitialiseLpv();
#endif
	}

	RenderTechnique::~RenderTechnique()noexcept
	{
		if ( m_clearLpvRunnable )
		{
			getEngine()->unregisterTimer( makeString( m_clearLpvRunnable->getName() )
				, m_clearLpvRunnable->getTimer() );
		}

		m_transparent.reset();
		m_opaque.reset();
		m_prepass.reset();

		m_llpvResult.clear();
		m_lpvResult.reset();

		if ( m_rsmResult )
		{
			m_rsmResult->destroy();
			m_rsmResult.reset();
		}

		m_voxelizer.reset();
		m_diffuse.destroy();
		m_scattering.destroy();
		m_normal.destroy();
		m_depth.destroy();

		for ( auto & array : m_activeShadowMaps )
		{
			array.clear();
		}

		m_directionalShadowMap.reset();
		m_pointShadowMap.reset();
		m_spotShadowMap.reset();
	}

	uint32_t RenderTechnique::countInitialisationSteps()noexcept
	{
		uint32_t result = 0u;
		result += PrepassRendering::countInitialisationSteps();
		result += Voxelizer::countInitialisationSteps();
		result += 1;// m_backgroundRenderer
		++result;// m_directionalShadowMap
		++result;// m_pointShadowMap
		++result;// m_spotShadowMap
		result += OpaqueRendering::countInitialisationSteps();
		result += TransparentRendering::countInitialisationSteps();
		result += 2;// m_clearLpv
		result += LightPropagationVolumes::countInitialisationSteps();
		result += LightPropagationVolumesG::countInitialisationSteps();
		result += LayeredLightPropagationVolumes::countInitialisationSteps();
		result += LayeredLightPropagationVolumesG::countInitialisationSteps();
		return result;
	}

	void RenderTechnique::update( CpuUpdater & updater )
	{
		auto & scene = *updater.scene;
		updater.voxelConeTracing = scene.getVoxelConeTracingConfig().enabled;
		auto const & debugConfig = getDebugConfig();
		updater.debugIndex = debugConfig.intermediateShaderValueIndex;

		doUpdateShadowMaps( updater );
		doUpdateRsm( updater );
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
		m_prepass->update( updater );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeBackground )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
		m_background->update( updater );

		if ( updater.voxelConeTracing && m_voxelizer )
		{
			m_voxelizer->update( updater );
		}

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeOpaque )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
		m_opaque->update( updater );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeTransparent )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
		m_transparent->update( updater );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforePostEffects )]
			, [&updater]( RenderTechniquePass & renderPass )
			{
				renderPass.update( updater );
			} );
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

		doInitialiseRsm();
		doInitialiseLpv();
		doUpdateShadowMaps( updater );

		if ( m_renderTarget.getTargetType() == TargetType::eWindow )
		{
			scene.getEnvironmentMap().update( updater );
		}

		m_prepass->update( updater );
		m_opaque->update( updater );
		m_transparent->update( updater );
	}

	SemaphoreWaitArray RenderTechnique::preRender( SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		auto result = toWait;
		result = doRenderShadowMaps( result, queue );
		result = doRenderRSM( result, queue );
		result = doRenderLPV( result, queue );
		result = doRenderEnvironmentMaps( result, queue );
		result = doRenderVCT( result, queue );
		return result;
	}

	void RenderTechnique::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "Technique Colour" )
			, *m_colour
			, m_renderTarget.getGraph().getFinalLayoutState( m_colour->getSampledViewId() ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Technique Depth" )
			, m_depth
			, m_renderTarget.getGraph().getFinalLayoutState( m_depth.getSampledViewId() ).layout
			, TextureFactors{}.invert( true ) );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeDepth )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );
		m_prepass->accept( visitor );

		if ( m_voxelizer )
		{
			m_voxelizer->accept( visitor );
		}

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
		m_opaque->accept( visitor );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforeTransparent )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );
		m_transparent->accept( visitor );

		rendtech::applyAction( m_renderPasses[size_t( TechniquePassEvent::eBeforePostEffects )]
			, [&visitor]( RenderTechniquePass & renderPass )
			{
				renderPass.accept( visitor );
			} );

		if ( m_reflectiveShadowMaps )
		{
			m_reflectiveShadowMaps->accept( visitor );
		}

#if !C3D_DebugDisableShadowMaps
		if ( m_directionalShadowMap )
		{
			m_directionalShadowMap->accept( visitor );
		}

		if ( m_spotShadowMap )
		{
			m_spotShadowMap->accept( visitor );
		}

		if ( m_pointShadowMap )
		{
			m_pointShadowMap->accept( visitor );
		}
#endif

		for ( auto const & lpv : m_lightPropagationVolumes )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}

		for ( auto const & lpv : m_lightPropagationVolumesG )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}

		for ( auto const & lpv : m_layeredLightPropagationVolumes )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}

		for ( auto const & lpv : m_layeredLightPropagationVolumesG )
		{
			if ( lpv )
			{
				lpv->accept( visitor );
			}
		}
	}

	SsaoConfig const & RenderTechnique::getSsaoConfig()const
	{
		return m_renderTarget.getSsaoConfig();
	}

	SsaoConfig & RenderTechnique::getSsaoConfig()
	{
		return m_renderTarget.getSsaoConfig();
	}

	Texture & RenderTechnique::getVelocity()const
	{
		return m_renderTarget.getVelocity();
	}

	ClustersConfig const * RenderTechnique::getClustersConfig()const
	{
		return &m_renderTarget.getClustersConfig();
	}

	Texture const & RenderTechnique::getSsaoResult()const
	{
		return m_opaque->getSsaoResult();
	}

	Texture const & RenderTechnique::getSssDiffuse()const
	{
		return m_opaque->getSssDiffuse();
	}

	Texture const & RenderTechnique::getDiffusionProfiles()const
	{
		auto const & engine = *m_renderTarget.getOwner();
		auto const & matCache = engine.getMaterialCache();
		return matCache.getSssProfileBuffer().getDiffusionProfilesImage();
	}

	crg::Attachment const & RenderTechnique::getVertexTransform()const
	{
		auto const & scene = *m_renderTarget.getScene();
		return scene.getRenderNodes().getVertexTransform();
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

	crg::ResourcesCache & RenderTechnique::getResources()const
	{
		return m_renderTarget.getResources();
	}

	bool RenderTechnique::isOpaqueEnabled()const
	{
		return m_opaque->isEnabled();
	}

	DebugConfig & RenderTechnique::getDebugConfig()const
	{
		return m_renderTarget.getScene()->getDebugConfig();
	}

	bool RenderTechnique::areDebugTargetsEnabled()const noexcept
	{
		return m_renderTarget.areDebugTargetsEnabled();
	}

	CameraUbo const & RenderTechnique::getCameraUbo()const noexcept
	{
		return m_renderTarget.getCameraUbo();
	}

	RenderUbo const & RenderTechnique::getRenderUbo()const noexcept
	{
		return m_renderTarget.getRenderUbo();
	}

	SceneUbo const & RenderTechnique::getSceneUbo()const noexcept
	{
		return m_renderTarget.getSceneUbo();
	}

	crg::FramePassArray RenderTechnique::doCreateRenderPasses( TechniquePassEvent event )
	{
		crg::FramePassArray result;

		for ( auto renderPassInfo : getEngine()->getRenderPassInfos( event ) )
		{
			result = renderPassInfo->create( m_device, *this
				, m_renderPasses, m_renderTarget.getResources() );
		}

		return result;
	}

	BackgroundRendererUPtr RenderTechnique::doCreateBackgroundPass( ProgressBar * progress )
	{
		auto & graph = m_graph.createPassGroup( "Background" );
		graph.addGroupOutput( getTargetResult().getWholeViewId() );
		auto result = makeUnique< BackgroundRenderer >( graph
			, m_device
			, progress
			, *m_renderTarget.getScene()->getBackground()
			, m_renderTarget.getRenderUbo()
			, getSceneUbo()
			, getTargetResult()
			, true /*clearColour*/
			, false /*clearDepth*/
			, false /*forceVisible*/
			, &getTargetDepth()
			, &m_prepass->getDepthObj() );

		return result;
	}

	void RenderTechnique::doInitialiseRsm()
	{
		auto & scene = *m_renderTarget.getScene();
		auto needRsm = scene.needsGlobalIllumination( GlobalIlluminationType::eRsm );

		if ( needRsm && !m_reflectiveShadowMaps )
		{
			m_rsmResult->create();
			m_reflectiveShadowMaps = makeUnique< ReflectiveShadowMaps >( getResources()
				, scene
				, m_device
				, getCameraUbo()
				, *m_shadowBuffer
				, m_prepass->getDepthObj()
				, m_normal
				, m_allShadowMaps[size_t( LightType::eDirectional )].front().first.get().getShadowPassResult( false )
				, m_allShadowMaps[size_t( LightType::ePoint )].front().first.get().getShadowPassResult( false )
				, m_allShadowMaps[size_t( LightType::eSpot )].front().first.get().getShadowPassResult( false )
				, *m_rsmResult );
			m_reflectiveShadowMaps->initialise();
		}
	}

	void RenderTechnique::doInitialiseLpv()
	{
		auto & scene = *m_renderTarget.getScene();

		for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
		{
			if ( m_allShadowMaps[i].empty() )
			{
				continue;
			}

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
				for ( auto const & lpvResult : m_llpvResult )
				{
					lpvResult->create();
				}
			}

			if ( needLpv && !m_lightPropagationVolumes[i] )
			{
				m_lightPropagationVolumes[i] = makeUnique< LightPropagationVolumes >( getResources()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult( false )
					, *m_lpvResult
					, m_lpvConfigUbo );
				m_lightPropagationVolumes[i]->initialise();
			}

			if ( needLpvG && !m_lightPropagationVolumesG[i] )
			{
				m_lightPropagationVolumesG[i] = makeUnique< LightPropagationVolumesG >( getResources()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult( false )
					, *m_lpvResult
					, m_lpvConfigUbo );
				m_lightPropagationVolumesG[i]->initialise();
			}

			if ( needLLpv && !m_layeredLightPropagationVolumes[i] )
			{
				m_layeredLightPropagationVolumes[i] = makeUnique< LayeredLightPropagationVolumes >( getResources()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult( false )
					, m_llpvResult
					, m_llpvConfigUbo );
				m_layeredLightPropagationVolumes[i]->initialise();
			}

			if ( needLLpvG && !m_layeredLightPropagationVolumesG[i] )
			{
				m_layeredLightPropagationVolumesG[i] = makeUnique< LayeredLightPropagationVolumesG >( getResources()
					, scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult( false )
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

			auto const & cache = scene.getLightCache();

			if ( m_directionalShadowMap )
			{
				rendtech::doPrepareShadowMap( cache
					, LightType::eDirectional
					, *m_shadowBuffer
					, *m_directionalShadowMap
					, m_activeShadowMaps
					, m_lightPropagationVolumes
					, m_lightPropagationVolumesG
					, m_layeredLightPropagationVolumes
					, m_layeredLightPropagationVolumesG
					, m_reflectiveShadowMaps.get()
					, updater );
			}

			if ( m_pointShadowMap )
			{
				rendtech::doPrepareShadowMap( cache
					, LightType::ePoint
					, *m_shadowBuffer
					, *m_pointShadowMap
					, m_activeShadowMaps
					, m_lightPropagationVolumes
					, m_lightPropagationVolumesG
					, m_layeredLightPropagationVolumes
					, m_layeredLightPropagationVolumesG
					, m_reflectiveShadowMaps.get()
					, updater );
			}

			if ( m_spotShadowMap )
			{
				rendtech::doPrepareShadowMap( cache
					, LightType::eSpot
					, *m_shadowBuffer
					, *m_spotShadowMap
					, m_activeShadowMaps
					, m_lightPropagationVolumes
					, m_lightPropagationVolumesG
					, m_layeredLightPropagationVolumes
					, m_layeredLightPropagationVolumesG
					, m_reflectiveShadowMaps.get()
					, updater );
			}
		}
#endif
	}

	void RenderTechnique::doUpdateShadowMaps( GpuUpdater & updater )const
	{
		for ( auto const & maps : m_activeShadowMaps )
		{
			for ( auto const & map : maps )
			{
				for ( auto const & [light, index] : map.ids )
				{
					updater.light = light;
					updater.index = index;
					map.shadowMap.get().update( updater );
				}
			}
		}
	}

	void RenderTechnique::doUpdateRsm( CpuUpdater & updater )
	{
		if ( m_reflectiveShadowMaps )
		{
			m_reflectiveShadowMaps->update( updater );
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

	SemaphoreWaitArray RenderTechnique::doRenderRSM( SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )
	{
		SemaphoreWaitArray result = semaphore;

		if ( m_renderTarget.getScene()->needsGlobalIllumination( GlobalIlluminationType::eRsm )
			&& m_reflectiveShadowMaps )
		{
			result = m_reflectiveShadowMaps->render( result, queue );
		}

		return result;
	}

	SemaphoreWaitArray RenderTechnique::doRenderLPV( SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )
	{
		SemaphoreWaitArray result = semaphore;

		if ( m_renderTarget.getScene()->needsGlobalIllumination()
			&& m_clearLpvRunnable )
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

	SemaphoreWaitArray RenderTechnique::doRenderShadowMaps( SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )const
	{
		SemaphoreWaitArray result = semaphore;

		if ( auto scene = m_renderTarget.getScene();
			scene && scene->hasShadows() )
		{
			for ( auto & array : m_activeShadowMaps )
			{
				for ( auto & shadowMap : array )
				{
					for ( auto & [_, index] : shadowMap.ids )
					{
						result = shadowMap.shadowMap.get().render( result, queue, index );
					}
				}
			}
		}

		return result;
	}

	SemaphoreWaitArray RenderTechnique::doRenderEnvironmentMaps( SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )const
	{
		SemaphoreWaitArray result = semaphore;

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

	SemaphoreWaitArray RenderTechnique::doRenderVCT( SemaphoreWaitArray const & semaphore
		, ashes::Queue const & queue )const
	{
		SemaphoreWaitArray result = semaphore;

		if ( auto scene = m_renderTarget.getScene();
			scene
				&& scene->getVoxelConeTracingConfig().enabled
				&& m_voxelizer )
		{
			result = m_voxelizer->render( result, queue );
		}

		return result;
	}
}

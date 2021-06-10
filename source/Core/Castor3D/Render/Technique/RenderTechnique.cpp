#include "Castor3D/Render/Technique/RenderTechnique.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ParticleSystemCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/BackgroundPass.hpp"
#include "Castor3D/Render/Passes/DepthPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"
#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"
#include "Castor3D/Render/Technique/Voxelize/Voxelizer.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

using namespace castor;

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

		struct VkImageViewCreateInfoComp
		{
			bool operator()( VkImageViewCreateInfo const & lhs
				, VkImageViewCreateInfo const & rhs )const
			{
				return ( lhs.image < rhs.image
					|| ( lhs.image == rhs.image
						&& ( lhs.subresourceRange.baseArrayLayer < rhs.subresourceRange.baseArrayLayer
							|| ( lhs.subresourceRange.baseArrayLayer == rhs.subresourceRange.baseArrayLayer
								&& ( lhs.subresourceRange.baseMipLevel < rhs.subresourceRange.baseMipLevel
									|| ( lhs.subresourceRange.baseMipLevel == rhs.subresourceRange.baseMipLevel
										&& ( lhs.subresourceRange.layerCount < rhs.subresourceRange.layerCount
											|| ( lhs.subresourceRange.layerCount == rhs.subresourceRange.layerCount
												&& ( lhs.subresourceRange.levelCount < rhs.subresourceRange.levelCount
													|| ( lhs.subresourceRange.levelCount == rhs.subresourceRange.levelCount
														&& ( lhs.viewType < rhs.viewType
															|| ( lhs.viewType == rhs.viewType
																&& ( lhs.format < rhs.format ) ) ) ) ) ) ) ) ) ) ) ) );

			}
		};

		class IntermediatesLister
			: public RenderTechniqueVisitor
		{
		public:
			static void submit( RenderTechnique & technique
				, IntermediateViewArray & intermediates )
			{
				std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > cache;

				PipelineFlags flags{};
				IntermediatesLister visOpaque{ flags, *technique.getRenderTarget().getScene(), cache, intermediates };
				technique.accept( visOpaque );

				flags.passFlags |= PassFlag::eAlphaBlending;
				IntermediatesLister visTransparent{ flags, *technique.getRenderTarget().getScene(), cache, intermediates };
				technique.accept( visTransparent );
			}

		private:
			IntermediatesLister( PipelineFlags const & flags
				, Scene const & scene
				, std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > & cache
				, IntermediateViewArray & result )
				: RenderTechniqueVisitor{ flags, scene, { true, false } }
				, m_result{ result }
				, m_cache{ cache }
			{
			}

			void doVisit( castor::String const & name
				, ashes::ImageView const & view
				, VkImageLayout layout
				, TextureFactors const & factors )override
			{
				m_cache.insert( view.createInfo );
				m_result.push_back( { name, view, layout, factors } );
			}

			bool doFilter( VkImageViewCreateInfo const & info )const override
			{
				return ( info.viewType == VK_IMAGE_VIEW_TYPE_3D
					|| ( info.viewType == VK_IMAGE_VIEW_TYPE_2D
						&& info.subresourceRange.baseMipLevel == 0u
						&& info.subresourceRange.levelCount == 1u
						&& info.subresourceRange.layerCount == 1u ) )
					&& m_cache.end() == m_cache.find( info );
			}

		private:
			IntermediateViewArray & m_result;
			std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > & m_cache;
		};

		std::map< double, LightSPtr > doSortLights( LightCache const & cache
			, LightType type
			, Camera const & camera )
		{
			using LockType = std::unique_lock< LightCache const >;
			LockType lock{ castor::makeUniqueLock( cache ) };
			std::map< double, LightSPtr > lights;

			for ( auto & light : cache.getLights( type ) )
			{
				light->setShadowMap( nullptr );

				if ( light->isShadowProducer()
					&& ( light->getLightType() == LightType::eDirectional
						|| camera.isVisible( light->getBoundingBox()
							, light->getParent()->getDerivedTransformationMatrix() ) ) )
				{
					lights.emplace( point::distanceSquared( camera.getParent()->getDerivedPosition()
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
					lightIt->second->setShadowMap( &shadowMap, index );
					active.second.push_back( index );
					updater.light = lightIt->second;
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
						layeredLightPropagationVolumes[size_t( type )]->registerLight( updater.light );
						break;
					case GlobalIlluminationType::eLayeredLpvG:
						layeredLightPropagationVolumesG[size_t( type )]->registerLight( updater.light );
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

		TextureUnit doCreateTextureUnit( RenderDevice const & device
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
			TextureUnit result{ engine };
			result.setTexture( colourTexture );
			result.setSampler( createSampler( engine
				, image.data->name
				, VK_FILTER_LINEAR
				, &range ) );
			result.initialise( device );
			return result;
		}

		LightVolumePassResultArray doCreateLLPVResult( crg::ResourceHandler & handler
			, RenderDevice const & device )
		{
			LightVolumePassResultArray result;
			auto & engine = *device.renderSystem.getEngine();

			for ( uint32_t i = 0u; i < shader::LpvMaxCascadesCount; ++i )
			{
				result.emplace_back( castor::makeUnique< LightVolumePassResult >( handler
					, device
					, castor::string::toString( i )
					, engine.getLpvGridSize() ) );
			}

			return result;
		}

		crg::FrameGraph doCreateClearLpvCommands( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & name
			, LightVolumePassResult  const & lpvResult
			, LightVolumePassResultArray const & llpvResult )
		{
			class LpvClear
				: public crg::RunnablePass
			{
			public:
				LpvClear( crg::FramePass const & pass
					, crg::GraphContext const & context
					, crg::RunnableGraph & graph )
					: crg::RunnablePass{ pass, context, graph }
				{
				}

			protected:
				void doInitialise()override
				{
				}

				void doRecordInto( VkCommandBuffer commandBuffer
					, uint32_t index )override
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

				VkPipelineStageFlags doGetSemaphoreWaitFlags()const override
				{
					return VK_PIPELINE_STAGE_TRANSFER_BIT;
				}
			};

			crg::FrameGraph result{ handler, name + "ClearLpv" };
			auto & pass = result.createPass( name + "LpvClear"
				, []( crg::FramePass const & pass
					, crg::GraphContext const & context
					, crg::RunnableGraph & graph )
				{
					return std::make_unique< LpvClear >( pass
						, context
						, graph );
				} );

			for ( auto & texture : lpvResult )
			{
				pass.addTransferOutputView( texture.wholeViewId
					, VK_IMAGE_LAYOUT_UNDEFINED );
			}

			for ( auto & textures : llpvResult )
			{
				for ( auto & texture : *textures )
				{
					pass.addTransferOutputView( texture.wholeViewId
						, VK_IMAGE_LAYOUT_UNDEFINED );
				}
			}

			return result;
		}
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( String const & name
		, RenderTarget & renderTarget
		, RenderDevice const & device
		, Parameters const & parameters
		, SsaoConfig const & ssaoConfig )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, Named{ name }
		, m_renderTarget{ renderTarget }
		, m_device{ device }
		, m_size{ m_renderTarget.getSize() }
		, m_ssaoConfig{ ssaoConfig }
		, m_colour{ device
			, getOwner()->getGraphResourceHandler()
			, "TechCol"
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_depth{ device
			, getOwner()->getGraphResourceHandler()
			, "TechDpt"
			, 0u
			, makeExtent3D( m_size )
			, 1u
			, 1u
			, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
				| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK }
		, m_colourTexture{ doCreateTextureUnit( m_device
			, m_colour.imageId ) }
		, m_depthBuffer{ doCreateTextureUnit( m_device
			, m_depth.imageId ) }
		, m_matrixUbo{ m_device }
		, m_sceneUbo{ m_device }
		, m_gpInfoUbo{ m_device }
		, m_lpvConfigUbo{ m_device }
		, m_llpvConfigUbo{ m_device }
		, m_vctConfigUbo{ m_device }
		, m_depthPassDecl{ &doCreateDepthPass() }
		, m_voxelizer{ castor::makeUnique< Voxelizer >( getOwner()->getGraphResourceHandler()
			, device
			, *m_renderTarget.getScene()
			, *m_renderTarget.getCamera()
			, m_matrixUbo
			, m_vctConfigUbo
			, m_renderTarget.getScene()->getVoxelConeTracingConfig() ) }
		, m_lpvResult{ castor::makeUnique< LightVolumePassResult >( getOwner()->getGraphResourceHandler()
			, m_device
			, cuEmptyString
			, getEngine()->getLpvGridSize() ) }
		, m_llpvResult{ doCreateLLPVResult( getOwner()->getGraphResourceHandler()
			, m_device ) }
		, m_backgroundPassDesc{ &doCreateBackgroundPass() }
#if !C3D_DebugDisableShadowMaps
		, m_directionalShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapDirectional >( getOwner()->getGraphResourceHandler()
			, m_device
			, *m_renderTarget.getScene() ) }
		, m_pointShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapPoint >( getOwner()->getGraphResourceHandler()
			, m_device
			, *m_renderTarget.getScene() ) }
		, m_spotShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapSpot >( getOwner()->getGraphResourceHandler()
			, m_device
			, *m_renderTarget.getScene() ) }
#endif
#if C3D_UseDeferredRendering
		, m_opaquePassResult{ castor::makeUnique< OpaquePassResult >( m_renderTarget.getGraph()
			, device
			, m_depthBuffer
			, m_renderTarget.getVelocity() ) }
		, m_opaquePassDesc{ &doCreateOpaquePass() }
		, m_deferredRendering{ castor::makeUnique< DeferredRendering >( m_renderTarget.getGraph()
			, m_device
			, static_cast< OpaquePass & >( *m_opaquePass )
			, *m_opaquePassResult
			, m_colourTexture
			, m_directionalShadowMap->getShadowPassResult()
			, m_pointShadowMap->getShadowPassResult()
			, m_spotShadowMap->getShadowPassResult()
			, *m_lpvResult
			, m_llpvResult
			, m_voxelizer->getFirstBounce()
			, m_voxelizer->getSecondaryBounce()
			, m_renderTarget.getSize()
			, *m_renderTarget.getScene()
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_vctConfigUbo
			, m_ssaoConfig )
}
#else
		, m_opaquePassDesc{ &doCreateOpaquePass() }
#endif
#if C3D_UseWeightedBlendedRendering
		, m_transparentPassResult{ castor::makeUnique< TransparentPassResult >( getOwner()->getGraphResourceHandler()
			, m_device
			, m_depth
			, m_renderTarget.getVelocityTexture() ) }
		, m_transparentPassDesc{ &doCreateTransparentPass() }
		, m_weightedBlendRendering{ castor::makeUnique< WeightedBlendRendering >( m_renderTarget.getGraph()
			, m_device
			, *m_transparentPassDesc
			, *m_transparentPassResult
			, m_colour.wholeViewId
			, m_renderTarget.getSize()
			, m_sceneUbo
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo )}
#else
		, m_transparentPassDesc{ &doCreateTransparentPass() }
#endif
		, m_signalFinished{ m_device->createSemaphore( "RenderTechnique" ) }
		, m_clearLpvGraph{ doCreateClearLpvCommands( getOwner()->getGraphResourceHandler(), device, getName(), *m_lpvResult, m_llpvResult ) }
		, m_clearLpvRunnable{ m_clearLpvGraph.compile( m_device.makeContext() ) }
		, m_particleTimer{ std::make_shared< RenderPassTimer >( device, cuT( "Particles" ), cuT( "Particles" ) ) }
	{
		m_clearLpvRunnable->record();
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
		m_opaquePassResult->cleanup();
		m_opaquePassResult.reset();
#endif
		m_llpvResult.clear();
		m_lpvResult.reset();
		m_voxelizer.reset();
		m_depthBuffer.cleanup();
		m_colourTexture.cleanup();
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
		m_deferredRendering.reset();
		m_weightedBlendRendering.reset();
	}

	void RenderTechnique::listIntermediates( IntermediateViewArray & intermediates )
	{
		IntermediatesLister::submit( *this, intermediates );
	}

	void RenderTechnique::update( CpuUpdater & updater )
	{
		if ( !m_depthPass )
		{
			return;
		}

		updater.camera = m_renderTarget.getCamera();
		updater.voxelConeTracing = m_renderTarget.getScene()->getVoxelConeTracingConfig().enabled;

		m_depthPass->update( updater );
		m_backgroundPass->update( updater );
		m_voxelizer->update( updater );

#if C3D_UseDeferredRendering
		m_deferredRendering->update( updater );
#else
		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).update( updater );
#endif
		static_cast< TransparentPassType & >( *m_transparentPass ).update( updater );

		if ( m_renderTarget.getScene()->getFog().getType() != FogType::eDisabled )
		{
			auto & background = m_renderTarget.getScene()->getColourBackground();
			background.update( updater );
		}
		else
		{
			auto & background = *m_renderTarget.getScene()->getBackground();
			background.update( updater );
		}

		auto & maps = m_renderTarget.getScene()->getEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().update( updater );
		}

		doUpdateShadowMaps( updater );
		doUpdateLpv( updater );
		doUpdateParticles( updater );

		auto & camera = *m_renderTarget.getCamera();
		auto jitter = m_renderTarget.getJitter();
		auto jitterProjSpace = jitter * 2.0f;
		jitterProjSpace[0] /= camera.getWidth();
		jitterProjSpace[1] /= camera.getHeight();
		m_matrixUbo.cpuUpdate( camera.getView()
			, camera.getProjection()
			, jitterProjSpace );
		m_sceneUbo.cpuUpdate( &camera
			, camera.getScene()->getFog() );
		m_gpInfoUbo.cpuUpdate( m_size
			, camera );
	}

	void RenderTechnique::update( GpuUpdater & updater )
	{
		if ( !m_depthPass )
		{
			return;
		}

		updater.scene = m_renderTarget.getScene();
		updater.camera = m_renderTarget.getCamera();
		updater.voxelConeTracing = m_renderTarget.getScene()->getVoxelConeTracingConfig().enabled;

		doInitialiseLpv();

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			map.get().update( updater );
		}

		m_depthPass->update( updater );
		m_backgroundPass->update( updater );

		if ( updater.voxelConeTracing )
		{
			m_voxelizer->update( updater );
		}

#if C3D_UseDeferredRendering
		m_deferredRendering->update( updater );
#else
		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).update( updater );
#endif
		static_cast< TransparentPassType & >( *m_transparentPass ).update( updater );

		if ( m_renderTarget.getScene()->getFog().getType() != FogType::eDisabled )
		{
			auto & background = m_renderTarget.getScene()->getColourBackground();
			background.update( updater );
		}
		else
		{
			auto & background = *m_renderTarget.getScene()->getBackground();
			background.update( updater );
		}

		doUpdateShadowMaps( updater );
		doUpdateLpv( updater );
		doUpdateParticles( updater );
	}

	crg::SemaphoreWait RenderTechnique::preRender( crg::SemaphoreWait const & toWait )
	{
		auto result = toWait;
		result = doRenderShadowMaps( result );
		result = doRenderLPV( result );
		result = doRenderEnvironmentMaps( result );
		result = doRenderVCT( result );
		return result;
	}

	bool RenderTechnique::writeInto( castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Technique Colour"
			, m_colourTexture.getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Technique Depth"
			, m_depthBuffer.getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		m_voxelizer->listIntermediates( visitor );

		if ( checkFlag( visitor.getFlags().passFlags, PassFlag::eAlphaBlending ) )
		{
#if C3D_UseWeightedBlendedRendering
			if ( m_weightedBlendRendering )
			{
				m_weightedBlendRendering->accept( visitor );
			}
#else
			if ( m_transparentPass )
			{
				m_transparentPass->accept( visitor );
			}
#endif
		}
		else
		{
#if C3D_UseDeferredRendering
			if ( m_deferredRendering )
			{
				m_deferredRendering->accept( visitor );
			}
#else
			if ( m_opaquePass )
			{
				m_opaquePass->accept( visitor );
			}
#endif
		}

#if !C3D_DebugDisableShadowMaps
		m_directionalShadowMap->accept( visitor );
		m_spotShadowMap->accept( visitor );
		m_pointShadowMap->accept( visitor );
#endif
	}

	crg::FramePass const & RenderTechnique::getLastPass()const
	{
#if C3D_UseWeightedBlendedRendering
		return m_weightedBlendRendering->getLastPass();
#else
		return *m_transparentPassDesc;
#endif
	}

	crg::FramePass & RenderTechnique::doCreateDepthPass()
	{
		auto & result = m_renderTarget.getGraph().createPass( "DepthPass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< DepthPass >( pass
					, context
					, graph
					, m_device
					, m_matrixUbo
					, m_renderTarget.getCuller()
					, m_ssaoConfig
					, m_depth.getExtent() );
				m_depthPass = result.get();
				return result;
			} );
		result.addOutputDepthView( m_depth.targetViewId
			, defaultClearDepthStencil );
		result.addOutputColourView( m_renderTarget.getVelocityId() );
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateBackgroundPass()
	{
		auto & background = *getRenderTarget().getScene()->getBackground();
		auto & result = m_renderTarget.getGraph().createPass( "BackgroundPass"
			, [this, &background]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< BackgroundPass >( pass
					, context
					, graph
					, m_device
					, background
					, makeExtent2D( m_colour.getExtent() ) );
				m_backgroundPass = result.get();
				return result;
			} );
		result.addDependency( *m_depthPassDecl );
		background.getMatrixUbo().createPassBinding( result
			, SceneBackground::MtxUboIdx );
		background.getModelUbo().createPassBinding( result
			, "Model"
			, SceneBackground::MdlMtxUboIdx );
		getRenderTarget().getHdrConfigUbo().createPassBinding( result
			, SceneBackground::HdrCfgUboIdx );
		result.addInOutDepthView( m_depth.targetViewId );
		result.addOutputColourView( m_colour.targetViewId );
		return result;
	}

	crg::FramePass & RenderTechnique::doCreateOpaquePass()
	{
#if C3D_UseDeferredRendering
		, m_opaquePass{ castor::makeUniqueDerived< RenderTechniquePass, OpaquePass >( m_device
			, m_size
			, m_matrixUbo
			, m_renderTarget.getCuller()
			, m_ssaoConfig
			, *m_opaquePassResult ) };
#else
		auto & result = m_renderTarget.getGraph().createPass( "OpaquePass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< ForwardRenderTechniquePass >( pass
					, context
					, graph
					, m_device
					, cuT( "Opaque" )
					, cuT( "Forward" )
					, SceneRenderPassDesc{ { m_size.getWidth(), m_size.getHeight(), 1u }, m_matrixUbo, m_renderTarget.getCuller() }
					, RenderTechniquePassDesc{ false, m_ssaoConfig }
						.lpvConfigUbo( m_lpvConfigUbo )
						.llpvConfigUbo( m_llpvConfigUbo )
						.vctConfigUbo( m_vctConfigUbo )
						.lpvResult( *m_lpvResult )
						.llpvResult( m_llpvResult )
						.vctFirstBounce( m_voxelizer->getFirstBounce() )
						.vctSecondaryBounce( m_voxelizer->getSecondaryBounce() ) );
				m_opaquePass = result.get();
				return result;
			} );
		result.addDependency( *m_backgroundPassDesc );
		result.addInOutDepthView( m_depth.targetViewId );
		result.addInOutColourView( m_colour.targetViewId );
		return result;
#endif
	}

	crg::FramePass & RenderTechnique::doCreateTransparentPass()
	{
		auto & result = m_renderTarget.getGraph().createPass( "TransparentPass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
#if C3D_UseWeightedBlendedRendering
				castor::String name = cuT( "Accumulation" );
				bool isOit = true;
#else
				castor::String name = cuT( "Forward" );
				bool isOit = false;
#endif
				auto result = std::make_unique< TransparentPassType >( pass
					, context
					, graph
					, m_device
					, cuT( "Transparent" )
					, name
					, SceneRenderPassDesc{ { m_size.getWidth(), m_size.getHeight(), 1u }, m_matrixUbo, m_renderTarget.getCuller(), isOit }
					, RenderTechniquePassDesc{ false, m_ssaoConfig }
						.lpvConfigUbo( m_lpvConfigUbo )
						.llpvConfigUbo( m_llpvConfigUbo )
						.vctConfigUbo( m_vctConfigUbo )
						.lpvResult( *m_lpvResult )
						.llpvResult( m_llpvResult )
						.vctFirstBounce( m_voxelizer->getFirstBounce() )
						.vctSecondaryBounce( m_voxelizer->getSecondaryBounce() )
						.hasVelocity( true ) );
				m_transparentPass = result.get();
				return result;
			} );
		result.addDependency( *m_opaquePassDesc );
		result.addInOutDepthView( m_depth.targetViewId );

#if C3D_UseWeightedBlendedRendering
		auto & transparentPassResult = *m_transparentPassResult;
		result.addInOutDepthStencilView( transparentPassResult[WbTexture::eDepth].targetViewId );
		result.addOutputColourView( transparentPassResult[WbTexture::eAccumulation].targetViewId
			, getClearValue( WbTexture::eAccumulation ) );
		result.addOutputColourView( transparentPassResult[WbTexture::eRevealage].targetViewId
			, getClearValue( WbTexture::eRevealage ) );
		result.addInOutColourView( transparentPassResult[WbTexture::eVelocity].targetViewId );
#else
		result.addInOutColourView( m_colourView );
#endif

		return result;
	}

	void RenderTechnique::doInitialiseLpv()
	{
		auto & graph = m_renderTarget.getGraph();
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

		if ( !cache.isEmpty() )
		{
			updater.index = 0u;

			for ( auto & particleSystem : cache )
			{
				particleSystem.second->update( updater );
			}
		}
	}
	
	void RenderTechnique::doUpdateParticles( GpuUpdater & updater )
	{
		auto & cache = updater.scene->getParticleSystemCache();
		auto lock( castor::makeUniqueLock( cache ) );

		if ( !cache.isEmpty() )
		{
			auto count = 2u * cache.getObjectCount();

			if ( m_particleTimer->getCount() < count )
			{
				m_particleTimer->updateCount( count );
			}

			RenderPassTimerBlock timerBlock{ m_particleTimer->start() };
			updater.index = 0u;
			updater.timer = m_particleTimer.get();

			for ( auto & particleSystem : cache )
			{
				particleSystem.second->update( updater );
				updater.info.m_particlesCount += particleSystem.second->getParticlesCount();
			}
		}
	}

	crg::SemaphoreWait RenderTechnique::doRenderLPV( crg::SemaphoreWait const & semaphore )
	{
		crg::SemaphoreWait result = semaphore;

		if ( m_renderTarget.getScene()->needsGlobalIllumination() )
		{
			result = m_clearLpvRunnable->run( result, *m_device.graphicsQueue );

			for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
			{
				if ( m_lightPropagationVolumes[i] )
				{
					result = m_lightPropagationVolumes[i]->render( result );
				}

				if ( m_lightPropagationVolumesG[i] )
				{
					result = m_lightPropagationVolumesG[i]->render( result );
				}

				if ( m_layeredLightPropagationVolumes[i] )
				{
					result = m_layeredLightPropagationVolumes[i]->render( result );
				}

				if ( m_layeredLightPropagationVolumesG[i] )
				{
					result = m_layeredLightPropagationVolumesG[i]->render( result );
				}
			}
		}

		return result;
	}

	crg::SemaphoreWait RenderTechnique::doRenderShadowMaps( crg::SemaphoreWait const & semaphore )const
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
						result = shadowMap.first.get().render( result, index );
					}
				}
			}
		}

		return result;
	}

	crg::SemaphoreWait RenderTechnique::doRenderEnvironmentMaps( crg::SemaphoreWait const & semaphore )const
	{
		crg::SemaphoreWait result = semaphore;

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			result = map.get().render( result );
		}

		return result;
	}

	crg::SemaphoreWait RenderTechnique::doRenderVCT( crg::SemaphoreWait const & semaphore )const
	{
		crg::SemaphoreWait result = semaphore;

		if ( m_renderTarget.getScene()->getVoxelConeTracingConfig().enabled )
		{
			result = m_voxelizer->render( result );
		}

		return result;
	}
}

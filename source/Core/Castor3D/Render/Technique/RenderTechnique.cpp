#include "Castor3D/Render/Technique/RenderTechnique.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardUboPools.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/ParticleSystemCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/DepthPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"
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
		class IntermediatesLister
			: public RenderTechniqueVisitor
		{
		public:
			static void submit( RenderTechnique & technique
				, std::vector< IntermediateView > & intermediates )
			{
				PipelineFlags flags{};
				IntermediatesLister visOpaque{ flags, *technique.getRenderTarget().getScene(), intermediates };
				technique.accept( visOpaque );

				flags.passFlags |= PassFlag::eAlphaBlending;
				IntermediatesLister visTransparent{ flags, *technique.getRenderTarget().getScene(), intermediates };
				technique.accept( visTransparent );
			}

		private:
			IntermediatesLister( PipelineFlags const & flags
				, Scene const & scene
				, std::vector< IntermediateView > & result )
				: RenderTechniqueVisitor{ flags, scene, { true, false } }
				, m_result{ result }
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
				return info.viewType == VK_IMAGE_VIEW_TYPE_2D
					&& info.subresourceRange.baseMipLevel == 0u
					&& info.subresourceRange.levelCount == 1u
					&& info.subresourceRange.layerCount == 1u
					&& m_cache.end() == m_cache.find( info );
			}

		private:
			std::vector< IntermediateView > & m_result;
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
			std::set< VkImageViewCreateInfo, VkImageViewCreateInfoComp > m_cache;
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
				if ( !shadowMap.isInitialised() )
				{
					cache.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [&shadowMap]( RenderDevice const & device )
						{
							shadowMap.initialise( device );
						} ) );
				}

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
			, Size const & size
			, VkFormat format
			, VkImageUsageFlags usage
			, castor::String debugName )
		{
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				makeExtent3D( size ),
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				usage | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			auto result = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, std::move( debugName ) );
			result->initialise( device );
			return result;
		}
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( String const & name
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & parameters
		, SsaoConfig const & ssaoConfig )
		: OwnedBy< Engine >{ *renderSystem.getEngine() }
		, Named{ name }
		, m_renderTarget{ renderTarget }
		, m_renderSystem{ renderSystem }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_gpInfoUbo{ *renderSystem.getEngine() }
		, m_lpvConfigUbo{}
		, m_llpvConfigUbo{}
#if C3D_UseDeferredRendering
		, m_opaquePass{ castor::makeUniqueDerived< RenderTechniquePass, OpaquePass >( m_matrixUbo
			, renderTarget.getCuller()
			, ssaoConfig ) }
#else
		, m_opaquePass{ castor::makeUniqueDerived< RenderTechniquePass, ForwardRenderTechniquePass >( cuT( "opaque_pass" )
			, m_matrixUbo
			, renderTarget.getCuller()
			, false
			, nullptr
			, ssaoConfig
			, &m_lpvConfigUbo
			, &m_llpvConfigUbo ) }
#endif
#if C3D_UseWeightedBlendedRendering
		, m_transparentPass{ castor::makeUniqueDerived< RenderTechniquePass, TransparentPass >( m_matrixUbo
			, renderTarget.getCuller()
			, ssaoConfig
			, m_lpvConfigUbo
			, m_llpvConfigUbo ) }
#else
		, m_transparentPass{ castor::makeUniqueDerived< RenderTechniquePass, ForwardRenderTechniquePass >( cuT( "transparent_pass" )
			, m_matrixUbo
			, renderTarget.getCuller()
			, false
			, false
			, nullptr
			, ssaoConfig
			, &m_lpvConfigUbo
			, &m_llpvConfigUbo ) }
#endif
		, m_initialised{ false }
		, m_ssaoConfig{ ssaoConfig }
		, m_colourTexture{ *renderSystem.getEngine() }
		, m_depthBuffer{ *renderSystem.getEngine() }
	{
		doCreateShadowMaps();
	}

	RenderTechnique::~RenderTechnique()
	{
		m_cbgCommandBuffer.reset();
		m_bgCommandBuffer.reset();
		m_bgFrameBuffer.reset();
		m_bgRenderPass.reset();
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
		m_transparentPass.reset();
		m_opaquePass.reset();
	}

	bool RenderTechnique::initialise( RenderDevice const & device
		, std::vector< IntermediateView > & intermediates )
	{
		if ( !m_initialised )
		{
			VkImageSubresourceRange range{ 0u, 0u, 1u, 0u, 1u };
			m_size = m_renderTarget.getSize();
			auto colourTexture = doCreateTexture( *getEngine()
				, device
				, m_size
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
				, cuT( "RenderTechnique_Colour" ) );
			m_colourTexture.setTexture( colourTexture );
			m_colourTexture.setSampler( createSampler( *getEngine()
				, cuT( "RenderTechnique_Colour" )
				, VK_FILTER_LINEAR
				, &range ) );

			auto depthBuffer = doCreateTexture( *getEngine()
				, device
				, m_size
				, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
					| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
				, ( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
				, cuT( "RenderTechnique_Depth" ) );
			m_depthBuffer.setTexture( depthBuffer );
			m_depthBuffer.setSampler( createSampler( *getEngine()
				, cuT( "RenderTechnique_Depth" )
				, VK_FILTER_LINEAR
				, &range ) );

			m_colourTexture.initialise( device );
			m_depthBuffer.initialise( device );
			m_signalFinished = device->createSemaphore( "RenderTechnique" );
			m_gpInfoUbo.initialise( device );
			m_matrixUbo.initialise( device );

			//m_voxelizer = std::make_unique< Voxelizer >( *m_renderSystem.getEngine()
			//	, VkExtent3D{ 64u, 64u, 64u }
			//	, *m_renderTarget.getScene()
			//	, m_renderTarget.getCuller()
			//	, m_renderTarget.getTexture().getTexture()->getDefaultView().getTargetView() );

			auto & maps = m_renderTarget.getScene()->getEnvironmentMaps();

			doInitialiseShadowMaps( device );
			doCreateLpv( device );
			doInitialiseLpv( device );
#if C3D_UseDepthPrepass
			doInitialiseDepthPass( device );
#endif
			doInitialiseBackgroundPass( device );
			doInitialiseOpaquePass( device );
			doInitialiseTransparentPass( device );

			for ( auto & map : maps )
			{
				map.get().initialise( device );
			}

			m_particleTimer = std::make_shared< RenderPassTimer >( *getEngine(), device, cuT( "Particles" ), cuT( "Particles" ) );
			IntermediatesLister::submit( *this, intermediates );
			m_initialised = true;
		}

		return m_initialised;
	}

	void RenderTechnique::cleanup( RenderDevice const & device )
	{
		m_onBgChanged.disconnect();
		m_bgCommandBuffer.reset();
		m_cbgCommandBuffer.reset();
		m_bgFrameBuffer.reset();
		m_bgRenderPass.reset();
		m_particleTimer.reset();
		m_weightedBlendRendering.reset();
		m_deferredRendering.reset();
		doCleanupLpv( device );
		doCleanupShadowMaps( device );
		m_transparentPass->cleanup( device );
		m_opaquePass->cleanup( device );
		m_voxelizer.reset();
#if C3D_UseDepthPrepass
		m_depthPass->cleanup( device );
		m_depthPass.reset();
#endif
		m_gpInfoUbo.cleanup( device );
		m_matrixUbo.cleanup( device );
		m_initialised = false;
		m_depthBuffer.cleanup();
		m_colourTexture.cleanup();
	}

	void RenderTechnique::update( CpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

		updater.camera = m_renderTarget.getCamera();

#if C3D_UseDepthPrepass
		m_depthPass->update( updater );
#endif
		//m_voxelizer->update( updater );
#if C3D_UseDeferredRendering
		m_deferredRendering->update( updater );
#else
		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).update( updater );
#endif
#if C3D_UseWeightedBlendedRendering
		m_weightedBlendRendering->update( updater );
#else
		static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).update( updater );
#endif

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
		m_gpInfoUbo.cpuUpdate( m_size
			, camera );
	}

	void RenderTechnique::update( GpuUpdater & updater )
	{
		if ( !m_initialised )
		{
			return;
		}

		updater.scene = m_renderTarget.getScene();
		updater.camera = m_renderTarget.getCamera();

		doInitialiseShadowMaps( updater.device );
		doInitialiseLpv( updater.device );

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			map.get().update( updater );
		}

#if C3D_UseDepthPrepass
		m_depthPass->update( updater );
#endif
		//m_voxelizer->update( updater );
#if C3D_UseDeferredRendering
		m_deferredRendering->update( updater );
#else
		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).update( updater );
#endif
#if C3D_UseWeightedBlendedRendering
		m_weightedBlendRendering->update( updater );
#else
		static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).update( updater );
#endif

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

	ashes::Semaphore const & RenderTechnique::render( RenderDevice const & device
		, ashes::SemaphoreCRefArray const & waitSemaphores
		, RenderInfo & info )
	{
		if ( !m_initialised )
		{
			return waitSemaphores.front();
		}

#if C3D_UseDepthPrepass
		auto * semaphore = &doRenderDepth( device, waitSemaphores );
		semaphore = &doRenderBackground( device, *semaphore );
#else
		auto * semaphore = &doRenderBackground( device, waitSemaphores );
#endif
		semaphore = &doRenderEnvironmentMaps( device, *semaphore );
		semaphore = &doRenderShadowMaps( device, *semaphore );
		semaphore = &doRenderLpv( device, *semaphore );

		// Render part
		//semaphore = &m_voxelizer->render( *semaphore );
		semaphore = &doRenderOpaque( device, *semaphore );
		semaphore = &doRenderTransparent( device, *semaphore );
		return *semaphore;
	}

	bool RenderTechnique::writeInto( castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Technique Colour"
			, m_colourTexture.getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		visitor.visit( "Technique Depth"
			, m_depthBuffer.getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		if ( checkFlag( visitor.getFlags().passFlags, PassFlag::eAlphaBlending ) )
		{
#if C3D_UseWeightedBlendedRendering
			m_weightedBlendRendering->accept( visitor );
#else
			m_transparentPass->accept( visitor );
#endif
		}
		else
		{
#if C3D_UseDeferredRendering
			m_deferredRendering->accept( visitor );
#else
			m_opaquePass->accept( visitor );
#endif
		}

		m_directionalShadowMap->accept( visitor );
		m_spotShadowMap->accept( visitor );
		m_pointShadowMap->accept( visitor );
	}

	void RenderTechnique::doCreateShadowMaps()
	{
		auto & scene = *m_renderTarget.getScene();
		m_directionalShadowMap = castor::makeUniqueDerived< ShadowMap, ShadowMapDirectional >( scene );
		m_allShadowMaps[size_t( LightType::eDirectional )].emplace_back( std::ref( *m_directionalShadowMap ), UInt32Array{} );
		m_spotShadowMap = castor::makeUniqueDerived< ShadowMap, ShadowMapSpot >( scene );
		m_allShadowMaps[size_t( LightType::eSpot )].emplace_back( std::ref( *m_spotShadowMap ), UInt32Array{} );
		m_pointShadowMap = castor::makeUniqueDerived< ShadowMap, ShadowMapPoint >( scene );
		m_allShadowMaps[size_t( LightType::ePoint )].emplace_back( std::ref( *m_pointShadowMap ), UInt32Array{} );
	}

	void RenderTechnique::doCreateLpv( RenderDevice const & device )
	{
		m_lpvConfigUbo.initialise( device );
		m_llpvConfigUbo.initialise( device );

		auto & scene = *m_renderTarget.getScene();
		m_lpvResult = castor::makeUnique< LightVolumePassResult >( *getEngine()
			, device
			, cuEmptyString
			, getEngine()->getLpvGridSize() );

		for ( uint32_t i = 0u; i < shader::LpvMaxCascadesCount; ++i )
		{
			m_llpvResult.emplace_back( castor::makeUnique< LightVolumePassResult >( *getEngine()
				, device
				, castor::string::toString( i )
				, getEngine()->getLpvGridSize() ) );
		}

		for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
		{
			m_lightPropagationVolumes[i] = castor::makeUnique< LightPropagationVolumes >( scene
				, LightType( i )
				, device
				, m_allShadowMaps[i].front().first.get().getShadowPassResult()
				, *m_lpvResult
				, m_lpvConfigUbo );
			m_layeredLightPropagationVolumes[i] = castor::makeUnique< LayeredLightPropagationVolumes >( scene
				, LightType( i )
				, device
				, m_allShadowMaps[i].front().first.get().getShadowPassResult()
				, m_llpvResult
				, m_llpvConfigUbo );
			m_lightPropagationVolumesG[i] = castor::makeUnique< LightPropagationVolumesG >( scene
				, LightType( i )
				, device
				, m_allShadowMaps[i].front().first.get().getShadowPassResult()
				, *m_lpvResult
				, m_lpvConfigUbo );
			m_layeredLightPropagationVolumesG[i] = castor::makeUnique< LayeredLightPropagationVolumesG >( scene
				, LightType( i )
				, device
				, m_allShadowMaps[i].front().first.get().getShadowPassResult()
				, m_llpvResult
				, m_llpvConfigUbo );
		}

		m_clearLpv = CommandsSemaphore{ device
			, this->getName() + cuT( "ClearLpv" ) };
		ashes::CommandBuffer & cmd = *m_clearLpv.commandBuffer;
		auto clearTex = [&cmd]( LightVolumePassResult const & result
			, LpvTexture tex )
		{
			cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, result[tex].getTexture()->getDefaultView().getSampledView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			cmd.clear( result[tex].getTexture()->getDefaultView().getSampledView(), getClearValue( tex ).color );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, result[tex].getTexture()->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		};
		cmd.begin();
		cmd.beginDebugBlock(
			{
				"Lighting - " + this->getName() + " - Clear Injection",
				castor3d::makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ),
			} );
		clearTex( *m_lpvResult, LpvTexture::eR );
		clearTex( *m_lpvResult, LpvTexture::eG );
		clearTex( *m_lpvResult, LpvTexture::eB );

		for ( auto & lpvResult : m_llpvResult )
		{
			clearTex( *lpvResult, LpvTexture::eR );
			clearTex( *lpvResult, LpvTexture::eG );
			clearTex( *lpvResult, LpvTexture::eB );
		}

		cmd.endDebugBlock();
		cmd.end();
	}

	void RenderTechnique::doInitialiseShadowMaps( RenderDevice const & device )
	{
		m_directionalShadowMap->initialise( device );
		m_spotShadowMap->initialise( device );
		m_pointShadowMap->initialise( device );
	}

	void RenderTechnique::doInitialiseLpv( RenderDevice const & device )
	{
		for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
		{
			m_lightPropagationVolumes[i]->initialise();
			m_lightPropagationVolumesG[i]->initialise();
			m_layeredLightPropagationVolumes[i]->initialise();
			m_layeredLightPropagationVolumesG[i]->initialise();
		}
	}

	void RenderTechnique::doInitialiseBackgroundPass( RenderDevice const & device )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		m_bgCommandBuffer = device.graphicsCommandPool->createCommandBuffer( "Background" );
		m_cbgCommandBuffer = device.graphicsCommandPool->createCommandBuffer( "ColourBackground" );
		auto depthLoadOp = C3D_UseDepthPrepass
			? VK_ATTACHMENT_LOAD_OP_LOAD
			: VK_ATTACHMENT_LOAD_OP_CLEAR;
		auto depthInitialLayout = C3D_UseDepthPrepass
			? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			: VK_IMAGE_LAYOUT_UNDEFINED;

		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				m_depthBuffer.getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				depthLoadOp,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				depthInitialLayout,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
			{
				0u,
				m_colourTexture.getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attachments ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_bgRenderPass = device->createRenderPass( "Background"
			, std::move( createInfo ) );

		ashes::ImageViewCRefArray attaches
		{
			m_depthBuffer.getTexture()->getDefaultView().getTargetView(),
			m_colourTexture.getTexture()->getDefaultView().getTargetView(),
		};
		m_bgFrameBuffer = m_bgRenderPass->createFrameBuffer( "Background"
			, { m_depthBuffer.getTexture()->getWidth(), m_depthBuffer.getTexture()->getHeight() }
			, std::move( attaches ) );

		auto & background = *m_renderTarget.getScene()->getBackground();
		auto prepareBackground = [&background, this]( RenderDevice const & device )
		{
			background.initialise( device, *m_bgRenderPass, m_renderTarget.getHdrConfigUbo() );
			background.prepareFrame( *m_bgCommandBuffer
				, Size{ m_colourTexture.getTexture()->getWidth(), m_colourTexture.getTexture()->getHeight() }
				, *m_bgRenderPass
				, *m_bgFrameBuffer );
		};
		prepareBackground( device );
		m_onBgChanged = background.onChanged.connect( [prepareBackground, this]( SceneBackground const & )
			{
				getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender, prepareBackground ) );
			} );
		auto & cbackground = m_renderTarget.getScene()->getColourBackground();
		auto prepareCBackground = [&cbackground, this]( RenderDevice const & device )
		{
			cbackground.initialise( device, *m_bgRenderPass, m_renderTarget.getHdrConfigUbo() );
			cbackground.prepareFrame( *m_cbgCommandBuffer
				, Size{ m_colourTexture.getTexture()->getWidth(), m_colourTexture.getTexture()->getHeight() }
				, *m_bgRenderPass
				, *m_bgFrameBuffer );
		};
		prepareCBackground( device );
		m_onCBgChanged = cbackground.onChanged.connect( [prepareCBackground, this]( SceneBackground const & )
			{
				getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender, prepareCBackground ) );
			} );
	}

#if C3D_UseDepthPrepass

	void RenderTechnique::doInitialiseDepthPass( RenderDevice const & device )
	{
		m_depthPass = castor::makeUnique< DepthPass >( cuT( "Depth Prepass" )
			, device
			, m_matrixUbo
			, m_renderTarget.getCuller()
			, m_ssaoConfig
			, m_depthBuffer.getTexture() );
		m_depthPass->initialise( device, m_size, nullptr );
	}

#endif

	void RenderTechnique::doInitialiseOpaquePass( RenderDevice const & device )
	{

#if C3D_UseDeferredRendering

		m_opaquePass->initialise( device, m_size, nullptr );
		m_deferredRendering = castor::makeUnique< DeferredRendering >( *getEngine()
			, device
			, static_cast< OpaquePass & >( *m_opaquePass )
			, m_depthBuffer
			, m_renderTarget.getVelocity()
			, m_colourTexture
			, m_directionalShadowMap->getShadowPassResult()
			, m_pointShadowMap->getShadowPassResult()
			, m_spotShadowMap->getShadowPassResult()
			, *m_lpvResult
			, m_llpvResult
			, m_renderTarget.getSize()
			, *m_renderTarget.getScene()
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_ssaoConfig );

#else

		m_opaquePass->initialise( device, m_size, m_lpvResult.get() );
		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).initialiseRenderPass( device
			, m_colourTexture.getTexture()->getDefaultView().getTargetView()
			, m_depthBuffer.getTexture()->getDefaultView().getTargetView()
			, m_size
			, false );

#endif

		m_colorTexTransition = { device, "TechniqueColourTexTransition" };
		auto & cmd = *m_colorTexTransition.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock(
			{
				"Technique Colour Texture Transition",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		cmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_colourTexture.getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		cmd.memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_renderTarget.getVelocity().getTexture()->getDefaultView().getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		cmd.clear( m_renderTarget.getVelocity().getTexture()->getDefaultView().getTargetView()
			, transparentBlackClearColor.color );
		cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_renderTarget.getVelocity().getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		cmd.endDebugBlock();
		cmd.end();
	}

	void RenderTechnique::doInitialiseTransparentPass( RenderDevice const & device )
	{

#if C3D_UseWeightedBlendedRendering

		m_weightedBlendRendering = castor::makeUnique< WeightedBlendRendering >( *getEngine()
			, device
			, static_cast< TransparentPass & >( *m_transparentPass )
			, m_depthBuffer
			, m_colourTexture.getTexture()->getDefaultView().getTargetView()
			, m_renderTarget.getVelocity()
			, m_renderTarget.getSize()
			, *m_renderTarget.getScene()
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo
			, *m_lpvResult );

#else

		static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).initialiseRenderPass( device
			, m_colourTexture.getTexture()->getDefaultView().getTargetView()
			, m_depthBuffer.getTexture()->getDefaultView().getTargetView()
			, m_size
			, false );
		m_transparentPass->initialise( device, m_size, m_lpvResult.get() ); 

#endif
	}

	void RenderTechnique::doCleanupShadowMaps( RenderDevice const & device )
	{
		m_directionalShadowMap->cleanup( device );
		m_spotShadowMap->cleanup( device );
		m_pointShadowMap->cleanup( device );
	}

	void RenderTechnique::doCleanupLpv( RenderDevice const & device )
	{
		m_clearLpv = {};

		for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
		{
			m_lightPropagationVolumes[i]->cleanup();
			m_lightPropagationVolumesG[i]->cleanup();
			m_layeredLightPropagationVolumes[i]->cleanup();
			m_layeredLightPropagationVolumesG[i]->cleanup();
		}

		m_lpvConfigUbo.cleanup();
		m_llpvConfigUbo.cleanup();
		m_llpvResult.clear();
		m_lpvResult.reset();
	}

	void RenderTechnique::doUpdateShadowMaps( CpuUpdater & updater )
	{
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
			m_lightPropagationVolumes[i]->update( updater );
			m_lightPropagationVolumesG[i]->update( updater );
			m_layeredLightPropagationVolumes[i]->update( updater );
			m_layeredLightPropagationVolumesG[i]->update( updater );
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

	ashes::Semaphore const & RenderTechnique::doRenderShadowMaps( RenderDevice const & device
		, ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;
		auto & scene = *m_renderTarget.getScene();

		if ( scene.hasShadows() )
		{
			for ( auto & array : m_activeShadowMaps )
			{
				for ( auto & shadowMap : array )
				{
					for ( auto & index : shadowMap.second )
					{
						result = &shadowMap.first.get().render( device, *result, index );
					}
				}
			}
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderLpv( RenderDevice const & device
		, ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		if ( m_renderTarget.getScene()->needsGlobalIllumination() )
		{
			result = &m_clearLpv.submit( *device.graphicsQueue, *result );

			for ( auto i = uint32_t( LightType::eMin ); i < uint32_t( LightType::eCount ); ++i )
			{
				result = &m_lightPropagationVolumes[i]->render( *result );
				result = &m_lightPropagationVolumesG[i]->render( *result );
				result = &m_layeredLightPropagationVolumes[i]->render( *result );
				result = &m_layeredLightPropagationVolumesG[i]->render( *result );
			}
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderEnvironmentMaps( RenderDevice const & device
		, ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			result = &map.get().render( device, *result );
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderDepth( RenderDevice const & device
		, ashes::SemaphoreCRefArray const & semaphores )
	{
		return m_depthPass->render( device, semaphores );
	}

	ashes::Semaphore const & RenderTechnique::doRenderBackground( RenderDevice const & device
		, ashes::Semaphore const & semaphore )
	{
		return doRenderBackground( device, { 1u, std::ref( semaphore ) } );
	}

	ashes::Semaphore const & RenderTechnique::doRenderBackground( RenderDevice const & device
		, ashes::SemaphoreCRefArray const & semaphores )
	{
		auto const & queue = *device.graphicsQueue;
		ashes::VkPipelineStageFlagsArray const stages( semaphores.size(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );

		if ( m_renderTarget.getScene()->getFog().getType() != FogType::eDisabled )
		{
			auto & background = m_renderTarget.getScene()->getColourBackground();
			auto & bgSemaphore = background.getSemaphore();
			auto timerBlock = background.start();
			timerBlock->notifyPassRender();
			queue.submit( { *m_cbgCommandBuffer }
				, semaphores
				, stages
				, { bgSemaphore }
				, nullptr );
			return bgSemaphore;
		}

		auto & background = *m_renderTarget.getScene()->getBackground();
		auto & bgSemaphore = background.getSemaphore();
		auto timerBlock = background.start();
		timerBlock->notifyPassRender();
		queue.submit( { *m_bgCommandBuffer }
			, semaphores
			, stages
			, { bgSemaphore }
			, nullptr );
		return bgSemaphore;
	}

	ashes::Semaphore const & RenderTechnique::doRenderOpaque( RenderDevice const & device
		, ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		if ( m_opaquePass->hasNodes() )
		{
			auto & scene = *m_renderTarget.getScene();
			auto & camera = *m_renderTarget.getCamera();

#if C3D_UseDeferredRendering
			result = &m_deferredRendering->render( device, scene, camera, *result );
#else
			result = &static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).render( device, *result );
#endif
		}
		else
		{
			result = &m_colorTexTransition.submit( *device.graphicsQueue, *result );
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderTransparent( RenderDevice const & device
		, ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		if ( m_transparentPass->hasNodes() )
		{
			auto & scene = *m_renderTarget.getScene();

#if C3D_UseWeightedBlendedRendering
			result = &m_weightedBlendRendering->render( device, scene, *result );
#else
			result = &static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).render( device, *result );
#endif
		}

		return *result;
	}
}

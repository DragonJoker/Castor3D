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
				, std::vector< IntermediateView > & intermediates )
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
				, std::vector< IntermediateView > & result )
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
			std::vector< IntermediateView > & m_result;
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
				if ( !shadowMap.isInitialised() )
				{
					cache.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [&shadowMap]( RenderDevice const & device )
						{
							shadowMap.initialise();
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
			ashes::ImageCreateInfo image{ 0u
				, VK_IMAGE_TYPE_2D
				, format
				, makeExtent3D( size )
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
			, castor::String const & name
			, VkFormat format
			, VkImageUsageFlags usage
			, castor::Size const & dimensions )
		{
			VkImageSubresourceRange range{ 0u, 0u, 1u, 0u, 1u };
			auto & engine = *device.renderSystem.getEngine();
			auto colourTexture = doCreateTexture( engine
				, device
				, dimensions
				, format
				, usage
				, name );
			TextureUnit result{ engine };
			result.setTexture( colourTexture );
			result.setSampler( createSampler( engine
				, name
				, VK_FILTER_LINEAR
				, &range ) );
			result.initialise( device );
			return result;
		}

		LightVolumePassResultArray doCreateLLPVResult( RenderDevice const & device )
		{
			LightVolumePassResultArray result;
			auto & engine = *device.renderSystem.getEngine();

			for ( uint32_t i = 0u; i < shader::LpvMaxCascadesCount; ++i )
			{
				result.emplace_back( castor::makeUnique< LightVolumePassResult >( engine
					, device
					, castor::string::toString( i )
					, engine.getLpvGridSize() ) );
			}

			return result;
		}

		CommandsSemaphore doCreateClearLpvCommands( RenderDevice const & device
			, castor::String const & name
			, LightVolumePassResult  const & lpvResult
			, LightVolumePassResultArray const & llpvResult )
		{
			CommandsSemaphore result{ device
				, name + cuT( "ClearLpv" ) };
			ashes::CommandBuffer & cmd = *result.commandBuffer;
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
					"Lighting - " + name + " - Clear Injection",
					castor3d::makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ),
				} );
			clearTex( lpvResult, LpvTexture::eR );
			clearTex( lpvResult, LpvTexture::eG );
			clearTex( lpvResult, LpvTexture::eB );

			for ( auto & lpvResult : llpvResult )
			{
				clearTex( *lpvResult, LpvTexture::eR );
				clearTex( *lpvResult, LpvTexture::eG );
				clearTex( *lpvResult, LpvTexture::eB );
			}

			cmd.endDebugBlock();
			cmd.end();
			return result;
		}

		ashes::RenderPassPtr createBgRenderPass( RenderDevice const & device
			, VkFormat depthFormat
			, VkFormat colourFormat )
		{
			ashes::VkAttachmentDescriptionArray attachments
			{
				{ 0u
					, depthFormat
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_LOAD
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL }
				, { 0u
					, colourFormat
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription{ 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, { { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
				, {}
				, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
				, {} } );
			ashes::VkSubpassDependencyArray dependencies
			{
				{ VK_SUBPASS_EXTERNAL
					, 0u
					, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_ACCESS_MEMORY_READ_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
					, VK_ACCESS_MEMORY_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo{ 0u
				, std::move( attachments )
				, std::move( subpasses )
				, std::move( dependencies ) };
			return device->createRenderPass( "Background"
				, std::move( createInfo ) );
		}

		ashes::FrameBufferPtr doCreateBgFramebuffer( ashes::RenderPass const & renderPass
			, TextureUnit const & depthBuffer
			, TextureUnit const & colourTexture )
		{
			ashes::ImageViewCRefArray attaches
			{
				depthBuffer.getTexture()->getDefaultView().getTargetView(),
				colourTexture.getTexture()->getDefaultView().getTargetView(),
			};
			return renderPass.createFrameBuffer( "Background"
				, { depthBuffer.getTexture()->getWidth(), depthBuffer.getTexture()->getHeight() }
			, std::move( attaches ) );
		}

		void doPrepareBgCommands( RenderDevice const & device
			, SceneBackground & background
			, ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & framebuffer
			, HdrConfigUbo const & hdrConfigUbo
			, ashes::CommandBuffer & commandBuffer )
		{
			background.initialise( device, renderPass, hdrConfigUbo );
			background.prepareFrame( commandBuffer
				, Size{ framebuffer.getDimensions().width, framebuffer.getDimensions().height }
				, renderPass
				, framebuffer );
		}

		CommandsSemaphore doPrepareColourTransitionCommands( RenderDevice const & device
			, TextureUnit const & colourTexture
			, TextureUnit const & velocityTexture )
		{
			auto & engine = *device.renderSystem.getEngine();
			CommandsSemaphore result{ device, "TechniqueColourTexTransition" };
			auto & cmd = *result.commandBuffer;
			cmd.begin();
			cmd.beginDebugBlock(
				{
					"Technique Colour Texture Transition",
					makeFloatArray( engine.getNextRainbowColour() ),
				} );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, colourTexture.getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_HOST_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, velocityTexture.getTexture()->getDefaultView().getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			cmd.clear( velocityTexture.getTexture()->getDefaultView().getTargetView()
				, transparentBlackClearColor.color );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, velocityTexture.getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
			cmd.endDebugBlock();
			cmd.end();

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
		, m_colourTexture{ doCreateTextureUnit( m_device
			, cuT( "RenderTechnique_Colour" )
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, m_size ) }
		, m_depthBuffer{ doCreateTextureUnit( m_device
			, cuT( "RenderTechnique_Depth" )
			, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
			, ( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
			, m_size ) }
		, m_matrixUbo{ m_device }
		, m_gpInfoUbo{ m_device }
		, m_lpvConfigUbo{ m_device }
		, m_llpvConfigUbo{ m_device }
		, m_vctConfigUbo{ m_device }
		, m_depthPass{ castor::makeUnique< DepthPass >( cuT( "Depth Prepass" )
			, m_device
			, m_matrixUbo
			, m_renderTarget.getCuller()
			, m_ssaoConfig
			, m_depthBuffer.getTexture() ) }
		, m_voxelizer{ castor::makeUnique< Voxelizer >( device
			, *m_renderTarget.getScene()
			, *m_renderTarget.getCamera()
			, m_matrixUbo
			, m_vctConfigUbo
			, m_renderTarget.getScene()->getVoxelConeTracingConfig() ) }
		, m_lpvResult{ castor::makeUnique< LightVolumePassResult >( *getEngine()
			, m_device
			, cuEmptyString
			, getEngine()->getLpvGridSize() ) }
		, m_llpvResult{ doCreateLLPVResult( device ) }
		, m_bgRenderPass{ createBgRenderPass( m_device
			, m_depthBuffer.getTexture()->getPixelFormat()
			, m_colourTexture.getTexture()->getPixelFormat() ) }
		, m_bgFrameBuffer{ doCreateBgFramebuffer( *m_bgRenderPass
			, m_depthBuffer
			, m_colourTexture ) }
		, m_bgCommandBuffer{ device.graphicsCommandPool->createCommandBuffer( "Background" ) }
		, m_cbgCommandBuffer{ device.graphicsCommandPool->createCommandBuffer( "ColourBackground" ) }
		, m_colorTexTransition{ doPrepareColourTransitionCommands( device
			, m_colourTexture
			, m_renderTarget.getVelocity() ) }
		, m_onBgChanged{ m_renderTarget.getScene()->getBackground()->onChanged.connect( [this]( SceneBackground const & )
			{
				getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [this]( RenderDevice const & device )
					{
						doPrepareBgCommands( m_device
							, *m_renderTarget.getScene()->getBackground()
							, *m_bgRenderPass
							, *m_bgFrameBuffer
							, m_renderTarget.getHdrConfigUbo()
							, *m_bgCommandBuffer );
					} ) );
			} ) }
		, m_onCBgChanged{ m_renderTarget.getScene()->getColourBackground().onChanged.connect( [this]( SceneBackground const & )
			{
				getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [this]( RenderDevice const & device )
					{
						doPrepareBgCommands( m_device
							, m_renderTarget.getScene()->getColourBackground()
							, *m_bgRenderPass
							, *m_bgFrameBuffer
							, m_renderTarget.getHdrConfigUbo()
							, *m_cbgCommandBuffer );
					} ) );
			} ) }
#if C3D_UseDeferredRendering
		, m_opaquePassResult{ castor::makeUnique< OpaquePassResult >( device
			, m_depthBuffer
			, m_renderTarget.getVelocity() ) }
		, m_opaquePass{ castor::makeUniqueDerived< RenderTechniquePass, OpaquePass >( m_device
			, m_size
			, m_matrixUbo
			, m_renderTarget.getCuller()
			, m_ssaoConfig
			, *m_opaquePassResult ) }
		, m_deferredRendering{ castor::makeUnique< DeferredRendering >( *getEngine()
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
		, m_opaquePass{ castor::makeUniqueDerived< RenderTechniquePass, ForwardRenderTechniquePass >( m_device
			, cuT( "Opaque" )
			, cuT( "Forward" )
			, SceneRenderPassDesc{ { m_size.getWidth(), m_size.getHeight(), 1u }, m_matrixUbo, m_renderTarget.getCuller() }
			, RenderTechniquePassDesc{ false, m_ssaoConfig }
				.lpvConfigUbo( m_lpvConfigUbo )
				.llpvConfigUbo( m_llpvConfigUbo )
				.vctConfigUbo( m_vctConfigUbo )
				.lpvResult( *m_lpvResult )
				.vctFirstBounce( m_voxelizer->getFirstBounce() )
				.vctSecondaryBounce( m_voxelizer->getSecondaryBounce() )
			, m_colourTexture.getTexture()->getDefaultView().getTargetView()
			, m_depthBuffer.getTexture()->getDefaultView().getTargetView()
			, false ) }
#endif
#if C3D_UseWeightedBlendedRendering
		, m_transparentPassResult{ castor::makeUnique< TransparentPassResult >( device
			, m_depthBuffer
			, m_renderTarget.getVelocity() ) }
		, m_transparentPass{ castor::makeUniqueDerived< RenderTechniquePass, TransparentPass >( m_device
			, m_size
			, m_matrixUbo
			, m_renderTarget.getCuller()
			, m_ssaoConfig
			, *m_transparentPassResult
			, m_lpvConfigUbo
			, m_llpvConfigUbo
			, m_vctConfigUbo
			, *m_lpvResult
			, m_voxelizer->getFirstBounce()
			, m_voxelizer->getSecondaryBounce() ) }
		, m_weightedBlendRendering{ castor::makeUnique< WeightedBlendRendering >( m_device
			, static_cast< TransparentPass & >( *m_transparentPass )
			, *m_transparentPassResult
			, m_colourTexture.getTexture()->getDefaultView().getTargetView()
			, m_renderTarget.getSize()
			, *m_renderTarget.getScene()
			, m_renderTarget.getHdrConfigUbo()
			, m_gpInfoUbo
			, *m_lpvResult )}
#else
		, m_transparentPass{ castor::makeUniqueDerived< RenderTechniquePass, ForwardRenderTechniquePass >( m_device
			, cuT( "Transparent" )
			, cuT( "Forward" )
			, SceneRenderPassDesc{ { m_size.getWidth(), m_size.getHeight(), 1u }, m_matrixUbo, m_renderTarget.getCuller(), false }
			, RenderTechniquePassDesc{ false, m_ssaoConfig }
				.lpvConfigUbo( m_lpvConfigUbo )
				.llpvConfigUbo( m_llpvConfigUbo )
				.vctConfigUbo( m_vctConfigUbo )
				.lpvResult( *m_lpvResult )
				.vctFirstBounce( m_voxelizer->getFirstBounce() )
				.vctSecondaryBounce( m_voxelizer->getSecondaryBounce() )
			, m_colourTexture.getTexture()->getDefaultView().getTargetView()
			, m_depthBuffer.getTexture()->getDefaultView().getTargetView()
			, false ) }
#endif
		, m_signalFinished{ m_device->createSemaphore( "RenderTechnique" ) }
		, m_directionalShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapDirectional >( m_device
			, *m_renderTarget.getScene() ) }
		, m_spotShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapSpot >( m_device
			, *m_renderTarget.getScene() ) }
		, m_pointShadowMap{ castor::makeUniqueDerived< ShadowMap, ShadowMapPoint >( m_device
			, *m_renderTarget.getScene() ) }
		, m_clearLpv{ doCreateClearLpvCommands( device, getName(), *m_lpvResult, m_llpvResult ) }
		, m_particleTimer{ std::make_shared< RenderPassTimer >( device, cuT( "Particles" ), cuT( "Particles" ) ) }
	{
		m_allShadowMaps[size_t( LightType::eDirectional )].emplace_back( std::ref( *m_directionalShadowMap ), UInt32Array{} );
		m_allShadowMaps[size_t( LightType::eSpot )].emplace_back( std::ref( *m_spotShadowMap ), UInt32Array{} );
		m_allShadowMaps[size_t( LightType::ePoint )].emplace_back( std::ref( *m_pointShadowMap ), UInt32Array{} );
		doInitialiseShadowMaps();
		doInitialiseLpv();

		doPrepareBgCommands( m_device
			, *m_renderTarget.getScene()->getBackground()
			, *m_bgRenderPass
			, *m_bgFrameBuffer
			, m_renderTarget.getHdrConfigUbo()
			, *m_bgCommandBuffer );
		doPrepareBgCommands( m_device
			, m_renderTarget.getScene()->getColourBackground()
			, *m_bgRenderPass
			, *m_bgFrameBuffer
			, m_renderTarget.getHdrConfigUbo()
			, *m_cbgCommandBuffer );

		auto & maps = m_renderTarget.getScene()->getEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().initialise();
		}
	}

	RenderTechnique::~RenderTechnique()
	{
		m_onBgChanged.disconnect();
		m_bgCommandBuffer.reset();
		m_cbgCommandBuffer.reset();
		m_bgFrameBuffer.reset();
		m_bgRenderPass.reset();
		m_particleTimer.reset();
#if C3D_UseWeightedBlendedRendering
		m_weightedBlendRendering.reset();
		m_transparentPassResult->cleanup();
		m_transparentPassResult.reset();
#endif
#if C3D_UseDeferredRendering
		m_deferredRendering.reset();
		m_opaquePassResult->cleanup();
		m_opaquePassResult.reset();
#endif
		m_clearLpv = {};
		m_llpvResult.clear();
		m_lpvResult.reset();
		m_voxelizer.reset();
		m_depthPass.reset();
		m_depthBuffer.cleanup();
		m_colourTexture.cleanup();
		getEngine()->getSamplerCache().remove( cuT( "RenderTechnique_Colour" ) );
		getEngine()->getSamplerCache().remove( cuT( "RenderTechnique_Depth" ) );
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

	void RenderTechnique::listIntermediates( std::vector< IntermediateView > & intermediates )
	{
		IntermediatesLister::submit( *this, intermediates );
	}

	void RenderTechnique::update( CpuUpdater & updater )
	{
		updater.camera = m_renderTarget.getCamera();
		updater.voxelConeTracing = m_renderTarget.getScene()->getVoxelConeTracingConfig().enabled;

		m_depthPass->update( updater );
		m_voxelizer->update( updater );

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
		updater.scene = m_renderTarget.getScene();
		updater.camera = m_renderTarget.getCamera();
		updater.voxelConeTracing = m_renderTarget.getScene()->getVoxelConeTracingConfig().enabled;

		doInitialiseShadowMaps();
		doInitialiseLpv();

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			map.get().update( updater );
		}

		m_depthPass->update( updater );

		if ( updater.voxelConeTracing )
		{
			m_voxelizer->update( updater );
		}

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
		auto * semaphore = &doRenderDepth( device, waitSemaphores );
		semaphore = &doRenderBackground( device, *semaphore );
		semaphore = &doRenderEnvironmentMaps( device, *semaphore );
		semaphore = &doRenderShadowMaps( device, *semaphore );
		semaphore = &doRenderLpv( device, *semaphore );

		// Render part
		if ( m_renderTarget.getScene()->getVoxelConeTracingConfig().enabled )
		{
			semaphore = &m_voxelizer->render( device, *semaphore );

		}

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

		m_voxelizer->listIntermediates( visitor );

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

	void RenderTechnique::doInitialiseShadowMaps()
	{
		m_directionalShadowMap->initialise();
		m_spotShadowMap->initialise();
		m_pointShadowMap->initialise();
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

			if ( needLpv && !m_lightPropagationVolumes[i] )
			{
				m_lightPropagationVolumes[i] = castor::makeUnique< LightPropagationVolumes >( scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult()
					, *m_lpvResult
					, m_lpvConfigUbo );
				m_lightPropagationVolumes[i]->initialise();
			}

			if ( needLpvG && !m_lightPropagationVolumesG[i] )
			{
				m_lightPropagationVolumesG[i] = castor::makeUnique< LightPropagationVolumesG >( scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult()
					, *m_lpvResult
					, m_lpvConfigUbo );
				m_lightPropagationVolumesG[i]->initialise();
			}

			if ( needLLpv && !m_layeredLightPropagationVolumes[i] )
			{
				m_layeredLightPropagationVolumes[i] = castor::makeUnique< LayeredLightPropagationVolumes >( scene
					, LightType( i )
					, m_device
					, m_allShadowMaps[i].front().first.get().getShadowPassResult()
					, m_llpvResult
					, m_llpvConfigUbo );
				m_layeredLightPropagationVolumes[i]->initialise();
			}

			if ( needLLpvG && !m_layeredLightPropagationVolumesG[i] )
			{
				m_layeredLightPropagationVolumesG[i] = castor::makeUnique< LayeredLightPropagationVolumesG >( scene
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
				if ( m_lightPropagationVolumes[i] )
				{
					result = &m_lightPropagationVolumes[i]->render( *result );
				}

				if ( m_lightPropagationVolumesG[i] )
				{
					result = &m_lightPropagationVolumesG[i]->render( *result );
				}

				if ( m_layeredLightPropagationVolumes[i] )
				{
					result = &m_layeredLightPropagationVolumes[i]->render( *result );
				}

				if ( m_layeredLightPropagationVolumesG[i] )
				{
					result = &m_layeredLightPropagationVolumesG[i]->render( *result );
				}
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
			result = &map.get().render( *result );
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderDepth( RenderDevice const & device
		, ashes::SemaphoreCRefArray const & semaphores )
	{
		return m_depthPass->render( semaphores );
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
			result = &m_deferredRendering->render( scene, camera, *result );
#else
			result = &static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).render( *result );
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
			result = &m_weightedBlendRendering->render( scene, *result );
#else
			result = &static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).render( *result );
#endif
		}

		return *result;
	}
}

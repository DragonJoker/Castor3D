#include "Castor3D/Render/Technique/Opaque/Lighting/LightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/DirectionalLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/LightPassReflectiveShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPassVolumePropagationShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPassLayeredVolumePropagationShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/PointLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/SpotLightPass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	LightingPass::LightingPass( Engine & engine
		, Size const & size
		, Scene & scene
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, ashes::ImageView const & depthView
		, SceneUbo & sceneUbo
		, GpInfoUbo const & gpInfoUbo )
		: m_engine{ engine }
		, m_size{ size }
		, m_result{ engine, size }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Opaque" ), cuT( "Lighting pass" ) ) }
		, m_srcDepth{ depthView }
		, m_blitDepth
		{
			getCurrentRenderDevice( engine ).graphicsCommandPool->createCommandBuffer( "LPBlitDepth", VK_COMMAND_BUFFER_LEVEL_PRIMARY ),
			getCurrentRenderDevice( engine )->createSemaphore( "LPBlitDepth" ),
		}
		, m_lpResultBarrier
		{
			getCurrentRenderDevice( engine ).graphicsCommandPool->createCommandBuffer( "LPResultBarrier", VK_COMMAND_BUFFER_LEVEL_PRIMARY ),
			getCurrentRenderDevice( engine )->createSemaphore( "LPResultBarrier" ),
		}
	{
		auto & lightCache = scene.getLightCache();
		lightCache.initialise();
		m_result.initialise();
		m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPass >( engine
			, m_result
			, gpInfoUbo
			, false );
		m_lightPass[size_t( LightType::ePoint )] = std::make_unique< PointLightPass >( engine
			, m_result
			, gpInfoUbo
			, false );
		m_lightPass[size_t( LightType::eSpot )] = std::make_unique< SpotLightPass >( engine
			, m_result
			, gpInfoUbo
			, false );
		m_lightPassShadow[size_t( GlobalIlluminationType::eNone )][size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassShadow >( engine
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( GlobalIlluminationType::eNone )][size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( engine
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( GlobalIlluminationType::eNone )][size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( engine
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( GlobalIlluminationType::eLpv )][size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassVolumePropagationShadow >( engine
			, lightCache
			, gpResult
			, smDirectionalResult
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( GlobalIlluminationType::eLpv )][size_t( LightType::ePoint )] = nullptr;
		m_lightPassShadow[size_t( GlobalIlluminationType::eLpv )][size_t( LightType::eSpot )] = std::make_unique< SpotLightPassVolumePropagationShadow >( engine
			, lightCache
			, gpResult
			, smSpotResult
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( GlobalIlluminationType::eLayeredLpv )][size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassLayeredVolumePropagationShadow >( engine
			, lightCache
			, gpResult
			, smDirectionalResult
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( GlobalIlluminationType::eLayeredLpv )][size_t( LightType::ePoint )] = nullptr;
		m_lightPassShadow[size_t( GlobalIlluminationType::eLayeredLpv )][size_t( LightType::eSpot )] = nullptr;

		for ( auto & lightPass : m_lightPass )
		{
			lightPass->initialise( scene
				, gpResult
				, sceneUbo
				, *m_timer );
		}

		for ( auto & lightPasses : m_lightPassShadow )
		{
			for ( auto & lightPass : lightPasses )
			{
				if ( lightPass )
				{
					lightPass->initialise( scene
						, gpResult
						, sceneUbo
						, *m_timer );
				}
			}
		}

		auto & device = getCurrentRenderDevice( engine );
		VkImageCopy copy
		{
			{ m_srcDepth->subresourceRange.aspectMask, 0u, 0u, 1u },
			VkOffset3D{ 0, 0, 0 },
			{ m_srcDepth->subresourceRange.aspectMask, 0u, 0u, 1u },
			VkOffset3D{ 0, 0, 0 },
			m_srcDepth.image->getDimensions(),
		};
		m_blitDepth.commandBuffer->begin();
		m_blitDepth.commandBuffer->beginDebugBlock(
			{
				"Deferred - Ligth Depth Blit",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		// Src depth buffer from depth attach to transfer source
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_srcDepth.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Dst depth buffer from unknown to transfer destination
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_result[LpTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Copy Src to Dst
		m_blitDepth.commandBuffer->copyImage( copy
			, *m_srcDepth.image
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, m_result[LpTexture::eDepth].getTexture()->getTexture()
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
		// Dst depth buffer from transfer destination to depth attach
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_result[LpTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeDepthStencilAttachment( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		// Src depth buffer from transfer source to depth stencil read only
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_srcDepth.makeDepthStencilReadOnly( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
		m_blitDepth.commandBuffer->endDebugBlock();
		m_blitDepth.commandBuffer->end();

		m_lpResultBarrier.commandBuffer->begin();
		m_lpResultBarrier.commandBuffer->beginDebugBlock(
			{
				"Deferred - Ligth Pass Result Barrier",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		// Diffuse view to shader read only
		m_lpResultBarrier.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_result[LpTexture::eDiffuse].getTexture()->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Specular view to shader read only
		m_lpResultBarrier.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_result[LpTexture::eSpecular].getTexture()->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		m_lpResultBarrier.commandBuffer->endDebugBlock();
		m_lpResultBarrier.commandBuffer->end();
	}

	LightingPass::~LightingPass()
	{
		m_lpResultBarrier.semaphore.reset();
		m_lpResultBarrier.commandBuffer.reset();
		m_blitDepth.semaphore.reset();
		m_blitDepth.commandBuffer.reset();

		for ( auto & lightPass : m_lightPass )
		{
			lightPass->cleanup();
			lightPass.reset();
		}

		for ( auto & lightPasses : m_lightPassShadow )
		{
			for ( auto & lightPass : lightPasses )
			{
				if ( lightPass )
				{
					lightPass->cleanup();
					lightPass.reset();
				}
			}
		}

		m_result.cleanup();
	}

	ashes::Semaphore const & LightingPass::render( Scene const & scene
		, Camera const & camera
		, OpaquePassResult const & gp
		, ashes::Semaphore const & toWait )
	{
		auto & cache = scene.getLightCache();
		ashes::Semaphore const * result = &toWait;

		if ( !cache.isEmpty() )
		{
			RenderPassTimerBlock timerBlock{ m_timer->start() };
			using LockType = std::unique_lock< LightCache const >;
			LockType lock{ castor::makeUniqueLock( cache ) };
			auto count = cache.getLightsCount( LightType::eDirectional )
				+ cache.getLightsCount( LightType::ePoint )
				+ cache.getLightsCount( LightType::eSpot );

			if ( timerBlock->getCount() != count )
			{
				timerBlock->updateCount( count );
			}

			auto & device = getCurrentRenderDevice( m_engine );
			device.graphicsQueue->submit( *m_blitDepth.commandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *m_blitDepth.semaphore
				, nullptr );
			result = m_blitDepth.semaphore.get();

			uint32_t index = 0;
			result = &doRenderLights( scene
				, camera
				, LightType::eDirectional
				, gp
				, *result
				, index );
			result = &doRenderLights( scene
				, camera
				, LightType::ePoint
				, gp
				, *result
				, index );
			result = &doRenderLights( scene
				, camera
				, LightType::eSpot
				, gp
				, *result
				, index );
		}
		else
		{
			auto & device = getCurrentRenderDevice( m_engine );
			device.graphicsQueue->submit( *m_lpResultBarrier.commandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *m_lpResultBarrier.semaphore
				, nullptr );
			result = m_lpResultBarrier.semaphore.get();
		}

		return *result;
	}

	void LightingPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( "Light Diffuse"
			, m_result[LpTexture::eDiffuse].getTexture()->getDefaultView().getSampledView() );
		visitor.visit( "Light Specular"
			, m_result[LpTexture::eSpecular].getTexture()->getDefaultView().getSampledView() );

		m_lightPass[size_t( LightType::eDirectional )]->accept( visitor );
		m_lightPass[size_t( LightType::ePoint )]->accept( visitor );
		m_lightPass[size_t( LightType::eSpot )]->accept( visitor );
		for ( auto & lightPasses : m_lightPassShadow )
		{
			for ( auto & lightPass : lightPasses )
			{
				if ( lightPass )
				{
					lightPass->accept( visitor );
				}
			}
		}
	}

	ashes::Semaphore const & LightingPass::doRenderLights( Scene const & scene
		, Camera const & camera
		, LightType type
		, OpaquePassResult const & gp
		, ashes::Semaphore const & toWait
		, uint32_t & index )
	{
		auto result = &toWait;
		auto & cache = scene.getLightCache();

		if ( cache.getLightsCount( type ) )
		{
			auto lightPass = m_lightPass[size_t( type )].get();

			for ( auto & light : cache.getLights( type ) )
			{
				auto lightPassShadow = doGetShadowLightPass( type
					, light->getGlobalIlluminationType() );

				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
				{
					LightPass * pass = nullptr;

					if ( light->isShadowProducer() && light->getShadowMap() )
					{
						pass = lightPassShadow;
					}
					else
					{
						pass = lightPass;
					}

					pass->update( index == 0u
						, camera.getSize()
						, *light
						, camera
						, light->getShadowMap()
						, light->getShadowMapIndex() );
					result = &pass->render( index
						, *result );
					++index;
				}
			}
		}

		return *result;
	}

	LightPass * LightingPass::doGetShadowLightPass( LightType lightType
		, GlobalIlluminationType giType )const
	{
		return ( ( lightType == LightType::ePoint && giType >= GlobalIlluminationType::eLpv )
			? m_lightPassShadow[size_t( GlobalIlluminationType::eNone )][size_t( lightType )].get()
			: ( ( lightType != LightType::eDirectional && giType == GlobalIlluminationType::eLayeredLpv )
				? m_lightPassShadow[size_t( GlobalIlluminationType::eLpv )][size_t( lightType )].get()
				: m_lightPassShadow[size_t( giType )][size_t( lightType )].get() ) );
	}
}

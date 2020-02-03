#include "Castor3D/Technique/Opaque/LightingPass.hpp"

#include "Castor3D/Technique/Opaque/DirectionalLightPass.hpp"
#include "Castor3D/Technique/Opaque/LightPassShadow.hpp"
#include "Castor3D/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Technique/Opaque/PointLightPass.hpp"
#include "Castor3D/Technique/Opaque/SpotLightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderWriter/Source.hpp>

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doCreateTexture( Engine & engine
			, Size const & size
			, VkFormat format
			, bool depth
			, castor::String debugName )
		{
			VkImageUsageFlags usage = depth
				? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				: VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size[0], size[1], 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				usage | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, std::move( debugName ) );
			texture->getDefaultImage().initialiseSource();

			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( engine.getLightsSampler() );
			result.initialise();
			return result;
		}

		TextureUnit doCreateColourTexture( Engine & engine
			, Size const & size )
		{
			auto & device = getCurrentRenderDevice( engine );
			return doCreateTexture( engine
				, size
				, device.selectSuitableFormat(
					{
						VK_FORMAT_R16G16B16A16_SFLOAT,
						VK_FORMAT_R32G32B32A32_SFLOAT,
					}
					, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
					| VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT )
				, false
				, cuT( "LightingPass_Colour" ) );
		}

		TextureUnit doCreateDepthTexture( Engine & engine
			, Size const & size )
		{
			auto & device = getCurrentRenderDevice( engine );
			return doCreateTexture( engine
				, size
				, device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
					| VK_FORMAT_FEATURE_TRANSFER_DST_BIT
					| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT
					| VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT )
				, true
				, cuT( "LightingPass_Depth" ) );
		}
	}

	LightingPass::LightingPass( Engine & engine
		, Size const & size
		, Scene const & scene
		, GeometryPassResult const & gpResult
		, OpaquePass & opaque
		, ashes::ImageView const & depthView
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo )
		: m_engine{ engine }
		, m_size{ size }
		, m_depth{ doCreateDepthTexture( engine, size ) }
		, m_diffuse{ doCreateColourTexture( engine, size ) }
		, m_specular{ doCreateColourTexture( engine, size ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Opaque" ), cuT( "Lighting pass" ) ) }
		, m_srcDepth{ depthView }
	{
		m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPass >( engine
			, m_depth.getTexture()->getDefaultView()
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo
			, false );
		m_lightPass[size_t( LightType::ePoint )] = std::make_unique< PointLightPass >( engine
			, m_depth.getTexture()->getDefaultView()
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo
			, false );
		m_lightPass[size_t( LightType::eSpot )] = std::make_unique< SpotLightPass >( engine
			, m_depth.getTexture()->getDefaultView()
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo
			, false );
		m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassShadow >( engine
			, m_depth.getTexture()->getDefaultView()
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo );
		m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( engine
			, m_depth.getTexture()->getDefaultView()
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo );
		m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( engine
			, m_depth.getTexture()->getDefaultView()
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo );

		for ( auto & lightPass : m_lightPass )
		{
			lightPass->initialise( scene
				, gpResult
				, sceneUbo
				, *m_timer );
		}

		for ( auto & lightPass : m_lightPassShadow )
		{
			lightPass->initialise( scene
				, gpResult
				, sceneUbo
				, *m_timer );
		}

		auto & device = getCurrentRenderDevice( engine );
		VkImageCopy copy
		{
			{ depthView->subresourceRange.aspectMask, 0u, 0u, 1u },
			VkOffset3D{ 0, 0, 0 },
			{ depthView->subresourceRange.aspectMask, 0u, 0u, 1u },
			VkOffset3D{ 0, 0, 0 },
			depthView.image->getDimensions(),
		};
		m_blitDepthSemaphore = device->createSemaphore();
		m_blitDepthCommandBuffer = device.graphicsCommandPool->createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		m_blitDepthCommandBuffer->begin();
		m_blitDepthCommandBuffer->beginDebugBlock(
			{
				"Deferred - Ligth Depth Blit",
				{ 0.7f, 1.0f, 0.7f, 1.0f },
			} );
		// Src depth buffer from depth attach to transfer source
		m_blitDepthCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_srcDepth.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Dst depth buffer from unknown to transfer destination
		m_blitDepthCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_depth.getTexture()->getDefaultView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Copy Src to Dst
		m_blitDepthCommandBuffer->copyImage( copy
			, *m_srcDepth.image
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, m_depth.getTexture()->getTexture()
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
		// Dst depth buffer from transfer destination to depth attach
		m_blitDepthCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_depth.getTexture()->getDefaultView().makeDepthStencilAttachment( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		// Src depth buffer from transfer source to depth stencil read only
		m_blitDepthCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_srcDepth.makeDepthStencilReadOnly( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
		m_blitDepthCommandBuffer->endDebugBlock();
		m_blitDepthCommandBuffer->end();
	}

	LightingPass::~LightingPass()
	{
		m_blitDepthSemaphore.reset();
		m_blitDepthCommandBuffer.reset();

		for ( auto & lightPass : m_lightPass )
		{
			lightPass->cleanup();
			lightPass.reset();
		}

		for ( auto & lightPass : m_lightPassShadow )
		{
			lightPass->cleanup();
			lightPass.reset();
		}

		m_diffuse.cleanup();
		m_specular.cleanup();
		m_depth.cleanup();
	}

	ashes::Semaphore const & LightingPass::render( Scene const & scene
		, Camera const & camera
		, GeometryPassResult const & gp
		, ashes::Semaphore const & toWait
		, RenderInfo & info )
	{
		auto & cache = scene.getLightCache();
		ashes::Semaphore const * result = &toWait;

		if ( !cache.isEmpty() )
		{
			auto timerBlock = m_timer->start();
			auto lock = makeUniqueLock( cache );
			auto count = cache.getLightsCount( LightType::eDirectional )
				+ cache.getLightsCount( LightType::ePoint )
				+ cache.getLightsCount( LightType::eSpot );

			if ( m_timer->getCount() != count )
			{
				m_timer->updateCount( count );
			}

			auto & device = getCurrentRenderDevice( m_engine );
			device.graphicsQueue->submit( *m_blitDepthCommandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *m_blitDepthSemaphore
				, nullptr );
			result = m_blitDepthSemaphore.get();

			uint32_t index = 0;
			result = &doRenderLights( scene
				, camera
				, LightType::eDirectional
				, gp
				, *result
				, index
				, info );
			result = &doRenderLights( scene
				, camera
				, LightType::ePoint
				, gp
				, *result
				, index
				, info );
			result = &doRenderLights( scene
				, camera
				, LightType::eSpot
				, gp
				, *result
				, index
				, info );
		}

		return *result;
	}

	void LightingPass::accept( RenderTechniqueVisitor & visitor )
	{
		m_lightPass[size_t( LightType::eDirectional )]->accept( visitor );
		m_lightPass[size_t( LightType::ePoint )]->accept( visitor );
		m_lightPass[size_t( LightType::eSpot )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::eDirectional )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::ePoint )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::eSpot )]->accept( visitor );
	}

	ashes::Semaphore const & LightingPass::doRenderLights( Scene const & scene
		, Camera const & camera
		, LightType type
		, GeometryPassResult const & gp
		, ashes::Semaphore const & toWait
		, uint32_t & index
		, RenderInfo & info )
	{
		auto result = &toWait;
		auto & cache = scene.getLightCache();

		if ( cache.getLightsCount( type ) )
		{
			auto lightPass = m_lightPass[size_t( type )].get();
			auto lightPassShadow = m_lightPassShadow[size_t( type )].get();

			for ( auto & light : cache.getLights( type ) )
			{
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

					pass->update( !index
						, camera.getSize()
						, *light
						, camera
						, light->getShadowMap()
						, light->getShadowMapIndex() );
					result = &pass->render( index
						, *result );

					++index;
					info.m_visibleLightsCount++;
				}

				info.m_totalLightsCount++;
			}
		}

		return *result;
	}
}

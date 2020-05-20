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
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassReflectiveShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/PointLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/SpotLightPass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
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
		m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassShadow >( engine
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( engine
			, m_result
			, gpInfoUbo );
		m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( engine
			, m_result
			, gpInfoUbo );
		m_lightPassRsm[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassReflectiveShadow >( engine
			, lightCache
			, gpResult
			, smDirectionalResult
			, m_result
			, gpInfoUbo );
		m_lightPassRsm[size_t( LightType::ePoint )] = std::make_unique< PointLightPassReflectiveShadow >( engine
			, lightCache
			, gpResult
			, smPointResult
			, m_result
			, gpInfoUbo );
		m_lightPassRsm[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassReflectiveShadow >( engine
			, lightCache
			, gpResult
			, smSpotResult
			, m_result
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

		for ( auto & lightPass : m_lightPassRsm )
		{
			lightPass->initialise( scene
				, gpResult
				, sceneUbo
				, *m_timer );
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
		m_blitDepthSemaphore = device->createSemaphore( "LightingPass" );
		m_blitDepthCommandBuffer = device.graphicsCommandPool->createCommandBuffer( "LightingPass"
			, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		m_blitDepthCommandBuffer->begin();
		m_blitDepthCommandBuffer->beginDebugBlock(
			{
				"Deferred - Ligth Depth Blit",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		// Src depth buffer from depth attach to transfer source
		m_blitDepthCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_srcDepth.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Dst depth buffer from unknown to transfer destination
		m_blitDepthCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_result[LpTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Copy Src to Dst
		m_blitDepthCommandBuffer->copyImage( copy
			, *m_srcDepth.image
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, m_result[LpTexture::eDepth].getTexture()->getTexture()
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
		// Dst depth buffer from transfer destination to depth attach
		m_blitDepthCommandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_result[LpTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeDepthStencilAttachment( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
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

		for ( auto & lightPass : m_lightPassRsm )
		{
			lightPass->cleanup();
			lightPass.reset();
		}

		m_result.cleanup();
	}

	void LightingPass::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, castor::Point2f const & jitter )
	{
		auto & cache = scene.getLightCache();

		if ( !cache.isEmpty() )
		{
			uint32_t index = 0;
			doUpdateLights( scene
				, camera
				, LightType::eDirectional
				, index
				, info );
			doUpdateLights( scene
				, camera
				, LightType::ePoint
				, index
				, info );
			doUpdateLights( scene
				, camera
				, LightType::eSpot
				, index
				, info );
		}
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
			auto timerBlock = m_timer->start();
			using LockType = std::unique_lock< LightCache const >;
			LockType lock{ castor::makeUniqueLock( cache ) };
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
		m_lightPassShadow[size_t( LightType::eDirectional )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::ePoint )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::eSpot )]->accept( visitor );
		m_lightPassRsm[size_t( LightType::eDirectional )]->accept( visitor );
		m_lightPassRsm[size_t( LightType::ePoint )]->accept( visitor );
		m_lightPassRsm[size_t( LightType::eSpot )]->accept( visitor );
	}

	void LightingPass::doUpdateLights( Scene const & scene
		, Camera const & camera
		, LightType type
		, uint32_t & index
		, RenderInfo & info )
	{
		auto & cache = scene.getLightCache();

		if ( cache.getLightsCount( type ) )
		{
			auto lightPass = m_lightPass[size_t( type )].get();
			auto lightPassShadow = m_lightPassShadow[size_t( type )].get();
			auto lightPassRsm = m_lightPassRsm[size_t( type )].get();

			for ( auto & light : cache.getLights( type ) )
			{
				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
				{
					LightPass * pass = nullptr;

					if ( light->isShadowProducer() && light->getShadowMap() )
					{
						if ( light->needsRsmShadowMaps() )
						{
							pass = lightPassRsm;
						}
						else
						{
							pass = lightPassShadow;
						}
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
					++index;
					info.m_visibleLightsCount++;
				}

				info.m_totalLightsCount++;
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
			auto lightPassShadow = m_lightPassShadow[size_t( type )].get();
			auto lightPassRsm = m_lightPassRsm[size_t( type )].get();

			for ( auto & light : cache.getLights( type ) )
			{
				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
				{
					LightPass * pass = nullptr;

					if ( light->isShadowProducer() && light->getShadowMap() )
					{
						if ( light->needsRsmShadowMaps() )
						{
							pass = lightPassRsm;
						}
						else
						{
							pass = lightPassShadow;
						}
					}
					else
					{
						pass = lightPass;
					}

					result = &pass->render( index
						, *result );
					++index;
				}
			}
		}

		return *result;
	}
}

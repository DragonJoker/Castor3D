#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( Scene & scene
			, ShadowMap & shadowMap
			, uint32_t cascadeCount )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassData > result;
			auto const width = ShadowMapPassDirectional::TextureSize;
			auto const height = ShadowMapPassDirectional::TextureSize;
			auto const w = float( width );
			auto const h = float( height );
			Viewport viewport{ engine };
			viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0, 5120.0 );
			viewport.resize( { width, height } );
			viewport.update();

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					std::make_shared< Camera >( cuT( "ShadowMapDirectional_" ) + string::toString( cascade + 1, std::locale{ "C" } )
						, scene
						, *scene.getCameraRootNode()
						, viewport
						, true ),
					nullptr,
					nullptr,
				};
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				passData.pass = std::make_shared< ShadowMapPassDirectional >( engine
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap
					, cascade );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapDirectional::ShadowMapDirectional( Scene & scene )
		: ShadowMap{ scene
			, LightType::eDirectional
			, ShadowMapResult{ *scene.getEngine()
				, cuT( "Directional" )
				, 0u
				, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize }
				, scene.getDirectionalShadowCascades() }
			, createPasses( scene, *this, scene.getDirectionalShadowCascades() )
			, 1u }
		, m_frameBuffers( m_passes.size() )
		, m_cascades{ scene.getDirectionalShadowCascades() }
	{
		log::trace << "Created ShadowMapDirectional" << std::endl;
	}

	ShadowMapDirectional::~ShadowMapDirectional()
	{
	}

	void ShadowMapDirectional::update( CpuUpdater & updater )
	{
		auto & light = *updater.light;
		auto & camera = *updater.camera;
		m_shadowType = light.getShadowType();
		auto node = light.getParent();
		node->update();

		auto & directional = *light.getDirectionalLight();

		if ( directional.updateShadow( camera ) )
		{
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				auto & culler = m_passes[cascade].pass->getCuller();
				auto & lightCamera = culler.getCamera();
				lightCamera.attachTo( *node );
				lightCamera.setProjection( directional.getProjMatrix( m_cascades - 1u ) );
				lightCamera.setView( directional.getViewMatrix( m_cascades - 1u ) );
				lightCamera.updateFrustum();

				updater.index = cascade;
				m_passes[cascade].pass->update( updater );
			}
		}
	}

	void ShadowMapDirectional::update( GpuUpdater & updater )
	{
		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			updater.index = cascade;
			m_passes[cascade].pass->update( updater );
		}
	}

	void ShadowMapDirectional::doInitialiseFramebuffers( RenderDevice const & device )
	{
		VkExtent2D const size
		{
			ShadowMapPassDirectional::TextureSize,
			ShadowMapPassDirectional::TextureSize,
		};

		if ( m_passes.size() == 1u )
		{
			auto & depth = m_result[SmTexture::eDepth].getTexture()->getDefault();
			auto & linear = m_result[SmTexture::eNormalLinear].getTexture()->getDefault();
			auto & variance = m_result[SmTexture::eVariance].getTexture()->getDefault();
			auto & position = m_result[SmTexture::ePosition].getTexture()->getDefault();
			auto & flux = m_result[SmTexture::eFlux].getTexture()->getDefault();

			m_blur = std::make_unique< GaussianBlur >( *getEngine()
				, device
				, "ShadowMapDirectional"
				, *variance.view
				, 5u );

			std::string debugName = "ShadowMapDirectional";
			auto cascade = 0u;
			auto & pass = m_passes[cascade];
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];
			frameBuffer.depthView = depth.view->getTargetView();
			frameBuffer.linearView = linear.view->getTargetView();
			frameBuffer.varianceView = variance.view->getTargetView();
			frameBuffer.positionView = position.view->getTargetView();
			frameBuffer.fluxView = flux.view->getTargetView();
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( frameBuffer.depthView );
			attaches.emplace_back( frameBuffer.linearView );
			attaches.emplace_back( frameBuffer.varianceView );
			attaches.emplace_back( frameBuffer.positionView );
			attaches.emplace_back( frameBuffer.fluxView );
			frameBuffer.frameBuffer = renderPass.createFrameBuffer( debugName, size, std::move( attaches ) );

			frameBuffer.blurCommands = m_blur->getCommands( true, cascade );
		}
		else
		{
			auto & depth = m_result[SmTexture::eDepth].getTexture()->getArray2D();
			auto & linear = m_result[SmTexture::eNormalLinear].getTexture()->getArray2D();
			auto & variance = m_result[SmTexture::eVariance].getTexture()->getArray2D();
			auto & position = m_result[SmTexture::ePosition].getTexture()->getArray2D();
			auto & flux = m_result[SmTexture::eFlux].getTexture()->getArray2D();

			m_blur = std::make_unique< GaussianBlur >( *getEngine()
				, device
				, "ShadowMapDirectional"
				, *variance.view->view
				, 5u );

			for ( uint32_t cascade = 0u; cascade < m_passes.size(); ++cascade )
			{
				std::string debugName = "ShadowMapDirectionalCascade" + std::to_string( cascade );
				auto & pass = m_passes[cascade];
				auto & renderPass = pass.pass->getRenderPass();
				auto & frameBuffer = m_frameBuffers[cascade];
				frameBuffer.depthView = depth.layers[cascade].view->getTargetView();
				frameBuffer.linearView = linear.layers[cascade].view->getTargetView();
				frameBuffer.varianceView = variance.layers[cascade].view->getTargetView();
				frameBuffer.positionView = position.layers[cascade].view->getTargetView();
				frameBuffer.fluxView = flux.layers[cascade].view->getTargetView();
				ashes::ImageViewCRefArray attaches;
				attaches.emplace_back( frameBuffer.depthView );
				attaches.emplace_back( frameBuffer.linearView );
				attaches.emplace_back( frameBuffer.varianceView );
				attaches.emplace_back( frameBuffer.positionView );
				attaches.emplace_back( frameBuffer.fluxView );
				frameBuffer.frameBuffer = renderPass.createFrameBuffer( debugName, size, std::move( attaches ) );

				frameBuffer.blurCommands = m_blur->getCommands( true, cascade );
			}
		}
	}

	void ShadowMapDirectional::doInitialise( RenderDevice const & device )
	{
		doInitialiseFramebuffers( device );
		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( m_name );
	}

	void ShadowMapDirectional::doCleanup( RenderDevice const & device )
	{
		m_commandBuffer.reset();
		m_frameBuffers.clear();
		m_blur.reset();
	}

	bool ShadowMapDirectional::isUpToDate( uint32_t index )const
	{
		return std::all_of( m_passes.begin()
			, m_passes.begin() + m_cascades
			, []( ShadowMap::PassData const & data )
			{
				return data.pass->isUpToDate();
			} );
	}

	ashes::Semaphore const & ShadowMapDirectional::doRender( RenderDevice const & device
		, ashes::Semaphore const & toWait
		, uint32_t index )
	{
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_commandBuffer->beginDebugBlock(
			{
				m_name + " generation " + std::to_string( index ),
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			auto & pass = m_passes[cascade];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[cascade];

			m_commandBuffer->beginDebugBlock(
				{
					m_name + " " + std::to_string( index ) + " cascade " + std::to_string( cascade ),
					makeFloatArray( getEngine()->getNextRainbowColour() ),
				} );
			timerBlock->notifyPassRender();
			timerBlock->beginPass( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( pass.pass->getRenderPass()
				, *frameBuffer.frameBuffer
				, getClearValues()
				, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

			if ( pass.pass->hasNodes() )
			{
				m_commandBuffer->executeCommands( { pass.pass->getCommandBuffer() } );
			}

			m_commandBuffer->endRenderPass();
			timerBlock->endPass( *m_commandBuffer );

			m_commandBuffer->endDebugBlock();
			pass.pass->setUpToDate();
		}

		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
		auto * result = &toWait;
		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		if ( m_shadowType == ShadowType::eVariance )
		{
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				auto & blurCommands = m_frameBuffers[cascade].blurCommands;
				device.graphicsQueue->submit( *blurCommands.commandBuffer
					, *result
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, *blurCommands.semaphore
					, nullptr );
				result = blurCommands.semaphore.get();
			}
		}
		return *result;
	}
}

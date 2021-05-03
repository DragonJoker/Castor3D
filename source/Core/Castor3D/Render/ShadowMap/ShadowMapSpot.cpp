#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( RenderDevice const & device
			, Scene & scene
			, ShadowMap & shadowMap )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassData > result;
			Viewport viewport{ engine };
			viewport.resize( Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } );

			for ( auto i = 0u; i < shader::getSpotShadowMapCount(); ++i )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					std::make_shared< Camera >( cuT( "ShadowMapSpot" )
						, scene
						, *scene.getCameraRootNode()
						, std::move( viewport ) ),
					nullptr,
					nullptr,
				};
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				passData.pass = std::make_shared< ShadowMapPassSpot >( device
					, i
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapSpot::ShadowMapSpot( RenderDevice const & device
		, Scene & scene )
		: ShadowMap{ device
			, scene
			, LightType::eSpot
			, ShadowMapResult{ *scene.getEngine()
				, cuT( "Spot" )
				, 0u
				, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize }
				, shader::getSpotShadowMapCount() }
			, createPasses( device, scene, *this )
			, shader::getSpotShadowMapCount() }
	{
		log::trace << "Created ShadowMapSpot" << std::endl;
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
		m_passesData.clear();
		m_blur.reset();
	}
	
	void ShadowMapSpot::update( CpuUpdater & updater )
	{
		m_passes[updater.index].pass->update( updater );
	}

	void ShadowMapSpot::update( GpuUpdater & updater )
	{
		m_passes[updater.index].pass->update( updater );
	}

	void ShadowMapSpot::doInitialise( RenderDevice const & device )
	{
		VkExtent2D size
		{
			ShadowMapPassSpot::TextureSize,
			ShadowMapPassSpot::TextureSize,
		};
		auto & depth = *m_result[SmTexture::eDepth].getTexture();
		auto & linear = *m_result[SmTexture::eNormalLinear].getTexture();
		auto & variance = *m_result[SmTexture::eVariance].getTexture();
		auto & position = *m_result[SmTexture::ePosition].getTexture();
		auto & flux = *m_result[SmTexture::eFlux].getTexture();
		m_blur = std::make_unique< GaussianBlur >( *getEngine()
			, device
			, "ShadowMapSpot"
			, variance.getDefaultView()
			, 5u );

		for ( auto i = 0u; i < m_passes.size(); ++i )
		{
			std::string debugName = "ShadowMapSpot" + std::to_string( i );
			auto & renderPass = m_passes[i].pass->getRenderPass();
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( depth.getLayer2DView( i ).getTargetView() );
			attaches.emplace_back( linear.getLayer2DView( i ).getTargetView() );
			attaches.emplace_back( variance.getLayer2DView( i ).getTargetView() );
			attaches.emplace_back( position.getLayer2DView( i ).getTargetView() );
			attaches.emplace_back( flux.getLayer2DView( i ).getTargetView() );
			m_passesData.push_back(
				{
					device.graphicsCommandPool->createCommandBuffer( debugName ),
					renderPass.createFrameBuffer( debugName, size, std::move( attaches ) ),
					device->createSemaphore( debugName ),
					{ nullptr, nullptr }
				} );
			m_passesData.back().blurCommands = m_blur->getCommands( true, i );
		}
	}

	ashes::Semaphore const & ShadowMapSpot::doRender( RenderDevice const & device
		, ashes::Semaphore const & toWait
		, uint32_t index )
	{
		auto & pass = m_passes[index];
		auto & commandBuffer = *m_passesData[index].commandBuffer;
		auto & frameBuffer = *m_passesData[index].frameBuffer;
		auto & finished = *m_passesData[index].finished;
		auto & blurCommands = m_passesData[index].blurCommands;

		auto & timer = pass.pass->getTimer();
		auto timerBlock = timer.start();

		commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		commandBuffer.beginDebugBlock(
			{
				m_name + " generation " + std::to_string( index ),
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		timerBlock->notifyPassRender();
		timerBlock->beginPass( commandBuffer );
		commandBuffer.beginRenderPass( pass.pass->getRenderPass()
			, frameBuffer
			, getClearValues()
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

		if ( pass.pass->hasNodes() )
		{
			commandBuffer.executeCommands( { pass.pass->getCommandBuffer() } );
		}

		commandBuffer.endRenderPass();
		timerBlock->endPass( commandBuffer );
		commandBuffer.endDebugBlock();
		commandBuffer.end();

		pass.pass->setUpToDate();

		auto * result = &toWait;
		device.graphicsQueue->submit( commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, finished
			, nullptr );
		result = &finished;

		if ( static_cast< ShadowMapPassSpot const & >( *pass.pass ).getShadowType() == ShadowType::eVariance )
		{
			device.graphicsQueue->submit( *blurCommands.commandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *blurCommands.semaphore
				, nullptr );
			result = blurCommands.semaphore.get();
		}

		return *result;
	}

	bool ShadowMapSpot::isUpToDate( uint32_t index )const
	{
		return m_passes[index].pass->isUpToDate();
	}
}

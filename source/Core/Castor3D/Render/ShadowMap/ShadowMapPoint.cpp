#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
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
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassPoint.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <algorithm>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		std::vector< ShadowMap::PassData > createPasses( Engine & engine
			, Scene & scene
			, ShadowMap & shadowMap )
		{
			std::vector< ShadowMap::PassData > result;

			for ( auto i = 0u; i < 6u * shader::getPointShadowMapCount(); ++i )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					nullptr,
					std::make_unique< DummyCuller >( scene ),
					nullptr,
				};
				passData.pass = std::make_shared< ShadowMapPassPoint >( engine
					, i
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, cuT( "ShadowMapPoint" )
			, ShadowMapResult
			{
				engine,
				cuT( "Point" ),
				VkImageCreateFlags( VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ),
				Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize },
				6u * shader::getPointShadowMapCount(),
			}
			, createPasses( engine, scene, *this )
			, shader::getPointShadowMapCount() }
	{
		log::trace << "Created ShadowMapPoint" << std::endl;
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}

	void ShadowMapPoint::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_passesData[index].shadowType = light.getShadowType();

		for ( size_t face = index * 6u; face < ( index * 6u ) + 6u; ++face )
		{
			auto & pass = m_passes[face];
			pass.pass->cpuUpdate( queues, light, index );
		}
	}

	ashes::ImageView const & ShadowMapPoint::getView( SmTexture texture
		, uint32_t index )const
	{
		return m_passesData[index].views[size_t( texture )];
	}

	void ShadowMapPoint::doInitialiseFramebuffers()
	{
		VkExtent2D size
		{
			ShadowMapPassPoint::TextureSize,
			ShadowMapPassPoint::TextureSize,
		};
		auto & depth = *m_result[SmTexture::eDepth].getTexture();
		auto & linear = *m_result[SmTexture::eNormalLinear].getTexture();
		auto & variance = *m_result[SmTexture::eVariance].getTexture();
		auto & position = *m_result[SmTexture::ePosition].getTexture();
		auto & flux = *m_result[SmTexture::eFlux].getTexture();

		for ( uint32_t layer = 0u; layer < shader::getPointShadowMapCount(); ++layer )
		{
			std::string debugName = "ShadowMapPoint" + std::to_string( layer );
			uint32_t passIndex = layer * 6u;
			PassData data{};
			data.views =
			{
				depth.getLayerCubeView( layer ).getSampledView(),
				linear.getLayerCubeView( layer ).getSampledView(),
				variance.getLayerCubeView( layer ).getSampledView(),
				position.getLayerCubeView( layer ).getSampledView(),
				flux.getLayerCubeView( layer ).getSampledView(),
			};

			for ( auto & frameBuffer : data.frameBuffers )
			{
				auto face = CubeMapFace( passIndex % 6u );
				auto fbDebugName = debugName + getName( face );
				auto & pass = m_passes[passIndex];
				auto & renderPass = pass.pass->getRenderPass();
				frameBuffer.views =
				{
					depth.getLayerCubeFaceView( layer, face ).getTargetView(),
					linear.getLayerCubeFaceView( layer, face ).getTargetView(),
					variance.getLayerCubeFaceView( layer, face ).getTargetView(),
					position.getLayerCubeFaceView( layer, face ).getTargetView(),
					flux.getLayerCubeFaceView( layer, face ).getTargetView(),
				};
				ashes::ImageViewCRefArray attaches;

				for ( auto & view : frameBuffer.views )
				{
					attaches.emplace_back( view );
				}

				frameBuffer.frameBuffer = renderPass.createFrameBuffer( fbDebugName
					, size
					, std::move( attaches ) );

				frameBuffer.varianceView = variance.getLayerCubeFaceView( layer, face ).getTargetView();
				frameBuffer.blur = std::make_unique< GaussianBlur >( *getEngine()
					, debugName
					, variance.getLayerCubeFaceView( layer, face )
					, 5u );
				frameBuffer.blurCommands = frameBuffer.blur->getCommands( true );
				++passIndex;
			}

			m_passesData.emplace_back( std::move( data ) );
		}
	}

	void ShadowMapPoint::doInitialise()
	{
		doInitialiseFramebuffers();
		uint32_t index = 0u;

		for ( auto & data : m_passesData )
		{
			std::string debugName = "ShadowMapPoint" + std::to_string( index++ );
			auto & device = getCurrentRenderDevice( *this );
			data.commandBuffer = device.graphicsCommandPool->createCommandBuffer( debugName );
			data.finished = device->createSemaphore( debugName );
		}
	}

	void ShadowMapPoint::doCleanup()
	{
		m_passesData.clear();
	}

	void ShadowMapPoint::updateDeviceDependent( uint32_t index )
	{
		uint32_t offset = index * 6u;

		for ( uint32_t face = offset; face < offset + 6u; ++face )
		{
			m_passes[face].pass->gpuUpdate( face - offset );
		}
	}

	ashes::Semaphore const & ShadowMapPoint::doRender( ashes::Semaphore const & toWait
		, uint32_t index )
	{
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();
		auto * result = &toWait;
		uint32_t offset = index * 6u;

		auto & passData = m_passesData[index];
		auto & commandBuffer = *passData.commandBuffer;
		auto & finished = *passData.finished;
		commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		commandBuffer.beginDebugBlock(
			{
				m_name + " generation " + std::to_string( index ),
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto & pass = m_passes[offset + face];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = passData.frameBuffers[face];

			commandBuffer.beginDebugBlock(
				{
					m_name + " " + std::to_string( index ) + " face " + std::to_string( face ),
					makeFloatArray( getEngine()->getNextRainbowColour() ),
				} );
			timerBlock->notifyPassRender();
			timerBlock->beginPass( commandBuffer );
			commandBuffer.beginRenderPass( renderPass
				, *frameBuffer.frameBuffer
				, getClearValues()
				, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			commandBuffer.executeCommands( { pass.pass->getCommandBuffer() } );
			commandBuffer.endRenderPass();
			timerBlock->endPass( commandBuffer );
			commandBuffer.endDebugBlock();
			pass.pass->setUpToDate();
		}

		commandBuffer.endDebugBlock();
		commandBuffer.end();
		auto & device = getCurrentRenderDevice( *this );

		device.graphicsQueue->submit( commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, finished
			, nullptr );
		result = &finished;

		if ( passData.shadowType == ShadowType::eVariance )
		{
			for ( uint32_t face = 0u; face < 6u; ++face )
			{
				auto & blurCommands = passData.frameBuffers[face].blurCommands;
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

	bool ShadowMapPoint::isUpToDate( uint32_t index )const
	{
		uint32_t offset = index * 6u;
		return std::all_of( m_passes.begin() + offset
			, m_passes.begin() + offset + 6u
			, []( ShadowMap::PassData const & data )
			{
				return data.pass->isUpToDate();
			} );
	}
}

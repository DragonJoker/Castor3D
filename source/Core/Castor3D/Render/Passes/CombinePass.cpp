#include "Castor3D/Render/Passes/CombinePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureLayoutSPtr doCreateTexture( RenderSystem & renderSystem
			, castor::String const & prefix
			, VkExtent2D const & size
			, VkFormat format )
		{
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size.width, size.height, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ),
			};
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, prefix + cuT( "CombineResult" ) );
			texture->initialise();
			return texture;
		}

		ashes::ImageView doCreateView( ashes::Image const & texture )
		{
			ashes::ImageViewCreateInfo imageView
			{
				0u,
				texture,
				VK_IMAGE_VIEW_TYPE_2D,
				texture.getFormat(),
				VkComponentMapping{},
				{ ashes::getAspectMask( texture.getFormat() ), 0u, 1u, 0u, 1u },
			};
			return texture.createView( imageView );
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderSystem const & renderSystem
			, castor::String const & prefix
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
					{},
					ashes::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto & device = getCurrentRenderDevice( renderSystem );
			auto result = device->createRenderPass( prefix + "Combine"
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( castor::String const & prefix
			, ashes::RenderPass const & renderPass
			, ashes::ImageView const & view
			, VkExtent2D const & size )
		{
			ashes::ImageViewCRefArray attachments{ view };
			return renderPass.createFrameBuffer( prefix + "Combine"
				, size
				, std::move( attachments ) );
		}
	}

	//*********************************************************************************************

	CombinePass::CombineQuad::CombineQuad( Engine & engine
		, castor::String const & prefix
		, ashes::ImageView const & lhsView
		, RenderQuad::TexcoordConfig const * config )
		: RenderQuad{ *engine.getRenderSystem(), VK_FILTER_LINEAR, nullptr, config }
		, m_lhsView{ lhsView }
		, m_lhsSampler{ createSampler( engine, prefix + cuT( "Combine" ), VK_FILTER_LINEAR, &lhsView->subresourceRange ) }
	{
		m_lhsSampler->initialise();
	}

	void CombinePass::CombineQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_lhsView
			, m_lhsSampler->getSampler() );
	}

	//*********************************************************************************************

	String const CombinePass::LhsMap = cuT( "c3d_mapLhs" );
	String const CombinePass::RhsMap = cuT( "c3d_mapRhs" );

	CombinePass::CombinePass( Engine & engine
		, castor::String const & prefix
		, VkFormat outputFormat
		, VkExtent2D const & outputSize
		, ShaderModule const & vertexShader
		, ShaderModule const & pixelShader
		, ashes::ImageView const & lhsView
		, ashes::ImageView const & rhsView
		, TextureLayoutSPtr resultTexture
		, RenderQuad::TexcoordConfig const * config )
		: m_engine{ engine }
		, m_prefix{ prefix }
		, m_image{ resultTexture }
		, m_view{ doCreateView( m_image->getTexture() ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, m_prefix, cuT( "Combine" ) ) }
		, m_finished{ getCurrentRenderDevice( m_engine )->createSemaphore( cuT( "CombinePass" ) ) }
		, m_vertexShader{ vertexShader }
		, m_pixelShader{ pixelShader }
		, m_renderPass{ doCreateRenderPass( *m_engine.getRenderSystem(), m_prefix, outputFormat ) }
		, m_frameBuffer{ doCreateFrameBuffer( m_prefix, *m_renderPass, m_view, outputSize ) }
		, m_quad{ engine, m_prefix, lhsView, config }
	{
		auto & device = getCurrentRenderDevice( engine );
		ashes::PipelineShaderStageCreateInfoArray program
		{
			makeShaderState( device, m_vertexShader ),
			makeShaderState( device, m_pixelShader ),
		};
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT )
		};
		m_quad.createPipeline( outputSize
			, Position{}
			, program
			, rhsView
			, * m_renderPass
			, bindings
			, {} );
		auto commands = getCommands( *m_timer, 0u );
		m_commandBuffer = std::move( commands.commandBuffer );
		m_finished = std::move( commands.semaphore );
	}

	CombinePass::CombinePass( Engine & engine
		, castor::String const & prefix
		, VkFormat outputFormat
		, VkExtent2D const & outputSize
		, ShaderModule const & vertexShader
		, ShaderModule const & pixelShader
		, ashes::ImageView const & lhsView
		, ashes::ImageView const & rhsView
		, TextureLayoutSPtr resultTexture )
		: CombinePass
		{
			engine,
			prefix,
			outputFormat,
			outputSize,
			vertexShader,
			pixelShader,
			lhsView,
			rhsView,
			resultTexture,
			nullptr,
		}
	{
	}

	CombinePass::CombinePass( Engine & engine
		, castor::String const & prefix
		, VkFormat outputFormat
		, VkExtent2D const & outputSize
		, ShaderModule const & vertexShader
		, ShaderModule const & pixelShader
		, ashes::ImageView const & lhsView
		, ashes::ImageView const & rhsView )
		: CombinePass
		{
			engine,
			prefix,
			outputFormat,
			outputSize,
			vertexShader,
			pixelShader,
			lhsView,
			rhsView,
			doCreateTexture( *engine.getRenderSystem(), prefix, outputSize, outputFormat ),
			nullptr,
		}
	{
	}

	CombinePass::CombinePass( Engine & engine
		, castor::String const & prefix
		, VkFormat outputFormat
		, VkExtent2D const & outputSize
		, ShaderModule const & vertexShader
		, ShaderModule const & pixelShader
		, ashes::ImageView const & lhsView
		, ashes::ImageView const & rhsView
		, TextureLayoutSPtr resultTexture
		, RenderQuad::TexcoordConfig const & config )
		: CombinePass
		{
			engine,
			prefix,
			outputFormat,
			outputSize,
			vertexShader,
			pixelShader,
			lhsView,
			rhsView,
			resultTexture,
			&config,
		}
	{
	}

	CombinePass::CombinePass( Engine & engine
		, castor::String const & prefix
		, VkFormat outputFormat
		, VkExtent2D const & outputSize
		, ShaderModule const & vertexShader
		, ShaderModule const & pixelShader
		, ashes::ImageView const & lhsView
		, ashes::ImageView const & rhsView
		, RenderQuad::TexcoordConfig const & config )
		: CombinePass
		{
			engine,
			prefix,
			outputFormat,
			outputSize,
			vertexShader,
			pixelShader,
			lhsView,
			rhsView,
			doCreateTexture( *engine.getRenderSystem(), prefix, outputSize, outputFormat ),
			&config,
		}
	{
	}

	void CombinePass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	ashes::Semaphore const & CombinePass::combine( ashes::Semaphore const & toWait )const
	{
		//auto timerBlock = m_timer->start();
		//m_timer->notifyPassRender();
		auto & device = getCurrentRenderDevice( m_engine );
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	CommandsSemaphore CombinePass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( m_engine );
		CommandsSemaphore result
		{
			device.graphicsCommandPool->createCommandBuffer( m_prefix + "CombinePass" ),
			device->createSemaphore( m_prefix + "CombinePass" ),
		};
		auto & cmd = *result.commandBuffer;

		cmd.begin();
		//timer.beginPass( cmd, index );
		cmd.beginDebugBlock(
			{
				m_prefix + " - Combine pass",
				makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ),
			} );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		m_quad.registerFrame( cmd );
		cmd.endRenderPass();
		cmd.endDebugBlock();
		//timer.endPass( cmd, index );
		cmd.end();

		return result;
	}
}

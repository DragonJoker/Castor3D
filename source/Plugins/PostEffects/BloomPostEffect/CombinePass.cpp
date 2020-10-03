#include "BloomPostEffect/CombinePass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace Bloom
{
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexProgram( castor3d::RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram( castor3d::RenderSystem & renderSystem
			, uint32_t blurPassesCount )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapPasses = writer.declSampledImage< FImg2DRgba32 >( CombinePass::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declSampledImage< FImg2DRgba32 >( CombinePass::CombineMapScene, 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = texture( c3d_mapScene, vtx_texture );

					for ( uint32_t i = 0; i < blurPassesCount; ++i )
					{
						pxl_fragColor += texture( c3d_mapPasses, vtx_texture, Float( float( i ) ) );
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::TextureLayoutSPtr doCreateTexture( castor3d::RenderDevice const & device
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
			auto texture = std::make_shared< castor3d::TextureLayout >( device.renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "BloomCombineResult" ) );
			texture->initialise( device );
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

		ashes::RenderPassPtr doCreateRenderPass( castor3d::RenderDevice const & device
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
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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
					VK_PIPELINE_STAGE_HOST_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_ACCESS_HOST_WRITE_BIT,
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
			auto result = device->createRenderPass( "BloomCombine"
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, ashes::ImageView const & view
			, VkExtent2D const & size )
		{
			ashes::ImageViewCRefArray attachments{ view };
			return renderPass.createFrameBuffer( "BloomCombine"
				, size
				, std::move( attachments ) );
		}

		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout( castor3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings
			{
				castor3d::makeDescriptorSetLayoutBinding( 0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT ),
				castor3d::makeDescriptorSetLayoutBinding( 1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT ),
			};
			return device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
		}

		ashes::GraphicsPipelinePtr doCreatePipeline( castor3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, castor3d::ShaderModule const & vertexShader
			, castor3d::ShaderModule const & pixelShader
			, ashes::RenderPass const & renderPass
			, VkExtent2D const & size )
		{
			ashes::PipelineVertexInputStateCreateInfo inputState
			{
				0u,
				ashes::VkVertexInputBindingDescriptionArray
				{
					{ 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
				},
				ashes::VkVertexInputAttributeDescriptionArray
				{
					{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( castor3d::NonTexturedQuad::Vertex, position ) },
				}
			};

			ashes::PipelineShaderStageCreateInfoArray stages;
			stages.push_back( castor3d::makeShaderState( device, vertexShader ) );
			stages.push_back( castor3d::makeShaderState( device, pixelShader ) );

			return device->createPipeline( ashes::GraphicsPipelineCreateInfo
				{
					0u,
					stages,
					inputState,
					ashes::PipelineInputAssemblyStateCreateInfo{},
					ashes::nullopt,
					ashes::PipelineViewportStateCreateInfo
					{
						0u,
						{ 1u, ashes::makeViewport( size ) },
						{ 1u, ashes::makeScissor( size ) },
					},
					ashes::PipelineRasterizationStateCreateInfo{},
					ashes::PipelineMultisampleStateCreateInfo{},
					ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
					ashes::PipelineColorBlendStateCreateInfo{},
					ashes::nullopt,
					pipelineLayout,
					renderPass,
				} );
		}

		ashes::DescriptorSetPtr doCreateDescriptorSet( ashes::DescriptorSetPool const & pool
			, ashes::ImageView const & sceneView
			, ashes::ImageView const & blurView
			, ashes::Sampler const & sceneSampler
			, ashes::Sampler const & blurSampler )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			result->createBinding( layout.getBinding( 0u )
				, blurView
				, blurSampler
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result->createBinding( layout.getBinding( 1u )
				, sceneView
				, sceneSampler
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result->update();
			return result;
		}
	}

	//*********************************************************************************************

	String const CombinePass::CombineMapPasses = cuT( "c3d_mapPasses" );
	String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );

	CombinePass::CombinePass( castor3d::RenderDevice const & device
		, VkFormat format
		, ashes::ImageView const & sceneView
		, ashes::ImageView const & blurView
		, VkExtent2D const & size
		, uint32_t blurPassesCount )
		: m_device{ device }
		, m_image{ doCreateTexture( device, size, format ) }
		, m_view{ doCreateView( m_image->getTexture() ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "BloomCombine", getVertexProgram( device.renderSystem ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "BloomCombine", getPixelProgram( device.renderSystem, blurPassesCount ) }
		, m_renderPass{ doCreateRenderPass( m_device, format ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_view, size ) }
		, m_descriptorLayout{ doCreateDescriptorLayout( m_device ) }
		, m_pipelineLayout{ m_device->createPipelineLayout( "BloomCombine" , *m_descriptorLayout ) }
		, m_pipeline{ doCreatePipeline( m_device, *m_pipelineLayout, m_vertexShader, m_pixelShader, *m_renderPass, size ) }
		, m_sceneSampler{ m_device->createSampler( "BloomCombineScene"
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR ) }
		, m_blurSampler{ m_device->createSampler( "BloomCombineBlur"
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, 0.0f
			, float( blurPassesCount ) ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( "BloomCombine", 1u ) }
		, m_descriptorSet{ doCreateDescriptorSet( *m_descriptorPool, sceneView, blurView, *m_sceneSampler, *m_blurSampler ) }
		, m_blurPassesCount{ blurPassesCount }
	{
	}

	castor3d::CommandsSemaphore CombinePass::getCommands( castor3d::RenderPassTimer const & timer
		, ashes::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const
	{
		auto result = m_device.graphicsCommandPool->createCommandBuffer( "BloomCombine" );
		auto & cmd = *result;

		cmd.begin();
		timer.beginPass( cmd, 1u + ( m_blurPassesCount * 2u ) );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		cmd.bindPipeline( *m_pipeline );
		cmd.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		cmd.bindVertexBuffer( 0u, vertexBuffer.getBuffer(), 0u );
		cmd.draw( 6u );
		cmd.endRenderPass();
		timer.endPass( cmd, 1u + ( m_blurPassesCount * 2u ) );
		cmd.end();

		return { std::move( result ), m_device->createSemaphore( "BloomCombine" ) };
	}

	void CombinePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

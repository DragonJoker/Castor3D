#include "BloomPostEffect/BlurPass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Shader/Program.hpp>

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

		std::unique_ptr< ast::Shader > getPixelProgram( castor3d::RenderSystem & renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, castor3d::GaussianBlur::Config, 0u, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( castor3d::GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( castor3d::GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( castor3d::GaussianBlur::Coefficients, castor3d::GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapSource", 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto offset = writer.declLocale( "offset"
						, vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = texture( c3d_mapSource, vtx_texture ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1u, i < c3d_coefficientsCount, ++i )
					{
						offset += c3d_pixelSize;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vtx_texture + offset );
					}
					ROF;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::vector< float > getHalfPascal( uint32_t height )
		{
			std::vector< float > result;
			result.resize( height );
			auto x = 1.0f;
			auto max = 1 + height;

			for ( uint32_t i = 0u; i <= max; ++i )
			{
				auto index = max - i;

				if ( index < height )
				{
					result[index] = x;
				}

				x = x * ( ( height + 1 ) * 2 - i ) / ( i + 1 );
			}

			// Normalize kernel coefficients
			float sum = result[0];

			for ( uint32_t i = 1u; i < height; ++i )
			{
				sum += result[i] * 2;
			}

			std::transform( std::begin( result )
				, std::end( result )
				, std::begin( result )
				, [&sum]( float & value )
				{
					return value /= sum;
				} );

			return result;
		}

		std::array< castor::Point4f, 15u > doCreateKernel( uint32_t count )
		{
			std::array< castor::Point4f, 15u > result;
			auto kernel = getHalfPascal( count );
			std::memcpy( result.data()->ptr()
				, kernel.data()
				, sizeof( float ) * std::min( size_t( 60u ), kernel.size() ) );
			return result;
		}

		castor3d::UniformBufferUPtr< castor3d::GaussianBlur::Configuration > doCreateUbo( castor3d::RenderDevice const & device
			, VkExtent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical )
		{
			auto result = castor3d::makeUniformBuffer< castor3d::GaussianBlur::Configuration >( device.renderSystem
				, blurPassesCount
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "GaussianBlurCfg" );
			auto coefficientsCount = blurKernelSize;
			auto kernel = doCreateKernel( coefficientsCount );

			for ( auto i = 0u; i < blurPassesCount; ++i )
			{
				auto & data = result->getData( i );
				data.textureSize = castor::Point2f
				{
					isVertical ? 0.0f : 1.0f / float( dimensions.width >> ( i + 1 ) ),
					isVertical ? 1.0f / float( dimensions.height >> ( i + 1 ) ) : 0.0f
				};
				data.blurCoeffsCount = coefficientsCount;
				data.blurCoeffs = kernel;
			}

			result->upload( 0u, blurPassesCount );
			return result;
		}

		std::vector< ashes::SamplerPtr > doCreateSamplers( castor3d::RenderDevice const & device
			, uint32_t blurPassesCount )
		{
			std::vector< ashes::SamplerPtr > result;

			for ( auto i = 0u; i < blurPassesCount; ++i )
			{
				result.push_back( device->createSampler( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
					, VK_FILTER_NEAREST
					, VK_FILTER_NEAREST
					, VK_SAMPLER_MIPMAP_MODE_NEAREST
					, float( i )
					, float( i + 1u ) ) );
			}

			return result;
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
			auto result = device->createRenderPass( std::move( createInfo ) );
			setDebugObjectName( device, *result, "BloomBlur" );
			return result;
		}

		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout( castor3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings
			{
				castor3d::makeDescriptorSetLayoutBinding( 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT ),
				castor3d::makeDescriptorSetLayoutBinding( 1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT ),
			};
			return device->createDescriptorSetLayout( std::move( bindings ) );
		}
	}

	//*********************************************************************************************

	BlurPass::Subpass::Subpass( castor3d::RenderDevice const & device
		, VkFormat format
		, ashes::ImageView const & srcView
		, ashes::ImageView const & dstView
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSetPool const & descriptorPool
		, ashes::PipelineLayout const & pipelineLayout
		, VkExtent2D dimensions
		, castor3d::ShaderModule const & vertexShader
		, castor3d::ShaderModule const & pixelShader
		, castor3d::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
		, uint32_t index )
	{
		dimensions.width >>= ( index + 1 );
		dimensions.height >>= ( index + 1 );

		sampler = device.device->createSampler( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, float( index )
			, float( index + 1u ) );

		auto & descriptorLayout = descriptorPool.getLayout();
		descriptorSet = descriptorPool.createDescriptorSet();
		descriptorSet->createSizedBinding( descriptorLayout.getBinding( 0u )
			, blurUbo
			, index );
		descriptorSet->createBinding( descriptorLayout.getBinding( 1u )
			, srcView
			, *sampler );
		descriptorSet->update();

		ashes::ImageViewCRefArray attaches{ dstView };
		frameBuffer = renderPass.createFrameBuffer( dimensions, std::move( attaches ) );
		
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
			},
		};

		ashes::PipelineShaderStageCreateInfoArray shaderStages;
		shaderStages.push_back( castor3d::makeShaderState( device, vertexShader ) );
		shaderStages.push_back( castor3d::makeShaderState( device, pixelShader ) );

		pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				shaderStages,
				inputState,
				ashes::PipelineInputAssemblyStateCreateInfo{},
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo
				{
					0u,
					{ 1u, ashes::makeViewport( {}, dimensions ) },
					{ 1u, ashes::makeScissor( {}, dimensions ) }
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

	std::vector< BlurPass::Subpass > doCreateSubpasses( castor3d::RenderDevice const & device
		, VkFormat format
		, castor3d::TextureLayout const & srcImage
		, castor3d::TextureLayout const & dstImage
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSetPool const & descriptorPool
		, ashes::PipelineLayout const & pipelineLayout
		, VkExtent2D dimensions
		, castor3d::ShaderModule const & vertexShader
		, castor3d::ShaderModule const & pixelShader
		, castor3d::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
		, uint32_t blurPassesCount )
	{
		std::vector< BlurPass::Subpass > result;

		for ( auto i = 0u; i < blurPassesCount; ++i )
		{
			result.emplace_back( device
				, format
				, srcImage.getImage( i ).getView()
				, dstImage.getImage( i ).getView()
				, renderPass
				, descriptorPool
				, pipelineLayout
				, dimensions
				, vertexShader
				, pixelShader
				, blurUbo
				, i );
		}

		return result;
	}

	//*********************************************************************************************

	BlurPass::BlurPass( castor3d::RenderDevice const & device
		, VkFormat format
		, castor3d::TextureLayout const & srcImage
		, castor3d::TextureLayout const & dstImage
		, VkExtent2D dimensions
		, uint32_t blurKernelSize
		, uint32_t blurPassesCount
		, bool isVertical )
		: m_device{ device }
		, m_blurKernelSize{ blurKernelSize }
		, m_blurPassesCount{ blurPassesCount }
		, m_blurUbo{ doCreateUbo( m_device, dimensions, m_blurKernelSize, m_blurPassesCount, isVertical ) }
		, m_renderPass{ doCreateRenderPass( m_device, format ) }
		, m_descriptorLayout{ doCreateDescriptorLayout( m_device ) }
		, m_pipelineLayout{ m_device->createPipelineLayout( *m_descriptorLayout ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "BloomBlurPass", getVertexProgram( m_device.renderSystem ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "BloomBlurPass", getPixelProgram( m_device.renderSystem ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( m_blurPassesCount ) }
		, m_passes{ doCreateSubpasses( m_device
			, format
			, srcImage
			, dstImage
			, *m_renderPass
			, *m_descriptorPool
			, *m_pipelineLayout
			, dimensions
			, m_vertexShader
			, m_pixelShader
			, *m_blurUbo
			, m_blurPassesCount ) }
		, m_isVertical{ isVertical }
	{
	}

	castor3d::CommandsSemaphoreArray BlurPass::getCommands( castor3d::RenderPassTimer const & timer
		, ashes::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const
	{
		castor3d::CommandsSemaphoreArray result;

		for ( auto i = 0u; i < m_blurPassesCount; ++i )
		{
			auto & blur = m_passes[i];

			ashes::CommandBufferPtr commandBuffer = m_device.graphicsCommandPool->createCommandBuffer();
			auto & cmd = *commandBuffer;

			cmd.begin();
			timer.beginPass( cmd, 1u + ( m_isVertical ? 1u : 0u ) * m_blurPassesCount + i );
			cmd.beginRenderPass( *m_renderPass
				, *blur.frameBuffer
				, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
			cmd.bindPipeline( *blur.pipeline );
			cmd.bindDescriptorSet( *blur.descriptorSet, *m_pipelineLayout );
			cmd.bindVertexBuffer( 0u, vertexBuffer.getBuffer(), 0u );
			cmd.draw( 6u );
			cmd.endRenderPass();
			timer.endPass( cmd, 1u + ( m_isVertical ? 1u : 0u ) * m_blurPassesCount + i );
			cmd.end();

			result.emplace_back( std::move( commandBuffer ), m_device->createSemaphore() );
		}

		return result;
	}

	void BlurPass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

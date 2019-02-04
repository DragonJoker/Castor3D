#include "BlurPass.hpp"

#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Miscellaneous/GaussianBlur.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/SubpassDependency.hpp>
#include <RenderPass/SubpassDescription.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace Bloom
{
	namespace
	{
		std::unique_ptr< sdw::Shader > getVertexProgram( castor3d::RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main", [&]()
				{
					vtx_texture = writer.paren( position + 1.0_f ) / 2.0_f;
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getPixelProgram( castor3d::RenderSystem & renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, castor3d::GaussianBlur::Config, 0u, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( castor3d::GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( castor3d::GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( castor3d::GaussianBlur::Coefficients, castor3d::GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDiffuse", 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main", [&]()
				{
					auto offset = writer.declLocale( "offset"
						, vec2( 0.0_f, 0 ) );
					pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0][0];

					FOR( writer, UInt, i, 1u, i < c3d_coefficientsCount, ++i )
					{
						offset += c3d_pixelSize;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vtx_texture + offset );
					}
					ROF;
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
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

		ashes::UniformBufferPtr< castor3d::GaussianBlur::Configuration > doCreateUbo( ashes::Device const & device
			, ashes::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical )
		{
			auto result = ashes::makeUniformBuffer< castor3d::GaussianBlur::Configuration >( device
				, blurPassesCount
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
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

		std::vector< ashes::SamplerPtr > doCreateSamplers( ashes::Device const & device
			, uint32_t blurPassesCount )
		{
			std::vector< ashes::SamplerPtr > result;

			for ( auto i = 0u; i < blurPassesCount; ++i )
			{
				result.push_back( device.createSampler( ashes::WrapMode::eClampToBorder
					, ashes::WrapMode::eClampToBorder
					, ashes::WrapMode::eClampToBorder
					, ashes::Filter::eNearest
					, ashes::Filter::eNearest
					, ashes::MipmapMode::eNearest
					, float( i )
					, float( i + 1u ) ) );
			}

			return result;
		}

		ashes::RenderPassPtr doCreateRenderPass( ashes::Device const & device
			, ashes::Format format )
		{
			ashes::RenderPassCreateInfo renderPass{};
			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].colorAttachments = { { 0u, ashes::ImageLayout::eColourAttachmentOptimal } };

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eHostWrite;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eHost;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;

			return device.createRenderPass( renderPass );
		}

		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout( ashes::Device const & device )
		{
			std::vector< ashes::DescriptorSetLayoutBinding > bindings
			{
				ashes::DescriptorSetLayoutBinding{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
				ashes::DescriptorSetLayoutBinding{ 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}
	}

	//*********************************************************************************************

	BlurPass::Subpass::Subpass( castor3d::RenderSystem & renderSystem
		, ashes::Device const & device
		, ashes::Format format
		, ashes::TextureView const & srcView
		, ashes::TextureView const & dstView
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSetPool const & descriptorPool
		, ashes::PipelineLayout const & pipelineLayout
		, ashes::Extent2D dimensions
		, castor3d::ShaderModule const & vertexShader
		, castor3d::ShaderModule const & pixelShader
		, ashes::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
		, uint32_t index )
	{
		dimensions.width >>= ( index + 1 );
		dimensions.height >>= ( index + 1 );

		sampler = device.createSampler( ashes::WrapMode::eClampToBorder
			, ashes::WrapMode::eClampToBorder
			, ashes::WrapMode::eClampToBorder
			, ashes::Filter::eNearest
			, ashes::Filter::eNearest
			, ashes::MipmapMode::eNearest
			, float( index )
			, float( index + 1u ) );

		auto & descriptorLayout = descriptorPool.getLayout();
		descriptorSet = descriptorPool.createDescriptorSet();
		descriptorSet->createBinding( descriptorLayout.getBinding( 0u )
			, blurUbo
			, index );
		descriptorSet->createBinding( descriptorLayout.getBinding( 1u )
			, srcView
			, *sampler );
		descriptorSet->update();

		ashes::FrameBufferAttachmentArray attaches
		{
			{ *renderPass.getAttachments().begin(), dstView }
		};
		frameBuffer = renderPass.createFrameBuffer( dimensions, std::move( attaches ) );

		ashes::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), ashes::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, ashes::Format::eR32G32_SFLOAT, 0u } );

		ashes::ShaderStageStateArray shaderStages;
		shaderStages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		shaderStages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		shaderStages[0].module->loadShader( renderSystem.compileShader( vertexShader ) );
		shaderStages[1].module->loadShader( renderSystem.compileShader( pixelShader ) );

		ashes::GraphicsPipelineCreateInfo pipelineInfo
		{
			shaderStages,
			renderPass,
			inputState,
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{},
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{},
			ashes::DepthStencilState{ 0u, false, false },
			ashes::nullopt,
			ashes::Viewport{ { 0, 0 }, dimensions },
			ashes::Scissor{ { 0, 0 }, dimensions }
		};
		pipeline = pipelineLayout.createPipeline( pipelineInfo );
	}

	std::vector< BlurPass::Subpass > doCreateSubpasses( castor3d::RenderSystem & renderSystem
		, ashes::Device const & device
		, ashes::Format format
		, castor3d::TextureLayout const & srcImage
		, castor3d::TextureLayout const & dstImage
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSetPool const & descriptorPool
		, ashes::PipelineLayout const & pipelineLayout
		, ashes::Extent2D dimensions
		, castor3d::ShaderModule const & vertexShader
		, castor3d::ShaderModule const & pixelShader
		, ashes::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
		, uint32_t blurPassesCount )
	{
		std::vector< BlurPass::Subpass > result;

		for ( auto i = 0u; i < blurPassesCount; ++i )
		{
			result.emplace_back( renderSystem
				, device
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

	BlurPass::BlurPass( castor3d::RenderSystem & renderSystem
		, ashes::Format format
		, castor3d::TextureLayout const & srcImage
		, castor3d::TextureLayout const & dstImage
		, ashes::Extent2D dimensions
		, uint32_t blurKernelSize
		, uint32_t blurPassesCount
		, bool isVertical )
		: m_device{ getCurrentDevice( renderSystem ) }
		, m_blurKernelSize{ blurKernelSize }
		, m_blurPassesCount{ blurPassesCount }
		, m_blurUbo{ doCreateUbo( m_device, dimensions, m_blurKernelSize, m_blurPassesCount, isVertical ) }
		, m_renderPass{ doCreateRenderPass( m_device, format ) }
		, m_descriptorLayout{ doCreateDescriptorLayout( m_device ) }
		, m_pipelineLayout{ m_device.createPipelineLayout( *m_descriptorLayout ) }
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "BloomBlurPass", getVertexProgram( renderSystem ) }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "BloomBlurPass", getPixelProgram( renderSystem ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( m_blurPassesCount ) }
		, m_passes{ doCreateSubpasses( renderSystem
			, m_device
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

			ashes::CommandBufferPtr commandBuffer = m_device.getGraphicsCommandPool().createCommandBuffer( true );
			auto & cmd = *commandBuffer;

			cmd.begin();
			timer.beginPass( cmd, 1u + ( m_isVertical ? 1u : 0u ) * m_blurPassesCount + i );
			cmd.beginRenderPass( *m_renderPass
				, *blur.frameBuffer
				, { ashes::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 } }
			, ashes::SubpassContents::eInline );
			cmd.bindPipeline( *blur.pipeline );
			cmd.bindDescriptorSet( *blur.descriptorSet, *m_pipelineLayout );
			cmd.bindVertexBuffer( 0u, vertexBuffer.getBuffer(), 0u );
			cmd.draw( 6u );
			cmd.endRenderPass();
			timer.endPass( cmd, 1u + ( m_isVertical ? 1u : 0u ) * m_blurPassesCount + i );
			cmd.end();

			result.emplace_back( std::move( commandBuffer ), m_device.createSemaphore() );
		}

		return result;
	}
}

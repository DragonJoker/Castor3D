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
#include <Shader/ShaderProgram.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace Bloom
{
	namespace
	{
		glsl::Shader getVertexProgram( castor3d::RenderSystem & renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = writer.paren( position + 1.0 ) / 2.0;
				out.gl_Position() = writer.rendererScalePosition( vec4( position, 0.0, 1.0 ) );
			} );
			return writer.finalise();
		}

		glsl::Shader getPixelProgram( castor3d::RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Ubo config{ writer, castor3d::GaussianBlur::Config, 0u, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( castor3d::GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( castor3d::GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( castor3d::GaussianBlur::Coefficients, castor3d::GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
					pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0][0];

					FOR( writer, UInt, i, 1u, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
					{
						offset += c3d_pixelSize;
						pxl_fragColor += c3d_coefficients[i / 4_ui][i % 4_ui] * texture( c3d_mapDiffuse, vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_ui][i % 4_ui] * texture( c3d_mapDiffuse, vtx_texture + offset );
					}
					ROF;
				} );
			return writer.finalise();
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

		renderer::UniformBufferPtr< castor3d::GaussianBlur::Configuration > doCreateUbo( renderer::Device const & device
			, renderer::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical )
		{
			auto result = renderer::makeUniformBuffer< castor3d::GaussianBlur::Configuration >( device
				, blurPassesCount
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
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

		std::vector< renderer::SamplerPtr > doCreateSamplers( renderer::Device const & device
			, uint32_t blurPassesCount )
		{
			std::vector< renderer::SamplerPtr > result;

			for ( auto i = 0u; i < blurPassesCount; ++i )
			{
				result.push_back( device.createSampler( renderer::WrapMode::eClampToBorder
					, renderer::WrapMode::eClampToBorder
					, renderer::WrapMode::eClampToBorder
					, renderer::Filter::eNearest
					, renderer::Filter::eNearest
					, renderer::MipmapMode::eNearest
					, float( i )
					, float( i + 1u ) ) );
			}

			return result;
		}

		renderer::RenderPassPtr doCreateRenderPass( renderer::Device const & device
			, renderer::Format format )
		{
			renderer::RenderPassCreateInfo renderPass{};
			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].colorAttachments = { { 0u, renderer::ImageLayout::eColourAttachmentOptimal } };

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eHostWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eHost;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;

			return device.createRenderPass( renderPass );
		}

		renderer::DescriptorSetLayoutPtr doCreateDescriptorLayout( renderer::Device const & device )
		{
			std::vector< renderer::DescriptorSetLayoutBinding > bindings
			{
				renderer::DescriptorSetLayoutBinding{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
				renderer::DescriptorSetLayoutBinding{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}
	}

	//*********************************************************************************************

	BlurPass::Subpass::Subpass( renderer::Device const & device
		, renderer::Format format
		, renderer::TextureView const & srcView
		, renderer::TextureView const & dstView
		, renderer::RenderPass const & renderPass
		, renderer::DescriptorSetPool const & descriptorPool
		, renderer::PipelineLayout const & pipelineLayout
		, renderer::Extent2D dimensions
		, glsl::Shader const & vertexShader
		, glsl::Shader const & pixelShader
		, renderer::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
		, uint32_t index )
	{
		dimensions.width >>= ( index + 1 );
		dimensions.height >>= ( index + 1 );

		sampler = device.createSampler( renderer::WrapMode::eClampToBorder
			, renderer::WrapMode::eClampToBorder
			, renderer::WrapMode::eClampToBorder
			, renderer::Filter::eNearest
			, renderer::Filter::eNearest
			, renderer::MipmapMode::eNearest
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

		renderer::FrameBufferAttachmentArray attaches
		{
			{ *renderPass.getAttachments().begin(), dstView }
		};
		frameBuffer = renderPass.createFrameBuffer( dimensions, std::move( attaches ) );

		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray shaderStages;
		shaderStages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		shaderStages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		shaderStages[0].module->loadShader( vertexShader.getSource() );
		shaderStages[1].module->loadShader( pixelShader.getSource() );

		renderer::GraphicsPipelineCreateInfo pipelineInfo
		{
			shaderStages,
			renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{},
			renderer::DepthStencilState{ 0u, false, false },
			std::nullopt,
			renderer::Viewport{ { 0, 0 }, dimensions },
			renderer::Scissor{ { 0, 0 }, dimensions }
		};
		pipeline = pipelineLayout.createPipeline( pipelineInfo );
	}

	std::vector< BlurPass::Subpass > doCreateSubpasses( renderer::Device const & device
		, renderer::Format format
		, castor3d::TextureLayout const & srcImage
		, castor3d::TextureLayout const & dstImage
		, renderer::RenderPass const & renderPass
		, renderer::DescriptorSetPool const & descriptorPool
		, renderer::PipelineLayout const & pipelineLayout
		, renderer::Extent2D dimensions
		, glsl::Shader const & vertexShader
		, glsl::Shader const & pixelShader
		, renderer::UniformBuffer< castor3d::GaussianBlur::Configuration > const & blurUbo
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

	BlurPass::BlurPass( castor3d::RenderSystem & renderSystem
		, renderer::Format format
		, castor3d::TextureLayout const & srcImage
		, castor3d::TextureLayout const & dstImage
		, renderer::Extent2D dimensions
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
		, m_vertexShader{ getVertexProgram( renderSystem ) }
		, m_pixelShader{ getPixelProgram( renderSystem ) }
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
		, renderer::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const
	{
		castor3d::CommandsSemaphoreArray result;

		for ( auto i = 0u; i < m_blurPassesCount; ++i )
		{
			auto & blur = m_passes[i];

			renderer::CommandBufferPtr commandBuffer = m_device.getGraphicsCommandPool().createCommandBuffer( true );
			auto & cmd = *commandBuffer;

			if ( cmd.begin() )
			{
				timer.beginPass( cmd, 1u + ( m_isVertical ? 1u : 0u ) * m_blurPassesCount + i );
				cmd.beginRenderPass( *m_renderPass
					, *blur.frameBuffer
					, { renderer::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 } }
				, renderer::SubpassContents::eInline );
				cmd.bindPipeline( *blur.pipeline );
				cmd.bindDescriptorSet( *blur.descriptorSet, *m_pipelineLayout );
				cmd.bindVertexBuffer( 0u, vertexBuffer.getBuffer(), 0u );
				cmd.draw( 6u );
				cmd.endRenderPass();
				timer.endPass( cmd, 1u + ( m_isVertical ? 1u : 0u ) * m_blurPassesCount + i );
				cmd.end();
			}

			result.emplace_back( std::move( commandBuffer ), m_device.createSemaphore() );
		}

		return result;
	}
}

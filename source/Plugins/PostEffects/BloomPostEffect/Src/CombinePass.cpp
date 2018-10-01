#include "CombinePass.hpp"

#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Miscellaneous/Parameter.hpp>
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
#include <Shader/GlslToSpv.hpp>
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
				out.gl_Position() = vec4( position, 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getPixelProgram( castor3d::RenderSystem & renderSystem
			, uint32_t blurPassesCount )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapPasses = writer.declSampler< Sampler2D >( CombinePass::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declSampler< Sampler2D >( CombinePass::CombineMapScene, 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = texture( c3d_mapScene, writer.ashesBottomUpToTopDown( vtx_texture ) );

				for ( uint32_t i = 0; i < blurPassesCount; ++i )
				{
					pxl_fragColor += texture( c3d_mapPasses, vtx_texture, Float( float( i ) ) );
				}
			} );
			return writer.finalise();
		}

		castor3d::TextureLayoutSPtr doCreateTexture( castor3d::RenderSystem & renderSystem
			, ashes::Extent2D const & size
			, ashes::Format format )
		{
			ashes::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.mipLevels = 1u;
			image.usage = ashes::ImageUsageFlag::eColourAttachment
				| ashes::ImageUsageFlag::eSampled
				| ashes::ImageUsageFlag::eTransferSrc;
			auto texture = std::make_shared< castor3d::TextureLayout >( renderSystem
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			texture->initialise();
			return texture;
		}

		ashes::TextureViewPtr doCreateView( ashes::Texture const & texture )
		{
			ashes::ImageViewCreateInfo imageView{};
			imageView.format = texture.getFormat();
			imageView.viewType = ashes::TextureViewType::e2D;
			imageView.subresourceRange.aspectMask = ashes::getAspectMask( imageView.format );
			imageView.subresourceRange.baseMipLevel = 0u;
			imageView.subresourceRange.levelCount = 1u;
			imageView.subresourceRange.baseArrayLayer = 0u;
			imageView.subresourceRange.layerCount = 1u;
			return texture.createView( imageView );
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
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

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

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, ashes::TextureView const & view
			, ashes::Extent2D const & size )
		{
			ashes::FrameBufferAttachmentArray attachments{ { *renderPass.getAttachments().begin(), view } };
			return renderPass.createFrameBuffer( size, std::move( attachments ) );
		}

		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout( ashes::Device const & device )
		{
			ashes::DescriptorSetLayoutBindingArray setLayoutBindings
			{
				{ 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
				{ 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment }
			};
			return device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		}

		ashes::PipelinePtr doCreatePipeline( ashes::Device const & device
			, ashes::PipelineLayout const & pipelineLayout
			, glsl::Shader const & vertexShader
			, glsl::Shader const & pixelShader
			, ashes::RenderPass const & renderPass
			, ashes::Extent2D const & size )
		{
			ashes::VertexInputState inputState;
			inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), ashes::VertexInputRate::eVertex } );
			inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, ashes::Format::eR32G32_SFLOAT, 0u } );

			ashes::ShaderStageStateArray stages;
			stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
			stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
			stages[0].module->loadShader( castor3d::compileGlslToSpv( device
				, ashes::ShaderStageFlag::eVertex
				, vertexShader.getSource() ) );
			stages[1].module->loadShader( castor3d::compileGlslToSpv( device
				, ashes::ShaderStageFlag::eFragment
				, pixelShader.getSource() ) );

			ashes::GraphicsPipelineCreateInfo pipeline
			{
				stages,
				renderPass,
				inputState,
				ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
				ashes::RasterisationState{},
				ashes::MultisampleState{},
				ashes::ColourBlendState::createDefault(),
				{},
				ashes::DepthStencilState{ 0u, false, false },
				std::nullopt,
				ashes::Viewport{ { 0, 0 }, size },
				ashes::Scissor{ { 0, 0 }, size }
			};
			return pipelineLayout.createPipeline( pipeline );
		}

		ashes::DescriptorSetPtr doCreateDescriptorSet( ashes::DescriptorSetPool const & pool
			, ashes::TextureView const & sceneView
			, ashes::TextureView const & blurView
			, ashes::Sampler const & sceneSampler
			, ashes::Sampler const & blurSampler )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			result->createBinding( layout.getBinding( 0u )
				, blurView
				, blurSampler
				, ashes::ImageLayout::eShaderReadOnlyOptimal );
			result->createBinding( layout.getBinding( 1u )
				, sceneView
				, sceneSampler
				, ashes::ImageLayout::eShaderReadOnlyOptimal );
			result->update();
			return result;
		}
	}

	//*********************************************************************************************

	String const CombinePass::CombineMapPasses = cuT( "c3d_mapPasses" );
	String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );

	CombinePass::CombinePass( castor3d::RenderSystem & renderSystem
		, ashes::Format format
		, ashes::TextureView const & sceneView
		, ashes::TextureView const & blurView
		, ashes::Extent2D const & size
		, uint32_t blurPassesCount )
		: m_device{ getCurrentDevice( renderSystem ) }
		, m_image{ doCreateTexture( renderSystem, size, format ) }
		, m_view{ doCreateView( m_image->getTexture() ) }
		, m_vertexShader{ getVertexProgram( renderSystem ) }
		, m_pixelShader{ getPixelProgram( renderSystem, blurPassesCount ) }
		, m_renderPass{ doCreateRenderPass( m_device, format ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, *m_view, size ) }
		, m_descriptorLayout{ doCreateDescriptorLayout( m_device ) }
		, m_pipelineLayout{ m_device.createPipelineLayout( *m_descriptorLayout ) }
		, m_pipeline{ doCreatePipeline( m_device, *m_pipelineLayout, m_vertexShader, m_pixelShader, *m_renderPass, size ) }
		, m_sceneSampler{ m_device.createSampler( ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::Filter::eLinear
			, ashes::Filter::eLinear ) }
		, m_blurSampler{ m_device.createSampler( ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::Filter::eLinear
			, ashes::Filter::eLinear
			, ashes::MipmapMode::eNearest
			, 0.0f
			, float( blurPassesCount ) ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( 1u ) }
		, m_descriptorSet{ doCreateDescriptorSet( *m_descriptorPool, sceneView, blurView, *m_sceneSampler, *m_blurSampler ) }
		, m_blurPassesCount{ blurPassesCount }
	{
	}

	castor3d::CommandsSemaphore CombinePass::getCommands( castor3d::RenderPassTimer const & timer
		, ashes::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const
	{
		auto result = m_device.getGraphicsCommandPool().createCommandBuffer( true );
		auto & cmd = *result;

		cmd.begin();
		timer.beginPass( cmd, 1u + ( m_blurPassesCount * 2u ) );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { ashes::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 } }
			, ashes::SubpassContents::eInline );
		cmd.bindPipeline( *m_pipeline );
		cmd.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		cmd.bindVertexBuffer( 0u, vertexBuffer.getBuffer(), 0u );
		cmd.draw( 6u );
		cmd.endRenderPass();
		timer.endPass( cmd, 1u + ( m_blurPassesCount * 2u ) );
		cmd.end();

		return { std::move( result ), m_device.createSemaphore() };
	}
}

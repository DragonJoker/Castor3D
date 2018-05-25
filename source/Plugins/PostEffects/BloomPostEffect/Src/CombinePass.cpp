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
				auto texcoords = writer.declLocale( cuT( "texcoords" )
					, vec2( vtx_texture.x(), 1.0_f - vtx_texture.y() ) );

				if ( writer.isTopDown() )
				{
					pxl_fragColor = texture( c3d_mapScene, vtx_texture );
				}
				else
				{
					pxl_fragColor = texture( c3d_mapScene, texcoords );
				}

				for ( uint32_t i = 0; i < blurPassesCount; ++i )
				{
					pxl_fragColor += texture( c3d_mapPasses, texcoords, Float( float( i ) ) );
				}
			} );
			return writer.finalise();
		}

		castor3d::TextureLayoutSPtr doCreateTexture( castor3d::RenderSystem & renderSystem
			, renderer::Extent2D const & size
			, renderer::Format format )
		{
			renderer::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.mipLevels = 1u;
			image.usage = renderer::ImageUsageFlag::eColourAttachment
				| renderer::ImageUsageFlag::eSampled
				| renderer::ImageUsageFlag::eTransferSrc;
			auto texture = std::make_shared< castor3d::TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			texture->initialise();
			return texture;
		}

		renderer::TextureViewPtr doCreateView( renderer::Texture const & texture )
		{
			renderer::ImageViewCreateInfo imageView{};
			imageView.format = texture.getFormat();
			imageView.viewType = renderer::TextureViewType::e2D;
			imageView.subresourceRange.aspectMask = renderer::getAspectMask( imageView.format );
			imageView.subresourceRange.baseMipLevel = 0u;
			imageView.subresourceRange.levelCount = 1u;
			imageView.subresourceRange.baseArrayLayer = 0u;
			imageView.subresourceRange.layerCount = 1u;
			return texture.createView( imageView );
		}

		renderer::RenderPassPtr doCreateRenderPass( castor3d::RenderSystem const & renderSystem
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
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

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

			return renderSystem.getCurrentDevice()->createRenderPass( renderPass );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, renderer::TextureView const & view
			, renderer::Extent2D const & size )
		{
			renderer::FrameBufferAttachmentArray attachments{ { *renderPass.getAttachments().begin(), view } };
			return renderPass.createFrameBuffer( size, std::move( attachments ) );
		}

		renderer::DescriptorSetLayoutPtr doCreateDescriptorLayout( castor3d::RenderSystem const & renderSystem )
		{
			renderer::DescriptorSetLayoutBindingArray setLayoutBindings
			{
				{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
				{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
			};
			return renderSystem.getCurrentDevice()->createDescriptorSetLayout( std::move( setLayoutBindings ) );
		}

		renderer::PipelinePtr doCreatePipeline( castor3d::RenderSystem const & renderSystem
			, renderer::PipelineLayout const & pipelineLayout
			, glsl::Shader const & vertexShader
			, glsl::Shader const & pixelShader
			, renderer::RenderPass const & renderPass
			, renderer::Extent2D const & size )
		{
			auto & device = *renderSystem.getCurrentDevice();
			renderer::VertexInputState inputState;
			inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), renderer::VertexInputRate::eVertex } );
			inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

			renderer::ShaderStageStateArray stages;
			stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
			stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
			stages[0].module->loadShader( vertexShader.getSource() );
			stages[1].module->loadShader( pixelShader.getSource() );

			renderer::GraphicsPipelineCreateInfo pipeline
			{
				stages,
				renderPass,
				inputState,
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
				renderer::RasterisationState{},
				renderer::MultisampleState{},
				renderer::ColourBlendState::createDefault(),
				{},
				renderer::DepthStencilState{ 0u, false, false },
				std::nullopt,
				renderer::Viewport{ { 0, 0 }, size },
				renderer::Scissor{ { 0, 0 }, size }
			};
			return pipelineLayout.createPipeline( pipeline );
		}

		renderer::DescriptorSetPtr doCreateDescriptorSet( renderer::DescriptorSetPool const & pool
			, renderer::TextureView const & sceneView
			, renderer::TextureView const & blurView
			, renderer::Sampler const & sceneSampler
			, renderer::Sampler const & blurSampler )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			result->createBinding( layout.getBinding( 0u )
				, blurView
				, blurSampler
				, renderer::ImageLayout::eShaderReadOnlyOptimal );
			result->createBinding( layout.getBinding( 1u )
				, sceneView
				, sceneSampler
				, renderer::ImageLayout::eShaderReadOnlyOptimal );
			result->update();
			return result;
		}
	}

	//*********************************************************************************************

	String const CombinePass::CombineMapPasses = cuT( "c3d_mapPasses" );
	String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );

	CombinePass::CombinePass( castor3d::RenderSystem & renderSystem
		, renderer::Format format
		, renderer::TextureView const & sceneView
		, renderer::TextureView const & blurView
		, renderer::Extent2D const & size
		, uint32_t blurPassesCount )
		: m_device{ *renderSystem.getCurrentDevice() }
		, m_image{ doCreateTexture( renderSystem, size, format ) }
		, m_view{ doCreateView( m_image->getTexture() ) }
		, m_vertexShader{ getVertexProgram( renderSystem ) }
		, m_pixelShader{ getPixelProgram( renderSystem, blurPassesCount ) }
		, m_renderPass{ doCreateRenderPass( renderSystem, format ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, *m_view, size ) }
		, m_descriptorLayout{ doCreateDescriptorLayout( renderSystem ) }
		, m_pipelineLayout{ renderSystem.getCurrentDevice()->createPipelineLayout( *m_descriptorLayout ) }
		, m_pipeline{ doCreatePipeline( renderSystem, *m_pipelineLayout, m_vertexShader, m_pixelShader, *m_renderPass, size ) }
		, m_sceneSampler{ renderSystem.getCurrentDevice()->createSampler( renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::Filter::eLinear
			, renderer::Filter::eLinear ) }
		, m_blurSampler{ renderSystem.getCurrentDevice()->createSampler( renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::Filter::eLinear
			, renderer::Filter::eLinear
			, renderer::MipmapMode::eNearest
			, 0.0f
			, float( blurPassesCount ) ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( 1u ) }
		, m_descriptorSet{ doCreateDescriptorSet( *m_descriptorPool, sceneView, blurView, *m_sceneSampler, *m_blurSampler ) }
		, m_blurPassesCount{ blurPassesCount }
	{
	}

	castor3d::CommandsSemaphore CombinePass::getCommands( castor3d::RenderPassTimer const & timer
		, renderer::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const
	{
		auto result = m_device.getGraphicsCommandPool().createCommandBuffer( true );
		auto & cmd = *result;

		if ( cmd.begin() )
		{
			timer.beginPass( cmd, 1u + ( m_blurPassesCount * 2u ) );
			cmd.beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { renderer::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 } }
				, renderer::SubpassContents::eInline );
			cmd.bindPipeline( *m_pipeline );
			cmd.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			cmd.bindVertexBuffer( 0u, vertexBuffer.getBuffer(), 0u );
			cmd.draw( 6u );
			cmd.endRenderPass();
			timer.endPass( cmd, 1u + ( m_blurPassesCount * 2u ) );
			cmd.end();
		}

		return { std::move( result ), m_device.createSemaphore() };
	}
}

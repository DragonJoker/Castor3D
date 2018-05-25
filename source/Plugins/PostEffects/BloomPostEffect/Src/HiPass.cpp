#include "HiPass.hpp"

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
			Vec2 texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texcoord;
				out.gl_Position() = vec4( position, 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getPixelProgram( castor3d::RenderSystem & renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, writer.adjustTexCoords( vtx_texture ), 0.0f ).xyz(), 1.0 );
				auto maxComponent = writer.declLocale( cuT( "maxComponent" ), glsl::max( pxl_fragColor.r(), pxl_fragColor.g() ) );
				maxComponent = glsl::max( maxComponent, pxl_fragColor.b() );

				IF( writer, maxComponent > 1.0_f )
				{
					pxl_fragColor.xyz() /= maxComponent;
				}
				ELSE
				{
					pxl_fragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );
			return writer.finalise();
		}

		castor3d::TextureLayoutSPtr doCreateTexture( castor3d::RenderSystem & renderSystem
			, renderer::Extent2D const & size
			, renderer::Format format
			, uint32_t blurPassesCount )
		{
			renderer::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
#if Bloom_DebugHiPass
			image.extent.width = size.width;
			image.extent.height = size.height;
#else
			image.extent.width = size.width >> 1;
			image.extent.height = size.height >> 1;
#endif
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = blurPassesCount;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eColourAttachment
				| renderer::ImageUsageFlag::eSampled
				| renderer::ImageUsageFlag::eTransferDst
				| renderer::ImageUsageFlag::eTransferSrc;
			auto texture = std::make_shared< castor3d::TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			texture->initialise();
			return texture;
		}

		renderer::RenderPassPtr doCreateRenderPass( renderer::Device const & device
			, renderer::Format format )
		{
			renderer::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, renderer::TextureView const & view
			, renderer::Extent2D const & size )
		{
			renderer::FrameBufferAttachmentArray attachments{ { *renderPass.getAttachments().begin(), view } };
#if Bloom_DebugHiPass
			return renderPass.createFrameBuffer( { size.width, size.height }, std::move( attachments ) );
#else
			return renderPass.createFrameBuffer( { size.width >> 1, size.height >> 1 }, std::move( attachments ) );
#endif
		}
	}

	//*********************************************************************************************

	HiPass::HiPass( castor3d::RenderSystem & renderSystem
		, renderer::Format format
		, renderer::TextureView const & sceneView
		, renderer::Extent2D size
		, uint32_t blurPassesCount )
		: castor3d::RenderQuad{ renderSystem, true }
		, m_device{ *renderSystem.getCurrentDevice() }
		, m_sceneView{ sceneView }
		, m_image{ doCreateTexture( renderSystem, size, format, blurPassesCount ) }
		, m_vertexShader{ getVertexProgram( renderSystem ) }
		, m_pixelShader{ getPixelProgram( renderSystem ) }
		, m_renderPass{ doCreateRenderPass( m_device, format ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_image->getImage().getView(), size ) }
	{
		renderer::ShaderStageStateArray shaderStages;
		shaderStages.push_back( { m_device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		shaderStages.push_back( { m_device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		shaderStages[0].module->loadShader( m_vertexShader.getSource() );
		shaderStages[1].module->loadShader( m_pixelShader.getSource() );

#if !Bloom_DebugHiPass
		size.width >>= 1;
		size.height >>= 1;
#endif

		renderer::DescriptorSetLayoutBindingArray bindings;
		this->createPipeline( size
			, {}
			, shaderStages
			, sceneView
			, *m_renderPass
			, bindings
			, {} );
	}

	castor3d::CommandsSemaphore HiPass::getCommands( castor3d::RenderPassTimer const & timer
		, renderer::VertexBuffer< castor3d::NonTexturedQuad > const & vertexBuffer )const
	{
		auto result = m_device.getGraphicsCommandPool().createCommandBuffer( true );
		auto & cmd = *result;

		if ( cmd.begin() )
		{
			timer.beginPass( cmd, 0u );
			cmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_sceneView.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			cmd.beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { renderer::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 } }
				, renderer::SubpassContents::eInline );
			registerFrame( cmd );
			cmd.endRenderPass();
#if !Bloom_DebugHiPass
			m_image->getTexture().generateMipmaps( cmd );
#endif
			timer.endPass( cmd, 0u );
			cmd.end();
		}

		return { std::move( result ), m_device.createSemaphore() };
	}
}

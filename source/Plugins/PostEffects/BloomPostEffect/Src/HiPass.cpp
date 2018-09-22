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
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vtx_texture, 0.0f ).xyz(), 1.0 );
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

		ashes::RenderPassPtr doCreateRenderPass( ashes::Device const & device
			, ashes::Format format )
		{
			ashes::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = format;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}
	}

	//*********************************************************************************************

	HiPass::HiPass( castor3d::RenderSystem & renderSystem
		, ashes::Format format
		, ashes::TextureView const & sceneView
		, ashes::Extent2D size
		, uint32_t blurPassesCount )
		: castor3d::RenderQuad{ renderSystem, true }
		, m_device{ getCurrentDevice( renderSystem ) }
		, m_sceneView{ sceneView }
		, m_vertexShader{ getVertexProgram( renderSystem ) }
		, m_pixelShader{ getPixelProgram( renderSystem ) }
		, m_renderPass{ doCreateRenderPass( m_device, format ) }
		, m_surface{ *renderSystem.getEngine() }
	{
		ashes::ShaderStageStateArray shaderStages;
		shaderStages.push_back( { m_device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		shaderStages.push_back( { m_device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		shaderStages[0].module->loadShader( castor3d::compileGlslToSpv( m_device
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader.getSource() ) );
		shaderStages[1].module->loadShader( castor3d::compileGlslToSpv( m_device
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader.getSource() ) );

#if !Bloom_DebugHiPass
		size.width >>= 1;
		size.height >>= 1;
#else
		blurPassesCount = 1u;
#endif

		ashes::DescriptorSetLayoutBindingArray bindings;
		this->createPipeline( size
			, {}
			, shaderStages
			, sceneView
			, *m_renderPass
			, bindings
			, {} );
		m_surface.initialise( *m_renderPass
			, { size.width, size.height }
			, m_sceneView.getFormat()
			, blurPassesCount );
	}

	castor3d::CommandsSemaphore HiPass::getCommands( castor3d::RenderPassTimer const & timer )const
	{
		castor3d::CommandsSemaphore commands
		{
			m_device.getGraphicsCommandPool().createCommandBuffer(),
			m_device.createSemaphore()
		};
		auto & cmd = *commands.commandBuffer;

		cmd.begin();
		timer.beginPass( cmd, 0u );

		// Put target image in shader input layout.
		cmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_sceneView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

		cmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, { ashes::ClearColorValue{} }
			, ashes::SubpassContents::eInline );
		registerFrame( cmd );
		cmd.endRenderPass();
#if !Bloom_DebugHiPass
		m_surface.colourTexture->getTexture().generateMipmaps( cmd );
#endif

		timer.endPass( cmd, 0u );
		cmd.end();

		return commands;
	}
}

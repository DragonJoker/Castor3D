#include "EdgeDetection.hpp"

#include "SmaaUbo.hpp"
#include "SMAA.hpp"

#include <Engine.hpp>

#include <Render/RenderPassTimer.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Core/Renderer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doGetEdgeDetectionVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			writeConstants( writer, config, renderTargetMetrics );
			writer << getEdgeDetectionVS();

			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 3u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;
					writer << "SMAAEdgeDetectionVS( vtx_texture, vtx_offset )" << endi;
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	EdgeDetection::EdgeDetection( castor3d::RenderTarget & renderTarget
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), false, false }
		, m_config{ config }
		, m_surface{ *renderTarget.getEngine() }
	{
		ashes::Extent2D size{ renderTarget.getSize().getWidth()
			, renderTarget.getSize().getHeight() };
		auto & device = getCurrentDevice( renderTarget );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = ashes::Format::eR8G8B8A8_UNORM;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[1].format = ashes::Format::eS8_UINT;
		renderPass.attachments[1].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].depthStencilAttachment = { 1u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eMemoryRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		m_surface.initialise( *m_renderPass
			, renderTarget.getSize()
			, ashes::Format::eR8G8B8A8_UNORM
			, ashes::Format::eS8_UINT );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader = doGetEdgeDetectionVP( m_renderSystem
			, pixelSize
			, m_config );
	}

	void EdgeDetection::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "EdgeDetection" )
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "EdgeDetection" )
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader );
	}
}

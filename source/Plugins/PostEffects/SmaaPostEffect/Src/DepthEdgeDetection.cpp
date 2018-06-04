#include "DepthEdgeDetection.hpp"

#include "SmaaUbo.hpp"

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
			, Point4f const & renderTargetMetrics )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_renderTargetMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 3u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texcoord;
					out.gl_Position() = vec4( position, 0.0, 1.0 );

					vtx_offset[0] = fma( c3d_renderTargetMetrics.xyxy(), vec4( -1.0_f, 0.0, 0.0, -1.0 ), vec4( vtx_texture, vtx_texture ) );
					vtx_offset[1] = fma( c3d_renderTargetMetrics.xyxy(), vec4( +1.0_f, 0.0, 0.0, +1.0 ), vec4( vtx_texture, vtx_texture ) );
					vtx_offset[2] = fma( c3d_renderTargetMetrics.xyxy(), vec4( -2.0_f, 0.0, 0.0, -2.0 ), vec4( vtx_texture, vtx_texture ) );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetEdgeDetectionFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();
			writer.enableExtension( cuT( "GL_ARB_texture_gather" ), 400u );

			// Shader inputs
			auto c3d_renderTargetMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.threshold ) );
			auto c3d_depthThreshold = writer.declConstant( constants::DepthThreshold
				, c3d_threshold * 0.1_f );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 3u );
			auto c3d_depthTex = writer.declSampler< Sampler2D >( cuT( "c3d_depthTex" ), 0u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			/**
			* Gathers current pixel, and the top-left neighbors.
			*/
			Function< Vec2 > calculatePredicatedThreshold;

			auto gatherNeighbours = writer.implementFunction< Vec3 >( cuT( "gatherNeighbours" )
				, [&]()
				{
					writer.returnStmt( textureGather( c3d_depthTex, vtx_texture + c3d_renderTargetMetrics.xy() * vec2( -0.5_f ) ).grb() );
				} );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto neighbours = writer.declLocale( cuT( "neighbours" )
						, gatherNeighbours() );
					auto delta = writer.declLocale( cuT( "delta" )
						, abs( neighbours.xx() - vec2( neighbours.y(), neighbours.z() ) ) );
					auto edges = writer.declLocale( cuT( "edges" )
						, step( vec2( c3d_depthThreshold ), delta ) );

					IF( writer, dot( edges, vec2( 1.0_f ) ) == 0.0_f )
					{
						writer.discard();
					}
					FI

					pxl_fragColour = vec4( edges, 0.0_f, 0.0_f );
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	DepthEdgeDetection::DepthEdgeDetection( castor3d::RenderTarget & renderTarget
		, renderer::TextureView const & depthView
		, castor3d::SamplerSPtr sampler
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_surface{ *renderTarget.getEngine() }
		, m_depthView{ depthView }
	{
		renderer::Extent2D size{ m_depthView.getTexture().getDimensions().width
			, m_depthView.getTexture().getDimensions().height };
		auto & device = getCurrentDevice( renderTarget );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = renderer::Format::eR16G16B16A16_SFLOAT;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[1].format = renderer::Format::eS8_UINT;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].depthStencilAttachment = { 1u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader = doGetEdgeDetectionVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize );
		m_pixelShader = doGetEdgeDetectionFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_vertexShader.getSource() );
		stages[1].module->loadShader( m_pixelShader.getSource() );

		renderer::DepthStencilState dsstate{ 0u, false, false };
		dsstate.stencilTestEnable = true;
		dsstate.front =
		{
			renderer::StencilOp::eReplace,
			renderer::StencilOp::eKeep,
			renderer::StencilOp::eKeep,
			renderer::CompareOp::eNever,
			0xFFFFFFFFu,
			0xFFFFFFFFu,
			0x00000001u,
		};
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings;
		createPipeline( size
			, castor::Position{}
			, stages
			, m_depthView
			, *m_renderPass
			, setLayoutBindings
			, {}
			, dsstate );
		m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, renderer::Format::eR8G8B8A8_UNORM
			, renderer::Format::eS8_UINT );
	}

	castor3d::CommandsSemaphore DepthEdgeDetection::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		castor3d::CommandsSemaphore edgeDetectionCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & edgeDetectionCmd = *edgeDetectionCommands.commandBuffer;

		if ( edgeDetectionCmd.begin() )
		{
			timer.beginPass( edgeDetectionCmd, passIndex );
			// Put source image in shader input layout.
			edgeDetectionCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_depthView.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

			edgeDetectionCmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { renderer::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 }, renderer::DepthStencilClearValue{ 1.0f, 0 } }
				, renderer::SubpassContents::eInline );
			registerFrame( edgeDetectionCmd );
			edgeDetectionCmd.endRenderPass();
			timer.endPass( edgeDetectionCmd, passIndex );
			edgeDetectionCmd.end();
		}

		return std::move( edgeDetectionCommands );
	}

	void DepthEdgeDetection::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "DepthEdgeDetection" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "DepthEdgeDetection" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
	}
}

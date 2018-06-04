#include "Reproject.hpp"

#include "SmaaUbo.hpp"

#include <Engine.hpp>

#include <Render/RenderPassTimer.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Buffer/UniformBuffer.hpp>
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
		glsl::Shader doGetReprojectVP( castor3d::RenderSystem & renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;
				} );
			return writer.finalise();
		}

		glsl::Shader doGetReprojectFP( castor3d::RenderSystem & renderSystem
			, float reprojectionWeightScale
			, bool reprojection )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			auto c3d_reprojectionWeightScale = writer.declConstant( cuT( "c3d_reprojectionWeightScale" )
				, Float( reprojectionWeightScale )
				, reprojection );

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto c3d_currentColourTex = writer.declSampler< Sampler2D >( cuT( "c3d_currentColourTex" ), 0u, 0u );
			auto c3d_previousColourTex = writer.declSampler< Sampler2D >( cuT( "c3d_previousColourTex" ), 1u, 0u );
			auto c3d_velocityTex = writer.declSampler< Sampler2D >( cuT( "c3d_velocityTex" ), 2u, 0u, reprojection );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					if ( reprojection )
					{
						// Velocity is assumed to be calculated for motion blur, so we need to
						// inverse it for reprojection:
						auto velocity = writer.declLocale( cuT( "velocity" )
							, -texture( c3d_velocityTex, vtx_texture ).rg() );

						// Fetch current pixel:
						auto current = writer.declLocale( cuT( "current" )
							, texture( c3d_currentColourTex, vtx_texture ) );

						// Reproject current coordinates and fetch previous pixel:
						auto previous = writer.declLocale( cuT( "previous" )
							, texture( c3d_previousColourTex, vtx_texture + velocity ) );

						// Attenuate the previous pixel if the velocity is different:
						auto delta = writer.declLocale( cuT( "delta" )
							, abs( current.a() * current.a() - previous.a() * previous.a() ) / 5.0 );
						auto weight = writer.declLocale( cuT( "weight" )
							, 0.5_f * clamp( 1.0_f - writer.paren( sqrt( delta ) * c3d_reprojectionWeightScale ), 0.0_f, 1.0_f ) );

						// Blend the pixels according to the calculated weight:
						pxl_fragColour = mix( current, previous, weight );
					}
					else
					{
						// Just blend the pixels:
						auto current = writer.declLocale( cuT( "current" )
							, texture( c3d_currentColourTex, vtx_texture ) );
						auto previous = writer.declLocale( cuT( "previous" )
							, texture( c3d_previousColourTex, vtx_texture ) );
						pxl_fragColour = mix( current, previous, 0.5 );
					}
				} );
			return writer.finalise();
		}
	}
	
	//*********************************************************************************************

	Reproject::Reproject( castor3d::RenderTarget & renderTarget
		, renderer::TextureView const & currentColourView
		, renderer::TextureView const & previousColourView
		, renderer::TextureView const * velocityView
		, castor3d::SamplerSPtr sampler
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_currentColourView{ currentColourView }
		, m_previousColourView{ previousColourView }
		, m_velocityView{ velocityView }
		, m_surface{ *renderTarget.getEngine() }
	{
		renderer::Extent2D size{ m_currentColourView.getTexture().getDimensions().width, m_currentColourView.getTexture().getDimensions().height };
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = renderTarget.getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point2f{ 1.0f / size.width, 1.0f / size.height };
		m_vertexShader = doGetReprojectVP( *renderTarget.getEngine()->getRenderSystem() );
		m_pixelShader = doGetReprojectFP( *renderTarget.getEngine()->getRenderSystem()
			, velocityView != nullptr
			, config.reprojectionWeightScale );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_vertexShader.getSource() );
		stages[1].module->loadShader( m_pixelShader.getSource() );

		renderer::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		auto * view = &m_previousColourView;

		if ( m_velocityView )
		{
			setLayoutBindings.emplace_back( 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
			view = m_velocityView;
		}

		createPipeline( size
			, castor::Position{}
			, stages
			, *view
			, *m_renderPass
			, setLayoutBindings
			, {} );
		m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, renderTarget.getPixelFormat() );
	}

	castor3d::CommandsSemaphore Reproject::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		castor3d::CommandsSemaphore reprojectCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & reprojectCmd = *reprojectCommands.commandBuffer;

		if ( reprojectCmd.begin() )
		{
			timer.beginPass( reprojectCmd, passIndex );
			// Put neighbourhood images in shader input layout.
			reprojectCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_currentColourView.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			reprojectCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_previousColourView.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

			reprojectCmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { renderer::ClearColorValue{} }
				, renderer::SubpassContents::eInline );
			registerFrame( reprojectCmd );
			reprojectCmd.endRenderPass();
			timer.endPass( reprojectCmd, passIndex );
			reprojectCmd.end();
		}

		return std::move( reprojectCommands );
	}

	void Reproject::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "Reproject" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "Reproject" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	void Reproject::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_currentColourView
			, m_sampler->getSampler() );

		if ( m_velocityView )
		{
			descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
				, m_previousColourView
				, m_sampler->getSampler() );
		}
	}

	//*********************************************************************************************
}

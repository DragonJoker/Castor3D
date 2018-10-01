#include "Reproject.hpp"

#include "SmaaUbo.hpp"
#include "SMAA.hpp"

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
#include <Shader/GlslToSpv.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doGetReprojectVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
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
					vtx_texture = writer.ashesBottomUpToTopDown( texcoord );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetReprojectFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config
			, bool reprojection )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();
			writeConstants( writer, config, renderTargetMetrics );
			writer.declConstant( constants::Reprojection
				, 1_i
				, reprojection );
			writer.declConstant( constants::ReprojectionWeightScale
				, Float( config.data.reprojectionWeightScale ) );
			writer << getResolvePS();

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
						writer << "pxl_fragColour = SMAAResolvePS( vtx_texture, c3d_currentColourTex, c3d_previousColourTex, c3d_velocityTex )" << endi;
					}
					else
					{
						writer << "pxl_fragColour = SMAAResolvePS( vtx_texture, c3d_currentColourTex, c3d_previousColourTex )" << endi;
					}
				} );
			return writer.finalise();
		}
	}
	
	//*********************************************************************************************

	Reproject::Reproject( castor3d::RenderTarget & renderTarget
		, ashes::TextureView const & currentColourView
		, ashes::TextureView const & previousColourView
		, ashes::TextureView const * velocityView
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_currentColourView{ currentColourView }
		, m_previousColourView{ previousColourView }
		, m_velocityView{ velocityView }
		, m_surface{ *renderTarget.getEngine() }
	{
		ashes::Extent2D size{ m_currentColourView.getTexture().getDimensions().width, m_currentColourView.getTexture().getDimensions().height };
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = renderTarget.getPixelFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader = doGetReprojectVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		m_pixelShader = doGetReprojectFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config
			, velocityView != nullptr );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader.getSource() ) );
		stages[1].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader.getSource() ) );

		ashes::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		auto * view = &m_previousColourView;

		if ( m_velocityView )
		{
			setLayoutBindings.emplace_back( 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
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

		reprojectCmd.begin();
		timer.beginPass( reprojectCmd, passIndex );
		// Put neighbourhood images in shader input layout.
		reprojectCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_currentColourView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
		reprojectCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_previousColourView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

		reprojectCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, { ashes::ClearColorValue{} }
			, ashes::SubpassContents::eInline );
		registerFrame( reprojectCmd );
		reprojectCmd.endRenderPass();
		timer.endPass( reprojectCmd, passIndex );
		reprojectCmd.end();

		return std::move( reprojectCommands );
	}

	void Reproject::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "Reproject" )
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "Reproject" )
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	void Reproject::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
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

#include "SmaaPostEffect/Reproject.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"
#include "SmaaPostEffect/SMAA.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Texture/Sampler.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>

#include <Ashes/Buffer/UniformBuffer.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/Pipeline/DepthStencilState.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace smaa
{
	namespace
	{
		std::unique_ptr< sdw::Shader > doGetReprojectVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer, renderSystem.isTopDown() };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
					vtx_texture = utils.bottomUpToTopDown( uv );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > doGetReprojectFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config
			, bool reprojection )
		{
			using namespace sdw;
			FragmentWriter writer;
			auto c3d_reprojectionWeightScale = writer.declConstant( constants::ReprojectionWeightScale
				, Float( config.data.reprojectionWeightScale ) );

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto c3d_currentColourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_currentColourTex", 0u, 0u );
			auto c3d_previousColourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_previousColourTex", 1u, 0u );
			auto c3d_velocityTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_velocityTex", 2u, 0u, reprojection );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			auto SMAAResolvePS = writer.implementFunction< Vec4 >( "SMAAResolvePS"
				, [&]( Vec2 const & texcoord
					, SampledImage2DRgba32 const & currentColorTex
					, SampledImage2DRgba32 const & previousColorTex )
				{
					if ( config.data.enableReprojection )
					{
						// Velocity is assumed to be calculated for motion blur, so we need to
						// inverse it for reprojection:
						auto velocity = writer.declLocale( "velocity"
							, -texture( c3d_velocityTex, texcoord ).rg() );

						// Fetch current pixel:
						auto current = writer.declLocale( "current"
							, texture( currentColorTex, texcoord ) );

						// Reproject current coordinates and fetch previous pixel:
						auto previous = writer.declLocale( "previous"
							, texture( previousColorTex, texcoord + velocity ) );

						// Attenuate the previous pixel if the velocity is different:
						auto delta = writer.declLocale( "delta"
							, abs( current.a() * current.a() - previous.a() * previous.a() ) / 5.0_f );
						auto weight = writer.declLocale( "weight"
							, 0.5_f * clamp( 1.0_f - sqrt( delta ) * c3d_reprojectionWeightScale, 0.0_f, 1.0_f ) );

						// Blend the pixels according to the calculated weight:
						writer.returnStmt( mix( current, previous, vec4( weight ) ) );
					}
					else
					{
						// Just blend the pixels:
						auto current = writer.declLocale( "current"
							, texture( currentColorTex, texcoord ) );
						auto previous = writer.declLocale( "previous"
							, texture( previousColorTex, texcoord ) );
						writer.returnStmt( mix( current, previous, vec4( 0.5_f ) ) );
					}
				}
				, InVec2{ writer, "texcoord" }
				, InSampledImage2DRgba32{ writer, "currentColorTex" }
				, InSampledImage2DRgba32{ writer, "previousColorTex" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = SMAAResolvePS( vtx_texture, c3d_currentColourTex, c3d_previousColourTex );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
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
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "SmaaReproject" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "SmaaReproject" }
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
		m_vertexShader.shader = doGetReprojectVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		m_pixelShader.shader = doGetReprojectFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config
			, velocityView != nullptr );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( renderSystem.compileShader( m_vertexShader ) );
		stages[1].module->loadShader( renderSystem.compileShader( m_pixelShader ) );

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
			, *m_vertexShader.shader );
		visitor.visit( cuT( "Reproject" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
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

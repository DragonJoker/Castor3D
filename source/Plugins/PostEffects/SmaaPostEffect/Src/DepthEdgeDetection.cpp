#include "DepthEdgeDetection.hpp"

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
#include <Shader/GlslToSpv.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doGetEdgeDetectionFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();
			writer.enableExtension( cuT( "GL_ARB_texture_gather" ), 400u );

			// Shader inputs
			writeConstants( writer, config, renderTargetMetrics );
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			auto c3d_depthThreshold = writer.declConstant( constants::DepthThreshold
				, c3d_threshold * 0.1_f );
			writer << getDepthEdgeDetectionPS();

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 3u );
			auto c3d_depthTex = writer.declSampler< Sampler2D >( cuT( "c3d_depthTex" ), 0u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );
					writer << "pxl_fragColour.xy = SMAADepthEdgeDetectionPS( vtx_texture, vtx_offset, c3d_depthTex )" << endi;
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	DepthEdgeDetection::DepthEdgeDetection( castor3d::RenderTarget & renderTarget
		, ashes::TextureView const & depthView
		, SmaaConfig const & config )
		: EdgeDetection{ renderTarget, config }
		, m_depthView{ depthView }
	{
		ashes::Extent2D size{ m_depthView.getTexture().getDimensions().width
			, m_depthView.getTexture().getDimensions().height };
		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_pixelShader = doGetEdgeDetectionFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		doInitialisePipeline();
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

		edgeDetectionCmd.begin();
		timer.beginPass( edgeDetectionCmd, passIndex );
		// Put source image in shader input layout.
		edgeDetectionCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_depthView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

		edgeDetectionCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, { ashes::ClearColorValue{ 0.0, 0.0, 0.0, 0.0 }, ashes::DepthStencilClearValue{ 1.0f, 0 } }
			, ashes::SubpassContents::eInline );
		registerFrame( edgeDetectionCmd );
		edgeDetectionCmd.endRenderPass();
		timer.endPass( edgeDetectionCmd, passIndex );
		edgeDetectionCmd.end();

		return std::move( edgeDetectionCommands );
	}

	void DepthEdgeDetection::doInitialisePipeline()
	{
		ashes::Extent2D size{ m_depthView.getTexture().getDimensions().width
			, m_depthView.getTexture().getDimensions().height };
		auto & device = getCurrentDevice( m_renderSystem );
		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader.getSource() ) );
		stages[1].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader.getSource() ) );

		ashes::DepthStencilState dsstate{ 0u, false, false };
		dsstate.stencilTestEnable = true;
		dsstate.front.passOp = ashes::StencilOp::eReplace;
		dsstate.front.reference = 1u;
		dsstate.back = dsstate.front;
		ashes::DescriptorSetLayoutBindingArray setLayoutBindings;
		createPipeline( size
			, castor::Position{}
			, stages
			, m_depthView
			, *m_renderPass
			, setLayoutBindings
			, {}
			, dsstate );
	}
}

#include "DepthEdgeDetection.hpp"

#include "SmaaUbo.hpp"
#include "SMAA.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Shader/Shaders/GlslUtils.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Core/Renderer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		std::unique_ptr< sdw::Shader > doGetEdgeDetectionFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader constants
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			auto c3d_depthThreshold = writer.declConstant( constants::DepthThreshold
				, c3d_threshold * 0.1_f );
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			castor3d::shader::Utils utils{ writer, renderSystem.isTopDown() };
			utils.declareInvertVec2Y();

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 1u, 3u );
			auto c3d_depthTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_depthTex", 0u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< Vec3 >( "SMAAGatherNeighbours"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & tex )
				{
					writer.returnStmt( textureGather( tex, texcoord + c3d_rtMetrics.xy() * vec2( -0.5_f, -0.5_f ) ).grb() );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "tex" } );

			/**
			 * Depth Edge Detection
			 */
			auto SMAADepthEdgeDetectionPS = writer.implementFunction< Vec2 >( "SMAADepthEdgeDetectionPS"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & depthTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, offset, depthTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_depthThreshold ), delta ) );

					IF( writer, dot( edges, vec2( 1.0_f, 1.0_f ) ) == 0.0_f )
					{
						writer.discard();
					}
					FI;

					writer.returnStmt( edges );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "depthTex" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );
					pxl_fragColour.xy() = SMAADepthEdgeDetectionPS( utils.topDownToBottomUp( vtx_texture ), vtx_offset, c3d_depthTex );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::TextureViewPtr doCreateDepthView( ashes::TextureView const & depthView )
		{
			ashes::ImageViewCreateInfo view{};
			view.format = depthView.getFormat();
			view.viewType = ashes::TextureViewType::e2D;
			auto & texture = depthView.getTexture();
			view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
			return texture.createView( view );
		}
	}

	//*********************************************************************************************

	DepthEdgeDetection::DepthEdgeDetection( castor3d::RenderTarget & renderTarget
		, ashes::TextureView const & depthView
		, SmaaConfig const & config )
		: EdgeDetection{ renderTarget, config }
		, m_depthView{ doCreateDepthView( depthView ) }
		, m_sourceView{ depthView }
	{
		ashes::Extent2D size{ m_depthView->getTexture().getDimensions().width
			, m_depthView->getTexture().getDimensions().height };
		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_pixelShader.shader = doGetEdgeDetectionFP( *renderTarget.getEngine()->getRenderSystem()
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
			, m_sourceView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

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
		ashes::Extent2D size{ m_depthView->getTexture().getDimensions().width
			, m_depthView->getTexture().getDimensions().height };
		auto & device = getCurrentDevice( m_renderSystem );
		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_renderSystem.compileShader( m_vertexShader ) );
		stages[1].module->loadShader( m_renderSystem.compileShader( m_pixelShader ) );

		ashes::DepthStencilState dsstate{ 0u, false, false };
		dsstate.stencilTestEnable = true;
		dsstate.front.passOp = ashes::StencilOp::eReplace;
		dsstate.front.reference = 1u;
		dsstate.back = dsstate.front;
		ashes::DescriptorSetLayoutBindingArray setLayoutBindings;
		createPipeline( size
			, castor::Position{}
			, stages
			, *m_depthView
			, *m_renderPass
			, setLayoutBindings
			, {}
			, dsstate );
	}
}

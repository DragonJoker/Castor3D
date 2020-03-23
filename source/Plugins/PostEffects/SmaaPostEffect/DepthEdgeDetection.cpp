#include "SmaaPostEffect/DepthEdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace smaa
{
	namespace
	{
		std::unique_ptr< ast::Shader > doGetEdgeDetectionFP( castor3d::RenderSystem & renderSystem
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

			castor3d::shader::Utils utils{ writer };
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
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ImageView doCreateDepthView( ashes::ImageView const & depthView )
		{
			ashes::ImageViewCreateInfo view
			{
				0u,
				*depthView.image,
				VK_IMAGE_VIEW_TYPE_2D,
				depthView.getFormat(),
				VkComponentMapping{},
				{ VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u }
			};
			return depthView.image->createView( view );
		}
	}

	//*********************************************************************************************

	DepthEdgeDetection::DepthEdgeDetection( castor3d::RenderTarget & renderTarget
		, ashes::ImageView const & depthView
		, SmaaConfig const & config )
		: EdgeDetection{ renderTarget, config }
		, m_depthView{ doCreateDepthView( depthView ) }
		, m_sourceView{ depthView }
	{
		VkExtent2D size{ m_depthView.image->getDimensions().width
			, m_depthView.image->getDimensions().height };
		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_pixelShader.shader = doGetEdgeDetectionFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		doInitialisePipeline();
	}

	castor3d::CommandsSemaphore DepthEdgeDetection::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		castor3d::CommandsSemaphore edgeDetectionCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & edgeDetectionCmd = *edgeDetectionCommands.commandBuffer;

		edgeDetectionCmd.begin();
		edgeDetectionCmd.beginDebugBlock(
			{
				"SMAA DepthEdgeDetection",
				{
					castor3d::transparentBlackClearColor.color.float32[0],
					castor3d::transparentBlackClearColor.color.float32[1],
					castor3d::transparentBlackClearColor.color.float32[2],
					castor3d::transparentBlackClearColor.color.float32[3],
				},
			} );
		timer.beginPass( edgeDetectionCmd, passIndex );
		// Put source image in shader input layout.
		edgeDetectionCmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_sourceView.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

		edgeDetectionCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, {
				castor3d::transparentBlackClearColor,
				castor3d::defaultClearDepthStencil,
			}
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( edgeDetectionCmd );
		edgeDetectionCmd.endRenderPass();
		timer.endPass( edgeDetectionCmd, passIndex );
		edgeDetectionCmd.endDebugBlock();
		edgeDetectionCmd.end();

		return std::move( edgeDetectionCommands );
	}

	void DepthEdgeDetection::doInitialisePipeline()
	{
		VkExtent2D size{ m_depthView.image->getDimensions().width
			, m_depthView.image->getDimensions().height };
		auto & device = getCurrentRenderDevice( m_renderSystem );
		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_vertexShader ) );
		stages.push_back( makeShaderState( device, m_pixelShader ) );

		ashes::PipelineDepthStencilStateCreateInfo dsstate{ 0u, VK_FALSE, VK_FALSE };
		dsstate->stencilTestEnable = true;
		dsstate->front.passOp = VK_STENCIL_OP_REPLACE;
		dsstate->front.reference = 1u;
		dsstate->back = dsstate->front;
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		createPipeline( size
			, castor::Position{}
			, stages
			, m_depthView
			, *m_renderPass
			, std::move( setLayoutBindings )
			, {}
			, std::move( dsstate ) );
	}
}

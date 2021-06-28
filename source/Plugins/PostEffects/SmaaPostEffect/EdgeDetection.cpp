#include "SmaaPostEffect/EdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
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
		std::unique_ptr< ast::Shader > doGetEdgeDetectionVP( castor::Size const & size
			, SmaaConfig const & config )
		{
			Point4f renderTargetMetrics{ 1.0f / size.getWidth()
				, 1.0f / size.getHeight()
				, float( size.getWidth() )
				, float( size.getHeight() ) };
			using namespace sdw;
			VertexWriter writer;

			// Shader constants
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( "vtx_offset", 1u, 3u );
			auto out = writer.getOut();

			/**
			 * Edge Detection Vertex Shader
			 */
			auto SMAAEdgeDetectionVS = writer.implementFunction< sdw::Void >( "SMAAEdgeDetectionVS"
				, [&]( Vec2 const & texCoord
					, Array< Vec4 > offset )
				{
					offset[0] = fma( c3d_rtMetrics.xyxy(), vec4( Float{ -1.0f }, 0.0_f, 0.0_f, Float{ -1.0f } ), vec4( texCoord.xy(), texCoord.xy() ) );
					offset[1] = fma( c3d_rtMetrics.xyxy(), vec4( 1.0_f, 0.0_f, 0.0_f, 1.0_f ), vec4( texCoord.xy(), texCoord.xy() ) );
					offset[2] = fma( c3d_rtMetrics.xyxy(), vec4( Float{ -2.0f }, 0.0_f, 0.0_f, Float{ -2.0f } ), vec4( texCoord.xy(), texCoord.xy() ) );
				}
				, InVec2{ writer, "texCoord" }
				, OutVec4Array{ writer, "offset", 3u } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
					vtx_offset[0] = vec4( 0.0_f );
					vtx_offset[1] = vec4( 0.0_f );
					vtx_offset[2] = vec4( 0.0_f );
					SMAAEdgeDetectionVS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	EdgeDetection::EdgeDetection( crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaConfig const & config
		, std::unique_ptr< ast::Shader > pixelShader )
		: m_device{ device }
		, m_graph{ renderTarget.getGraph() }
		, m_config{ config }
		, m_outColour{ m_device
			, m_graph.getHandler()
			, "SMEDRes"
			, 0u
			, castor3d::makeExtent3D( renderTarget.getSize() )
			, 1u
			, 1u
			, VK_FORMAT_R8G8B8A8_UNORM
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) }
		, m_outDepth{ m_device
			, m_graph.getHandler()
			, "SMEDStRes"
			, 0u
			, castor3d::makeExtent3D( renderTarget.getSize() )
			, 1u
			, 1u
			, device.selectSuitableStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ) }
		, m_outDepthStencilView{ m_graph.createView( crg::ImageViewData{ "SMEDStRes"
			, m_outDepth.imageId
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_outDepth.imageId.data->info.format
			, { VK_IMAGE_ASPECT_STENCIL_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SmaaEdge", doGetEdgeDetectionVP( renderTarget.getSize(), config ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaEdge", std::move( pixelShader ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_pass{ m_graph.createPass( "SmaaEdge"
			, [this, &device, &renderTarget]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				ashes::PipelineDepthStencilStateCreateInfo dsState{ 0u, VK_FALSE, VK_FALSE };
				dsState->stencilTestEnable = VK_TRUE;
				dsState->front.passOp = VK_STENCIL_OP_REPLACE;
				dsState->front.reference = 1u;
				dsState->back = dsState->front;
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( renderTarget.getSize() ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.depthStencilState( dsState )
					.build( pass, context, graph );
				device.renderSystem.getEngine()->registerTimer( "SMAA"
					, result->getTimer() );
				return result;
			} ) }
	{
		m_pass.addDependency( previousPass );
		m_pass.addOutputStencilView( m_outDepthStencilView
			, castor3d::defaultClearDepthStencil );
		m_pass.addOutputColourView( m_outColour.targetViewId
			, castor3d::transparentBlackClearColor );
		m_outColour.create();
		m_outDepth.create();
	}

	void EdgeDetection::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( "SMAA EdgeDetection Colour Result"
			, m_outColour
			, m_graph.getFinalLayout( m_outColour.sampledViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}
}

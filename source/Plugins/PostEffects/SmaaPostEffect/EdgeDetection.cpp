#include "SmaaPostEffect/EdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <numeric>

namespace smaa
{
	EdgeDetection::EdgeDetection( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, SmaaConfig const & config
		, castor3d::ShaderPtr shader
		, bool const * enabled
		, uint32_t const * passIndex
		, uint32_t passCount )
		: m_device{ device }
		, m_graph{ graph }
		, m_config{ config }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_outColour{ m_device
			, renderTarget.getResources()
			, cuT( "SMEDRes" )
			, 0u
			, m_extent
			, 1u
			, 1u
			, VK_FORMAT_R8G8B8A8_UNORM
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) }
		, m_outDepth{ m_device
			, renderTarget.getResources()
			, cuT( "SMEDStRes" )
			, 0u
			, m_extent
			, 1u
			, 1u
			, device.selectSuitableStencilFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
				| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ) }
		, m_outDepthStencilView{ m_graph.createView( crg::ImageViewData{ "SMEDStRes"
			, m_outDepth.imageId
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_outDepth.imageId.data->info.format
			, { VK_IMAGE_ASPECT_STENCIL_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_shader{ cuT( "SmaaEdge" ), castor::move( shader ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_pass{ m_graph.createPass( "EdgeDetection"
			, [this, &device, passIndex, enabled, passCount]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				ashes::PipelineDepthStencilStateCreateInfo dsState{ 0u, VK_FALSE, VK_FALSE };
				dsState->stencilTestEnable = VK_TRUE;
				dsState->front.passOp = VK_STENCIL_OP_REPLACE;
				dsState->front.reference = 1u;
				dsState->back = dsState->front;
				auto builder = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.depthStencilState( dsState )
					.enabled( enabled );

				if ( passIndex )
				{
					builder.passIndex( passIndex );
				}

				auto result = builder.build( framePass
					, context
					, graph
					, crg::ru::Config{ passCount } );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} ) }
	{
		m_pass.addDependency( previousPass );
		ubo.createPassBinding( m_pass
			, SmaaUboIdx );
		m_pass.addOutputStencilView( m_outDepthStencilView
			, castor3d::defaultClearDepthStencil );
		m_pass.addOutputColourView( m_outColour.targetViewId
			, castor3d::transparentBlackClearColor );
		m_outColour.create();
		m_outDepth.create();
	}

	EdgeDetection::~EdgeDetection()
	{
		m_outColour.destroy();
		m_outDepth.destroy();
	}

	void EdgeDetection::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
		visitor.visit( cuT( "SMAA EdgeDetection Colour Result" )
			, m_outColour
			, m_graph.getFinalLayoutState( m_outColour.sampledViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}

	void EdgeDetection::getVertexProgram( sdw::TraditionalGraphicsWriter & writer
		, SmaaData const & smaaData )
	{
		namespace c3d = castor3d::shader;

		auto SMAAEdgeDetectionVS = writer.implementFunction< sdw::Void >( "SMAAEdgeDetectionVS"
			, [&]( sdw::Vec2 const & texCoord
				, sdw::Vec4Array offset )
			{
				offset[0] = fma( smaaData.rtMetrics.xyxy(), vec4( sdw::Float{ -1.0f }, 0.0_f, 0.0_f, sdw::Float{ -1.0f } ), vec4( texCoord.xy(), texCoord.xy() ) );
				offset[1] = fma( smaaData.rtMetrics.xyxy(), vec4( 1.0_f, 0.0_f, 0.0_f, 1.0_f ), vec4( texCoord.xy(), texCoord.xy() ) );
				offset[2] = fma( smaaData.rtMetrics.xyxy(), vec4( sdw::Float{ -2.0f }, 0.0_f, 0.0_f, sdw::Float{ -2.0f } ), vec4( texCoord.xy(), texCoord.xy() ) );
			}
			, sdw::InVec2{ writer, "texCoord" }
			, sdw::OutVec4Array{ writer, "offset", 3u } );

		writer.implementEntryPointT< c3d::PosUv2FT, EDVertexT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
			, sdw::VertexOutT< EDVertexT > out )
			{
				out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				out.texcoord() = in.uv();
				out.offset()[0] = vec4( 0.0_f );
				out.offset()[1] = vec4( 0.0_f );
				out.offset()[2] = vec4( 0.0_f );
				SMAAEdgeDetectionVS( out.texcoord(), out.offset() );
			} );
	}
}

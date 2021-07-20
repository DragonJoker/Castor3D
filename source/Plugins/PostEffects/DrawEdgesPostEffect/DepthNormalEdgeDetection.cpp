#include "DrawEdgesPostEffect/DepthNormalEdgeDetection.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/PassBuffer/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace draw_edges
{
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexShader( VkExtent3D const & size )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getFragmentProgram( VkExtent3D const & extent )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			castor3d::shader::Materials materials{ writer };
			materials.declare( true, DepthNormalEdgeDetection::eMaterials, 0u );
			auto data0( writer.declSampledImage< FImg2DRgba32 >( "data0", DepthNormalEdgeDetection::eData0, 0u ) );
			auto data1( writer.declSampledImage< FImg2DRgba32 >( "data1", DepthNormalEdgeDetection::eData1, 0u ) );
			auto depthRangeBuffer( writer.declShaderStorageBuffer( "DepthRangeBuffer", DepthNormalEdgeDetection::eDepthRange, 0u ) );
			auto minmax = depthRangeBuffer.declMember< sdw::Int >( "minmax", 2u );
			depthRangeBuffer.end();
			auto in = writer.getIn();

			// Outputs
			auto output( writer.declOutput< sdw::Float >( "output", 0u ) );

			auto Fdepth = writer.implementFunction< sdw::Float >( "Fdepth"
				, [&]( sdw::Float const & z
					, sdw::Float const & zNear
					, sdw::Float const & zFar )
				{
					writer.returnStmt( abs( ( 1.0_f / z - 1.0_f / zNear ) / ( ( 1.0_f / zFar ) - ( 1.0_f / zNear ) ) ) );
				}
				, sdw::InFloat{ writer, "z" }
				, sdw::InFloat{ writer, "zNear" }
				, sdw::InFloat{ writer, "zFar" } );

			auto computeContour = writer.implementFunction< sdw::Float >( "c3d_computeContour"
				, [&]( sdw::IVec2 const & texCoord
					, sdw::Vec4 const & X
					, sdw::Vec3 const & Xn
					, sdw::Vec2 const & depthRange
					, sdw::Float const & edgeWidth
					, sdw::Float const & depthFactor
					, sdw::Float const & normalFactor )
				{
					auto w = writer.declLocale( "w"
						, writer.cast< sdw::Int >( edgeWidth ) );
					auto h = writer.declLocale( "h"
						, writer.cast< sdw::Int >( edgeWidth ) );

					auto An = writer.declLocale( "An", data1.fetch( texCoord, 0_i, ivec2(   -w,   +h ) ).xyz() );  //  +---+---+---+
					auto Bn = writer.declLocale( "Bn", data1.fetch( texCoord, 0_i, ivec2( +0_i,   +h ) ).xyz() );  //  | A | B | C |
					auto Cn = writer.declLocale( "Cn", data1.fetch( texCoord, 0_i, ivec2(   +w,   +h ) ).xyz() );  //  +---+---+---+
					auto Dn = writer.declLocale( "Dn", data1.fetch( texCoord, 0_i, ivec2(   -w, +0_i ) ).xyz() );  //  | D | X | E |
					auto En = writer.declLocale( "En", data1.fetch( texCoord, 0_i, ivec2(   +w, +0_i ) ).xyz() );  //  | F | G | H |
					auto Fn = writer.declLocale( "Fn", data1.fetch( texCoord, 0_i, ivec2(   -w,   -h ) ).xyz() );  //  +---+---+---+
					auto Gn = writer.declLocale( "Gn", data1.fetch( texCoord, 0_i, ivec2( +0_i,   -h ) ).xyz() );
					auto Hn = writer.declLocale( "Hn", data1.fetch( texCoord, 0_i, ivec2(   +w,   -h ) ).xyz() );

					// Normal Gradient
					auto Ngrad = writer.declLocale( "Ngrad", 0.0_f );
					// compute length of gradient using Sobel/Kroon operator
					const float k0 = 17. / 23.75;
					const float k1 = 61. / 23.75;
					auto grad_y = writer.declLocale( "grad_y"
						, k0 * An + k1 * Bn + k0 * Cn - k0 * Fn - k1 * Gn - k0 * Hn );
					auto grad_x = writer.declLocale( "grad_x"
						, k0 * Cn + k1 * En + k0 * Hn - k0 * An - k1 * Dn - k0 * Fn );
					auto g = writer.declLocale( "g"
						, length( grad_x ) + length( grad_y ) );

					Ngrad = smoothStep( 2.0_f, 3.0_f, g * normalFactor );  //!! magic

					// Depth Gradient
					auto Dgrad = writer.declLocale( "Dgrad", 0.0_f );
					auto zNear = writer.declLocale( "zNear", depthRange.x() );
					auto zFar = writer.declLocale( "zFar", depthRange.y() );
					auto Ad = writer.declLocale( "Ad", Fdepth( data0.fetch( texCoord, 0_i, ivec2(   -w,   +h ) ).y(), zNear, zFar ) );  //  +---+---+---+
					auto Bd = writer.declLocale( "Bd", Fdepth( data0.fetch( texCoord, 0_i, ivec2( +0_i,   +h ) ).y(), zNear, zFar ) );  //  | A | B | C |
					auto Cd = writer.declLocale( "Cd", Fdepth( data0.fetch( texCoord, 0_i, ivec2(   +w,   +h ) ).y(), zNear, zFar ) );  //  +---+---+---+
					auto Dd = writer.declLocale( "Dd", Fdepth( data0.fetch( texCoord, 0_i, ivec2(   -w, +0_i ) ).y(), zNear, zFar ) );  //  | D | X | E |
					auto Xd = writer.declLocale( "Xd", Fdepth( X.y(), zNear, zFar ) );                                                  //  | F | G | H |
					auto Ed = writer.declLocale( "Ed", Fdepth( data0.fetch( texCoord, 0_i, ivec2(   +w, +0_i ) ).y(), zNear, zFar ) );  //  +---+---+---+
					auto Fd = writer.declLocale( "Fd", Fdepth( data0.fetch( texCoord, 0_i, ivec2(   -w,   -h ) ).y(), zNear, zFar ) );  
					auto Gd = writer.declLocale( "Gd", Fdepth( data0.fetch( texCoord, 0_i, ivec2( +0_i,   -h ) ).y(), zNear, zFar ) );
					auto Hd = writer.declLocale( "Hd", Fdepth( data0.fetch( texCoord, 0_i, ivec2(   +w,   -h ) ).y(), zNear, zFar ) );

					g = ( abs( Ad + 2 * Bd + Cd - Fd - 2 * Gd - Hd ) + abs( Cd + 2 * Ed + Hd - Ad - 2 * Dd - Fd ) ) / 8.0;
					auto l = writer.declLocale( "l"
						, ( 8 * Xd - Ad - Bd - Cd - Dd - Ed - Fd - Gd - Hd ) / 3.0 );

					Dgrad = ( l + g ) * depthFactor;
					Dgrad = smoothStep( 0.03_f, 0.1_f, Dgrad );

					writer.returnStmt( Ngrad + Dgrad );
				}
				, sdw::InIVec2{ writer, "texCoord" }
				, sdw::InVec4{ writer, "X" }
				, sdw::InVec3{ writer, "Xn" }
				, sdw::InVec2{ writer, "depthRange" }
				, sdw::InFloat{ writer, "edgeWidth" }
				, sdw::InFloat{ writer, "depthFactor" }
				, sdw::InFloat{ writer, "normalFactor" } );

			writer.implementMain( [&]()
				{
					auto size = writer.declLocale( "size"
						, ivec2( sdw::Int{ int( extent.width ) }, sdw::Int{ int( extent.height ) } ) );
					auto texelCoord = writer.declLocale( "texelCoord"
						, ivec2( vec2( size ) * vtx_texture ) );

					auto X = writer.declLocale( "X"
						, data0.fetch( texelCoord, 0_i ) );

					IF( writer, X.w() == 0.0_f )
					{
						writer.discard();
					}
					FI;

					auto material = writer.declLocale( "material"
						, materials.getMaterial( writer.cast< sdw::UInt >( X.w() ) ) );

					IF( writer, material.edgeColour.w() == 0.0_f )
					{
						writer.discard();
					}
					FI;

					auto Xn = writer.declLocale( "Xn"
						, data1.fetch( texelCoord, 0_i ) );
					auto depthRange = writer.declLocale( "depthRange"
						, vec2( intBitsToFloat( minmax[0] )
							, intBitsToFloat( minmax[1] ) ) );

					output = computeContour( texelCoord
						, X
						, Xn.xyz()
						, depthRange
						, material.edgeWidth
						, material.depthFactor
						, material.normalFactor );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	DepthNormalEdgeDetection::DepthNormalEdgeDetection( crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, castor3d::PassBuffer const & passBuffer
		, crg::ImageViewId const & data0
		, crg::ImageViewId const & data1
		, ashes::Buffer< int32_t > const & depthRange )
		: m_device{ device }
		, m_graph{ renderTarget.getGraph() }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_result{ m_device
			, m_graph.getHandler()
			, "DNEdges"
			, 0u
			, m_extent
			, 1u
			, 1u
			, VK_FORMAT_R16_SFLOAT
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "DNEdgesDetection", getVertexShader( m_extent ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "DNEdgesDetection", getFragmentProgram( m_extent ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_pass{ m_graph.createPass( "DNEdgesDetection"
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
					.renderSize( castor3d::makeExtent2D( m_extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.depthStencilState( dsState )
					.build( pass, context, graph );
				device.renderSystem.getEngine()->registerTimer( "Draw Edges"
					, result->getTimer() );
				return result;
			} ) }
	{
		m_pass.addDependency( previousPass );
		passBuffer.createPassBinding( m_pass, eMaterials );
		m_pass.addSampledView( data0, eData0, VK_IMAGE_LAYOUT_UNDEFINED );
		m_pass.addSampledView( data1, eData1, VK_IMAGE_LAYOUT_UNDEFINED );
		m_pass.addInputStorageBuffer( { depthRange.getBuffer(), "DepthRange" }, eDepthRange, 0u, depthRange.getBuffer().getSize() );
		m_pass.addOutputColourView( m_result.targetViewId
			, castor3d::transparentBlackClearColor );
		m_result.create();
	}

	void DepthNormalEdgeDetection::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( "Depth Normal Edge Detection Result"
			, m_result
			, m_graph.getFinalLayout( m_result.sampledViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}
}

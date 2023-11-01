#include "SmaaPostEffect/ColourEdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

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
	namespace coled
	{
		namespace c3d = castor3d::shader;

		enum Idx : uint32_t
		{
			ColorTexIdx = SmaaUboIdx + 1,
			PredicationTexIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device
			, bool predication )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			// Shader inputs
			C3D_Smaa( writer, SmaaUboIdx, 0u );
			auto c3d_colourTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_colourTex", ColorTexIdx, 0u );
			auto c3d_predicationTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_predicationTex", PredicationTexIdx, 0u, predication );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< sdw::Vec3 >( "SMAAGatherNeighbours"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec4Array const & offset
					, sdw::CombinedImage2DRgba32 const & predicationTex )
				{
					writer.returnStmt( predicationTex.gather( texcoord + c3d_smaaData.rtMetrics.xy() * vec2( -0.5_f, -0.5_f ), 0_i ).grb() );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec4Array{ writer, "offset", 3u }
				, sdw::InCombinedImage2DRgba32{ writer, "predicationTex" } );

			/**
			 * Adjusts the threshold by means of predication.
			 */
			auto SMAACalculatePredicatedThreshold = writer.implementFunction< sdw::Vec2 >( "SMAACalculatePredicatedThreshold"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec4Array const & offset
					, sdw::CombinedImage2DRgba32 const & predicationTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, offset, predicationTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_smaaData.predicationThreshold ), delta ) );
					writer.returnStmt( c3d_smaaData.predicationScale * c3d_smaaData.threshold * ( 1.0_f - c3d_smaaData.predicationStrength * edges ) );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec4Array{ writer, "offset", 3u }
				, sdw::InCombinedImage2DRgba32{ writer, "predicationTex" } );

			/**
			 * Color Edge Detection
			 *
			 * IMPORTANT NOTICE: color edge detection requires gamma-corrected colors, and
			 * thus 'colorTex' should be a non-sRGB texture.
			 */
			auto SMAAColorEdgeDetectionPS = writer.implementFunction< sdw::Vec2 >( "SMAAColorEdgeDetectionPS"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec4Array const & offset )
				{
					// Calculate the threshold:
					auto threshold = writer.declLocale( "threshold"
						, ( predication
							? SMAACalculatePredicatedThreshold( texcoord, offset, c3d_predicationTex )
							: vec2( c3d_smaaData.threshold, c3d_smaaData.threshold ) ) );

					// Calculate color deltas:
					auto delta = writer.declLocale< sdw::Vec4 >( "delta" );
					auto C = writer.declLocale( "C"
						, c3d_colourTex.sample( texcoord ).rgb() );

					auto Cleft = writer.declLocale( "Cleft"
						, c3d_colourTex.sample( offset[0].xy() ).rgb() );
					auto t = writer.declLocale( "t"
						, abs( C - Cleft ) );
					delta.x() = max( max( t.r(), t.g() ), t.b() );

					auto Ctop = writer.declLocale( "Ctop"
						, c3d_colourTex.sample( offset[0].zw() ).rgb() );
					t = abs( C - Ctop );
					delta.y() = max( max( t.r(), t.g() ), t.b() );

					// We do the usual threshold:
					auto edges = writer.declLocale( "edges"
						, step( threshold, delta.xy() ) );

					// Then discard if there is no edge:
					IF( writer, dot( edges, vec2( 1.0_f, 1.0_f ) ) == 0.0_f )
					{
						writer.demote();
					}
					FI;

					// Calculate right and bottom deltas:
					auto Cright = writer.declLocale( "Cright"
						, c3d_colourTex.sample( offset[1].xy() ).rgb() );
					t = abs( C - Cright );
					delta.z() = max( max( t.r(), t.g() ), t.b() );

					auto Cbottom = writer.declLocale( "Cbottom"
						, c3d_colourTex.sample( offset[1].zw() ).rgb() );
					t = abs( C - Cbottom );
					delta.w() = max( max( t.r(), t.g() ), t.b() );

					// Calculate the maximum delta in the direct neighborhood:
					auto maxDelta = writer.declLocale( "maxDelta"
						, max( delta.xy(), delta.zw() ) );

					// Calculate left-left and top-top deltas:
					auto Cleftleft = writer.declLocale( "Cleftleft"
						, c3d_colourTex.sample( offset[2].xy() ).rgb() );
					t = abs( C - Cleftleft );
					delta.z() = max( max( t.r(), t.g() ), t.b() );

					auto Ctoptop = writer.declLocale( "Ctoptop"
						, c3d_colourTex.sample( offset[2].zw() ).rgb() );
					t = abs( C - Ctoptop );
					delta.w() = max( max( t.r(), t.g() ), t.b() );

					// Calculate the final maximum delta:
					maxDelta = max( maxDelta.xy(), delta.zw() );
					auto finalDelta = writer.declLocale( "finalDelta"
						, max( maxDelta.x(), maxDelta.y() ) );

					// Local contrast adaptation:
					edges.xy() *= step( vec2( finalDelta ), c3d_smaaData.localContrastAdaptationFactor * delta.xy() );

					writer.returnStmt( edges );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec4Array{ writer, "offset", 3u } );

			EdgeDetection::getVertexProgram( writer, c3d_smaaData );

			writer.implementEntryPointT< EDVertexT, c3d::Colour4FT >( [&]( sdw::FragmentInT< EDVertexT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					out.colour() = vec4( 0.0_f );
					out.colour().xy() = SMAAColorEdgeDetectionPS( in.texcoord(), in.offset() );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static crg::ImageViewData doCreatePredicationView( crg::ImageViewId const & pred )
		{
			return crg::ImageViewData{ "SMCEDPred"
				, pred.data->image
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, pred.data->info.format
				, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } };
		}
	}

	//*********************************************************************************************

	ColourEdgeDetection::ColourEdgeDetection( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, crg::ImageViewIdArray const & colourView
		, crg::ImageViewId const * predication
		, SmaaConfig const & config
		, bool const * enabled
		, uint32_t const * passIndex )
		: EdgeDetection{ graph
			, previousPass
			, renderTarget
			, device
			, ubo
			, config
			, coled::getProgram( device, predication != nullptr )
			, enabled
			, passIndex
			, uint32_t( colourView.size() ) }
		, m_predicationView{ ( predication
			? m_graph.createView( coled::doCreatePredicationView( *predication ) )
			: crg::ImageViewId{} ) }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addSampledView( colourView
			, coled::ColorTexIdx
			, linearSampler );

		if ( predication )
		{
			m_pass.addSampledView( m_predicationView
				, coled::PredicationTexIdx
				, linearSampler );
		}
	}
}

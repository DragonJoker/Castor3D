#include "SmaaPostEffect/ColourEdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/makeVkType.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

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
		enum Idx : uint32_t
		{
			ColorTexIdx,
			PredicationTexIdx,
		};

		std::unique_ptr< ast::Shader > doGetEdgeDetectionFPPredication( castor::Size const & size
			, SmaaConfig const & config )
		{
			auto renderTargetMetrics = Point4f{ 1.0f / size.getWidth()
				, 1.0f / size.getHeight()
				, float( size.getWidth() )
				, float( size.getHeight() ) };
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			auto c3d_localContrastAdaptationFactor = writer.declConstant( constants::LocalContrastAdaptationFactor
				, Float( config.data.localContrastAdaptationFactor ) );
			auto c3d_predicationThreshold = writer.declConstant( constants::PredicationThreshold
				, Float( config.data.predicationThreshold ) );
			auto c3d_predicationScale = writer.declConstant( constants::PredicationScale
				, Float( config.data.predicationScale ) );
			auto c3d_predicationStrength = writer.declConstant( constants::PredicationStrength
				, Float( config.data.predicationStrength ) );
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 1u, 3u );
			auto c3d_colourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_colourTex", ColorTexIdx, 0u );
			auto c3d_predicationTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_predicationTex", PredicationTexIdx, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< Vec3 >( "SMAAGatherNeighbours"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & predicationTex )
				{
					writer.returnStmt( predicationTex.gather( texcoord + c3d_rtMetrics.xy() * vec2( -0.5_f, -0.5_f ) ).grb() );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "predicationTex" } );

			/**
			 * Adjusts the threshold by means of predication.
			 */
			auto SMAACalculatePredicatedThreshold = writer.implementFunction< Vec2 >( "SMAACalculatePredicatedThreshold"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & predicationTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, offset, predicationTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_predicationThreshold ), delta ) );
					writer.returnStmt( c3d_predicationScale * c3d_threshold * ( 1.0_f - c3d_predicationStrength * edges ) );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "predicationTex" } );

			/**
			 * Color Edge Detection
			 *
			 * IMPORTANT NOTICE: color edge detection requires gamma-corrected colors, and
			 * thus 'colorTex' should be a non-sRGB texture.
			 */
			auto SMAAColorEdgeDetectionPS = writer.implementFunction< Vec2 >( "SMAAColorEdgeDetectionPS"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset )
				{
					// Calculate the threshold:
					auto threshold = writer.declLocale< Vec2 >( "threshold" );
					threshold = SMAACalculatePredicatedThreshold( texcoord, offset, c3d_predicationTex );

					// Calculate color deltas:
					auto delta = writer.declLocale< Vec4 >( "delta" );
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
						writer.discard();
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
					edges.xy() *= step( vec2( finalDelta ), c3d_localContrastAdaptationFactor * delta.xy() );

					writer.returnStmt( edges );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );
					pxl_fragColour.xy() = SMAAColorEdgeDetectionPS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > doGetEdgeDetectionFPNoPredication( castor::Size const & size
			, SmaaConfig const & config )
		{
			auto renderTargetMetrics = Point4f{ 1.0f / size.getWidth()
				, 1.0f / size.getHeight()
				, float( size.getWidth() )
				, float( size.getHeight() ) };
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			auto c3d_localContrastAdaptationFactor = writer.declConstant( constants::LocalContrastAdaptationFactor
				, Float( config.data.localContrastAdaptationFactor ) );
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 1u, 3u );
			auto c3d_colourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_colourTex", 0u, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			/**
			 * Color Edge Detection
			 *
			 * IMPORTANT NOTICE: color edge detection requires gamma-corrected colors, and
			 * thus 'colorTex' should be a non-sRGB texture.
			 */
			auto SMAAColorEdgeDetectionPS = writer.implementFunction< Vec2 >( "SMAAColorEdgeDetectionPS"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset )
				{
					// Calculate the threshold:
					auto threshold = writer.declLocale< Vec2 >( "threshold"
						, vec2( c3d_threshold, c3d_threshold ) );

					// Calculate color deltas:
					auto delta = writer.declLocale< Vec4 >( "delta" );
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
						writer.discard();
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
					edges.xy() *= step( vec2( finalDelta ), c3d_localContrastAdaptationFactor * delta.xy() );

					writer.returnStmt( edges );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );
					pxl_fragColour.xy() = SMAAColorEdgeDetectionPS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		crg::ImageViewData doCreatePredicationView( crg::ImageViewId const & pred )
		{
			return crg::ImageViewData{ "SMCEDPred"
				, pred.data->image
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, pred.data->info.format
				, { VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u } };
		}
	}

	//*********************************************************************************************

	ColourEdgeDetection::ColourEdgeDetection( crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & colourView
		, crg::ImageViewId const * predication
		, SmaaConfig const & config )
		: EdgeDetection{ previousPass
			, renderTarget
			, device
			, config
			, ( predication
				? doGetEdgeDetectionFPPredication( renderTarget.getSize(), config )
				: doGetEdgeDetectionFPNoPredication( renderTarget.getSize(), config ) ) }
		, m_colourView{ colourView }
		, m_predicationView{ ( predication
			? renderTarget.getGraph().createView( doCreatePredicationView( *predication ) )
			: crg::ImageViewId{} ) }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addSampledView( m_colourView
			, ColorTexIdx
			, {}
			, linearSampler );

		if ( predication )
		{
			m_pass.addSampledView( m_predicationView
				, PredicationTexIdx
				, {}
				, linearSampler );
		}
	}
}

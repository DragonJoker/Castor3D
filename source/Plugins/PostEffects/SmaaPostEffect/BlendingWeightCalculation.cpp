#include "SmaaPostEffect/BlendingWeightCalculation.hpp"

#include "SmaaPostEffect/AreaTex.h"
#include "SmaaPostEffect/SearchTex.h"
#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>
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
		std::unique_ptr< ast::Shader > doBlendingWeightCalculationVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader constants
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );
			auto c3d_maxSearchSteps = writer.declConstant( constants::MaxSearchSteps
				, Int( config.data.maxSearchSteps ) );

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_pixcoord = writer.declOutput< Vec2 >( "vtx_pixcoord", 1u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( "vtx_offset", 2u, 3u );
			auto out = writer.getOut();

			/**
			 * Blend Weight Calculation Vertex Shader
			 */
			auto SMAABlendingWeightCalculationVS = writer.implementFunction< sdw::Void >( "SMAABlendingWeightCalculationVS"
				, [&]( Vec2 const & texCoord
					, Vec2 pixcoord
					, Array< Vec4 > offset )
				{
					pixcoord = texCoord * c3d_rtMetrics.zw();

					// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
					offset[0] = fma( c3d_rtMetrics.xyxy(), vec4( -0.25_f, -0.125f, 1.25f, -0.125f ), vec4( texCoord.xy(), texCoord.xy() ) );
					offset[1] = fma( c3d_rtMetrics.xyxy(), vec4( -0.125_f, -0.25f, -0.125f, 1.25f ), vec4( texCoord.xy(), texCoord.xy() ) );

					// And these for the searches, they indicate the ends of the loops:
					offset[2] = fma( c3d_rtMetrics.xxyy()
						, vec4( -2.0_f, 2.0_f, -2.0_f, 2.0_f ) * writer.cast< Float >( c3d_maxSearchSteps )
						, vec4( offset[0].xz(), offset[1].yw() ) );
				}
				, InVec2{ writer, "texCoord" }
				, OutVec2{ writer, "pixcoord" }
				, OutVec4Array{ writer, "offset", 3u } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
					SMAABlendingWeightCalculationVS( vtx_texture, vtx_pixcoord, vtx_offset );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > doBlendingWeightCalculationFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader constants
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );
			auto c3d_areaTexMaxDistance = writer.declConstant( constants::AreaTexMaxDistance
				, Float( 16.0f ) );
			auto c3d_areaTexMaxDistanceDiag = writer.declConstant( constants::AreaTexMaxDistanceDiag
				, Float( 20.0f ) );
			auto c3d_areaTexPixelSize = writer.declConstant( constants::AreaTexPixelSize
				, vec2( 1.0_f ) / vec2( 160.0_f, 560.0_f ) );
			auto c3d_areaTexSubtexSize = writer.declConstant( constants::AreaTexSubtexSize
				, 1.0_f / 7.0_f );
			auto c3d_searchTexSize = writer.declConstant( constants::SearchTexSize
				, vec2( 66.0_f, 33.0_f ) );
			auto c3d_searchTexPackedSize = writer.declConstant( constants::SearchTexPackedSize
				, vec2( 64.0_f, 16.0_f ) );
			auto c3d_maxSearchStepsDiag = writer.declConstant( constants::MaxSearchStepsDiag
				, Int( config.data.maxSearchStepsDiag ) );
			auto c3d_cornerRounding = writer.declConstant( constants::CornerRounding
				, Int( config.data.cornerRounding ) );
			auto c3d_cornerRoundingNorm = writer.declConstant( constants::CornerRoundingNorm
				, writer.cast< Float >( c3d_cornerRounding ) / 100.0_f );

			// Shader inputs
			Ubo ubo{ writer, cuT( "Subsample" ), 0u, 0u };
			auto c3d_subsampleIndices = ubo.declMember< Vec4 >( constants::SubsampleIndices );
			ubo.end();
			auto c3d_areaTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_areaTex", 1u, 0u );
			auto c3d_searchTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_searchTex", 2u, 0u );
			auto c3d_edgesTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_edgesTex", 3u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_pixcoord = writer.declInput< Vec2 >( "vtx_pixcoord", 1u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 2u, 3u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			/**
			 * Conditional move:
			 */
			auto SMAAMovc = writer.implementFunction< sdw::Void >( "SMAAMovc"
				, [&]( BVec2 const & cond
					, Vec2 variable
					, Vec2 const & value )
				{
					IF( writer, cond.x() )
					{
						variable.x() = value.x();
					}
					FI;

					IF( writer, cond.y() )
					{
						variable.y() = value.y();
					}
					FI;
				}
				, InBVec2{ writer, "cond" }
				, InOutVec2{ writer, "variable" }
				, InVec2{ writer, "value" } );

			/**
			 * Allows to decode two binary values from a bilinear-filtered access.
			 */
			auto SMAADecodeDiagBilinearAccess2 = writer.implementFunction< Vec2 >( "SMAADecodeDiagBilinearAccess2"
				, [&]( Vec2 const & e )
				{
					// Bilinear access for fetching 'e' have a 0.25 offset, and we are
					// interested in the R and G edges:
					//
					// +---G---+-------+
					// |   x o R   x   |
					// +-------+-------+
					//
					// Then, if one of these edge is enabled:
					//   Red:   (0.75 * X + 0.25 * 1) => 0.25 or 1.0
					//   Green: (0.75 * 1 + 0.25 * X) => 0.75 or 1.0
					//
					// This function will unpack the values (mad + mul + round):
					// wolframalpha.com: round(x * abs(5 * x - 5 * 0.75)) plot 0 to 1
					e.r() = e.r() * abs( 5.0_f * e.r() - 5.0_f * 0.75_f );
					writer.returnStmt( round( e ) );
				}
				, InVec2{ writer, "e" } );

			auto SMAADecodeDiagBilinearAccess4 = writer.implementFunction< Vec4 >( "SMAADecodeDiagBilinearAccess4"
				, [&]( Vec4 const & e )
				{
					e.rb() = e.rb() * abs( 5.0_f * e.rb() - vec2( 5.0_f * 0.75_f ) );
					writer.returnStmt( round( e ) );
				}
				, InVec4{ writer, "e" } );

			/**
			 * These functions allows to perform diagonal pattern searches.
			 */
			auto SMAASearchDiag1 = writer.implementFunction< Vec2 >( "SMAASearchDiag1"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, Vec2 const & texcoord
					, Vec2 const & dir
					, Vec2 e )
				{
					auto coord = writer.declLocale( "coord"
						, vec4( texcoord, -1.0_f, 1.0_f ) );
					auto t = writer.declLocale( "t"
						, vec3( c3d_rtMetrics.xy(), 1.0_f ) );

					WHILE( writer, coord.z() < writer.cast< Float >( c3d_maxSearchStepsDiag - 1 )
						&& coord.w() > 0.9_f )
					{
						coord.xyz() = fma( t, vec3( dir, 1.0_f ), coord.xyz() );
						e = textureLod( edgesTex, coord.xy(), 0.0_f ).rg();
						coord.w() = dot( e, vec2( 0.5_f, 0.5_f ) );
					}
					ELIHW;

					writer.returnStmt( coord.zw() );
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InVec2{ writer, "texcoord" }
				, InVec2{ writer, "dir" }
				, OutVec2{ writer, "e" } );

			auto SMAASearchDiag2 = writer.implementFunction< Vec2 >( "SMAASearchDiag2"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, Vec2 const & texcoord
					, Vec2 const & dir
					, Vec2 e )
				{
					auto coord = writer.declLocale( "coord"
						, vec4( texcoord, -1.0_f, 1.0_f ) );
					coord.x() += 0.25_f * c3d_rtMetrics.x(); // See @SearchDiag2Optimization
					auto t = writer.declLocale( "t"
						, vec3( c3d_rtMetrics.xy(), 1.0_f ) );

					WHILE( writer, coord.z() < writer.cast< Float >( c3d_maxSearchStepsDiag - 1 )
						&& coord.w() > 0.9_f )
					{
						coord.xyz() = fma( t, vec3( dir, 1.0_f ), coord.xyz() );

						// @SearchDiag2Optimization
						// Fetch both edges at once using bilinear filtering:
						e = textureLod( edgesTex, coord.xy(), 0.0_f ).rg();
						e = SMAADecodeDiagBilinearAccess2( e );

						// Non-optimized version:
						// e.g = textureLod(edgesTex, coord.xy, 0.0_f).g;
						// e.r = textureLodOffset(edgesTex, coord.xy, 0.0_f, ivec2(1, 0)).r;

						coord.w() = dot( e, vec2( 0.5_f, 0.5_f ) );
					}
					ELIHW;

					writer.returnStmt( coord.zw() );
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InVec2{ writer, "texcoord" }
				, InVec2{ writer, "dir" }
				, OutVec2{ writer, "e" } );

			/**
			 * Similar to SMAAArea, this calculates the area corresponding to a certain
			 * diagonal distance and crossing edges 'e'.
			 */
			auto SMAAAreaDiag = writer.implementFunction< Vec2 >( "SMAAAreaDiag"
				, [&]( SampledImage2DRgba32 const & areaTex
					, Vec2 const & dist
					, Vec2 const & e
					, Float const & offset )
				{
					auto texcoord = writer.declLocale( "texcoord"
						, fma( vec2( c3d_areaTexMaxDistanceDiag, c3d_areaTexMaxDistanceDiag ), e, dist ) );

					// We do a scale and bias for mapping to texel space:
					texcoord = fma( c3d_areaTexPixelSize, texcoord, 0.5_f * c3d_areaTexPixelSize );

					// Diagonal areas are on the second half of the texture:
					texcoord.x() += 0.5_f;

					// Move to proper place, according to the subpixel offset:
					texcoord.y() += c3d_areaTexSubtexSize * offset;

					// Do it!
					writer.returnStmt( textureLod( areaTex, texcoord, 0.0_f ).rg() );
				}
				, InSampledImage2DRgba32{ writer, "areaTex" }
				, InVec2{ writer, "dist" }
				, InVec2{ writer, "e" }
				, InFloat{ writer, "offset" } );

			/**
			 * This searches for diagonal patterns and returns the corresponding weights.
			 */
			auto SMAACalculateDiagWeights = writer.implementFunction< Vec2 >( "SMAACalculateDiagWeights"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, SampledImage2DRgba32 const & areaTex
					, Vec2 const & texcoord
					, Vec2 const & e
					, Vec4 const & subsampleIndices )
				{
					auto weights = writer.declLocale( "weights"
						, vec2( 0.0_f, 0.0_f ) );

					// Search for the line ends:
					auto d = writer.declLocale< Vec4 >( "d"
						, vec4( 0.0_f ) );
					auto end = writer.declLocale( "end"
						, vec2( 0.0_f ) );

					IF( writer, e.r() > 0.0_f )
					{
						d.xz() = SMAASearchDiag1( edgesTex, texcoord, vec2( -1.0_f, 1.0_f ), end );
						d.x() += 1.0_f - step( end.y(), 0.9_f );// end.y > 0.9 ? 1.0 : 0.0
						// step => 0.9 < end.y ? 0.0 : 1.0
					}
					ELSE
					{
						d.xz() = vec2( 0.0_f, 0.0_f );
					}
					FI;

					d.yw() = SMAASearchDiag1( edgesTex, texcoord, vec2( 1.0_f, -1.0_f ), end );

					IF( writer, d.x() + d.y() > 2.0_f )
					{ // d.x + d.y + 1 > 3
						// Fetch the crossing edges:
						auto coords = writer.declLocale( "coords"
							, fma( vec4( -d.x() + 0.25_f, d.x(), d.y(), -d.y() - 0.25_f )
								, c3d_rtMetrics.xyxy()
								, vec4( texcoord.xy(), texcoord.xy() ) ) );
						auto c = writer.declLocale< Vec4 >( "c" );
						c.xy() = textureLodOffset( edgesTex, coords.xy(), 0.0_f, ivec2( -1_i, 0_i ) ).rg();
						c.zw() = textureLodOffset( edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 0_i ) ).rg();
						c.yxwz() = SMAADecodeDiagBilinearAccess4( c.xyzw() );

						// Non-optimized version:
						// vec4 coords = fma(vec4(-d.x, d.x, d.y, -d.y), SMAA_RT_METRICS.xyxy, texcoord.xyxy);
						// vec4 c;
						// c.x = textureLodOffset(edgesTex, coords.xy, 0.0_f, ivec2(-1,  0)).g;
						// c.y = textureLodOffset(edgesTex, coords.xy, 0.0_f, ivec2( 0,  0)).r;
						// c.z = textureLodOffset(edgesTex, coords.zw, 0.0_f, ivec2( 1,  0)).g;
						// c.w = textureLodOffset(edgesTex, coords.zw, 0.0_f, ivec2( 1, -1)).r;

						// Merge crossing edges at each side into a single value:
						auto cc = writer.declLocale( "cc"
							, fma( vec2( 2.0_f, 2.0_f ), c.xz(), c.yw() ) );

						// Remove the crossing edge if we didn't found the end of the line:
						SMAAMovc( bvec2( step( vec2( 0.9_f ), d.zw() ) ), cc, vec2( 0.0_f, 0.0_f ) );

						// Fetch the areas for this line:
						weights += SMAAAreaDiag( areaTex, d.xy(), cc, subsampleIndices.z() );
					}
					FI;

					// Search for the line ends:
					d.xz() = SMAASearchDiag2( edgesTex, texcoord, vec2( -1.0_f, -1.0_f ), end );

					IF( writer, textureLodOffset( edgesTex, texcoord, 0.0_f, ivec2( 1_i, 0_i ) ).r() > 0.0_f )
					{
						d.yw() = SMAASearchDiag2( edgesTex, texcoord, vec2( 1.0_f, 1.0_f ), end );
						d.y() += 1.0_f - step( end.y(), 0.9_f );// end.y > 0.9 ? 1.0 : 0.0
						// step => 0.9 < end.y ? 0.0 : 1.0
					}
					ELSE
					{
						d.yw() = vec2( 0.0_f, 0.0_f );
					}
					FI;

					IF( writer, d.x() + d.y() > 2.0_f )
					{ // d.x + d.y + 1 > 3
						// Fetch the crossing edges:
						auto coords = writer.declLocale( "coords"
							, fma( vec4( -d.x(), -d.x(), d.y(), d.y() )
								, c3d_rtMetrics.xyxy()
								, vec4( texcoord.xy(), texcoord.xy() ) ) );
						auto c = writer.declLocale< Vec4 >( "c" );
						c.x() = textureLodOffset( edgesTex, coords.xy(), 0.0_f, ivec2( -1_i, 0_i ) ).g();
						c.y() = textureLodOffset( edgesTex, coords.xy(), 0.0_f, ivec2( 0_i, -1_i ) ).r();
						c.zw() = textureLodOffset( edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 0_i ) ).gr();
						auto cc = writer.declLocale( "cc"
							, fma( vec2( 2.0_f, 2.0_f ), c.xz(), c.yw() ) );

						// Remove the crossing edge if we didn't found the end of the line:
						SMAAMovc( bvec2( step( vec2( 0.9_f ), d.zw() ) ), cc, vec2( 0.0_f, 0.0_f ) );

						// Fetch the areas for this line:
						weights += SMAAAreaDiag( areaTex, d.xy(), cc, subsampleIndices.w() ).gr();
					}
					FI;

					writer.returnStmt( weights );
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InSampledImage2DRgba32{ writer, "areaTex" }
				, InVec2{ writer, "texcoord" }
				, InVec2{ writer, "e" }
				, InVec4{ writer, "subsampleIndices" } );

			//-----------------------------------------------------------------------------
			// Horizontal/Vertical Search Functions

			/**
			 * This allows to determine how much length should we add in the last step
			 * of the searches. It takes the bilinearly interpolated edge (see
			 * @PSEUDO_GATHER4), and adds 0, 1 or 2, depending on which edges and
			 * crossing edges are active.
			 */
			auto SMAASearchLength = writer.implementFunction< Float >( "SMAASearchLength"
				, [&]( SampledImage2DRgba32 const & searchTex
					, Vec2 const & e
					, Float const & offset )
				{
					// The texture is flipped vertically, with left and right cases taking half
					// of the space horizontally:
					auto scale = writer.declLocale( "scale"
						, c3d_searchTexSize * vec2( 0.5_f, -1.0_f ) );
					auto bias = writer.declLocale( "bias"
						, c3d_searchTexSize * vec2( offset, 1.0_f ) );

					// Scale and bias to access texel centers:
					scale += vec2( -1.0_f, 1.0_f );
					bias += vec2( 0.5_f, -0.5_f );

					// Convert from pixel coordinates to texcoords:
					// (We use SMAA_SEARCHTEX_PACKED_SIZE because the texture is cropped)
					scale *= vec2( 1.0_f ) / c3d_searchTexPackedSize;
					bias *= vec2( 1.0_f ) / c3d_searchTexPackedSize;

					// Lookup the search texture:
					writer.returnStmt( textureLod( searchTex, fma( scale, e, bias ), 0.0_f ).r() );
				}
				, InSampledImage2DRgba32{ writer, "searchTex" }
				, InVec2{ writer, "e" }
				, InFloat{ writer, "offset" } );

			/**
			 * Horizontal/vertical search functions for the 2nd pass.
			 */
			auto SMAASearchXLeft = writer.implementFunction< Float >( "SMAASearchXLeft"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, SampledImage2DRgba32 const & searchTex
					, Vec2 texcoord
					, Float end )
				{
					/**
					 * @PSEUDO_GATHER4
					 * This texcoord has been offset by (-0.25, -0.125) in the vertex shader to
					 * sample between edge, thus fetching four edges in a row.
					 * Sampling with different offsets in each direction allows to disambiguate
					 * which edges are active from the four fetched ones.
					 */
					auto e = writer.declLocale( "e"
						, vec2( 0.0_f, 1.0_f ) );

					WHILE( writer, texcoord.x() > end
						&& e.g() > 0.8281_f // Is there some edge not activated?
						&& e.r() == 0.0_f )
					{ // Or is there a crossing edge that breaks the line?
						e = textureLod( edgesTex, texcoord, 0.0_f ).rg();
						texcoord = fma( -vec2( 2.0_f, 0.0_f ), c3d_rtMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( "offset"
						, fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e, 0.0_f ), 3.25_f ) );
					writer.returnStmt( fma( c3d_rtMetrics.x(), offset, texcoord.x() ) );

					// Non-optimized version:
					// We correct the previous (-0.25, -0.125) offset we applied:
					// texcoord.x += 0.25 * SMAA_RT_METRICS.x;

					// The searches are bias by 1, so adjust the coords accordingly:
					// texcoord.x += SMAA_RT_METRICS.x;

					// Disambiguate the length added by the last step:
					// texcoord.x += 2.0 * SMAA_RT_METRICS.x; // Undo last step
					// texcoord.x -= SMAA_RT_METRICS.x * (255.0 / 127.0) * SMAASearchLength(searchTex, e, 0.0);
					// return fma(SMAA_RT_METRICS.x, offset, texcoord.x);
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InSampledImage2DRgba32{ writer, "searchTex" }
				, PVec2{ writer, "texcoord" }
				, PFloat{ writer, "end" } );

			auto SMAASearchXRight = writer.implementFunction< Float >( "SMAASearchXRight"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, SampledImage2DRgba32 const & searchTex
					, Vec2 texcoord
					, Float end )
				 {
					 auto e = writer.declLocale( "e"
						 , vec2( 0.0_f, 1.0_f ) );
					 WHILE ( writer, texcoord.x() < end
						 && e.g() > 0.8281_f // Is there some edge not activated?
						 && e.r() == 0.0_f )
					 { // Or is there a crossing edge that breaks the line?
						 e = textureLod( edgesTex, texcoord, 0.0_f ).rg();
						 texcoord = fma( vec2( 2.0_f, 0.0_f ), c3d_rtMetrics.xy(), texcoord );
					 }
					 ELIHW;

					 auto offset = writer.declLocale( "offset"
						 , fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e, 0.5_f ), 3.25_f ) );
					 writer.returnStmt( fma( -c3d_rtMetrics.x(), offset, texcoord.x() ) );
				 }
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InSampledImage2DRgba32{ writer, "searchTex" }
				, PVec2{ writer, "texcoord" }
				, PFloat{ writer, "end" } );

			auto SMAASearchYUp = writer.implementFunction< Float >( "SMAASearchYUp"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, SampledImage2DRgba32 const & searchTex
					, Vec2 texcoord
					, Float end )
				{
					auto e = writer.declLocale( "e"
						 , vec2( 1.0_f, 0.0_f ) );
					WHILE ( writer, texcoord.y() > end
						&& e.r() > 0.8281_f // Is there some edge not activated?
						&& e.g() == 0.0_f )
					{ // Or is there a crossing edge that breaks the line?
						e = textureLod( edgesTex, texcoord, 0.0_f ).rg();
						texcoord = fma( -vec2( 0.0_f, 2.0_f ), c3d_rtMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( "offset"
						, fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e.gr(), 0.0_f ), 3.25_f ) );
					writer.returnStmt( fma( c3d_rtMetrics.y(), offset, texcoord.y() ) );
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InSampledImage2DRgba32{ writer, "searchTex" }
				, PVec2{ writer, "texcoord" }
				, PFloat{ writer, "end" } );

			auto SMAASearchYDown = writer.implementFunction< Float >( "SMAASearchYDown"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, SampledImage2DRgba32 const & searchTex
					, Vec2 texcoord
					, Float end )
				{
					auto e = writer.declLocale( "e"
						 , vec2( 1.0_f, 0.0_f ) );
					WHILE ( writer, texcoord.y() < end
						&& e.r() > 0.8281_f // Is there some edge not activated?
						&& e.g() == 0.0_f )
					{ // Or is there a crossing edge that breaks the line?
						e = textureLod( edgesTex, texcoord, 0.0_f ).rg();
						texcoord = fma( vec2( 0.0_f, 2.0_f ), c3d_rtMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( "offset"
						, fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e.gr(), 0.5_f ), 3.25_f ) );
					writer.returnStmt( fma( -c3d_rtMetrics.y(), offset, texcoord.y() ) );
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InSampledImage2DRgba32{ writer, "searchTex" }
				, PVec2{ writer, "texcoord" }
				, PFloat{ writer, "end" } );

			 /**
			  * Ok, we have the distance and both crossing edges. So, what are the areas
			  * at each side of current edge?
			  */
			auto SMAAArea = writer.implementFunction< Vec2 >( "SMAAArea"
				, [&]( SampledImage2DRgba32 const & areaTex
					, Vec2 const & dist
					, Float const & e1
					, Float const & e2
					, Float const & offset )
				{
					// Rounding prevents precision errors of bilinear filtering:
					auto texcoord = writer.declLocale( "texcoord"
						, fma( vec2( c3d_areaTexMaxDistance, c3d_areaTexMaxDistance ), round( 4.0_f * vec2( e1, e2 ) ), dist ) );

					// We do a scale and bias for mapping to texel space:
					texcoord = fma( c3d_areaTexPixelSize, texcoord, 0.5_f * c3d_areaTexPixelSize );

					// Move to proper place, according to the subpixel offset:
					texcoord.y() = fma( c3d_areaTexSubtexSize, offset, texcoord.y() );

					// Do it!
					writer.returnStmt( textureLod( areaTex, texcoord, 0.0_f ).rg() );
				}
				, InSampledImage2DRgba32{ writer, "areaTex" }
				, InVec2{ writer, "dist" }
				, InFloat{ writer, "e1" }
				, InFloat{ writer, "e2" }
				, InFloat{ writer, "offset" } );

			//-----------------------------------------------------------------------------
			// Corner Detection Functions

			auto SMAADetectHorizontalCornerPattern = writer.implementFunction< Void >( "SMAADetectHorizontalCornerPattern"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, Vec2 weights
					, Vec4 const & texcoord
					, Vec2 const & d )
				{
					if ( !config.data.disableCornerDetection )
					{
						auto leftRight = writer.declLocale( "leftRight"
							, step( d.xy(), d.yx() ) );
						auto rounding = writer.declLocale( "rounding"
							, ( 1.0_f - c3d_cornerRoundingNorm ) * leftRight );

						rounding /= leftRight.x() + leftRight.y(); // Reduce blending for pixels in the center of a line.

						auto factor = writer.declLocale( "factor"
							, vec2( 1.0_f, 1.0_f ) );
						factor.x() -= rounding.x() * textureLodOffset( edgesTex, texcoord.xy(), 0.0_f, ivec2( 0_i, 1_i ) ).r();
						factor.x() -= rounding.y() * textureLodOffset( edgesTex, texcoord.zw(), 0.0_f, ivec2( 1_i, 1_i ) ).r();
						factor.y() -= rounding.x() * textureLodOffset( edgesTex, texcoord.xy(), 0.0_f, ivec2( 0_i, -2_i ) ).r();
						factor.y() -= rounding.y() * textureLodOffset( edgesTex, texcoord.zw(), 0.0_f, ivec2( 1_i, -2_i ) ).r();

						weights *= clamp( factor, vec2( 0.0_f ), vec2( 1.0_f ) );
					}
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InOutVec2{ writer, "weights" }
				, InVec4{ writer, "texcoord" }
				, InVec2{ writer, "d" } );

			auto SMAADetectVerticalCornerPattern = writer.implementFunction< Void >( "SMAADetectVerticalCornerPattern"
				, [&]( SampledImage2DRgba32 const & edgesTex
					, Vec2 weights
					, Vec4 const & texcoord
					, Vec2 const & d )
				{
					if ( !config.data.disableCornerDetection )
					{
						auto leftRight = writer.declLocale( "leftRight"
							, step( d.xy(), d.yx() ) );
						auto rounding = writer.declLocale( "rounding"
							, ( 1.0_f - c3d_cornerRoundingNorm ) * leftRight );

						rounding /= leftRight.x() + leftRight.y();

						auto factor = writer.declLocale( "factor"
							, vec2( 1.0_f, 1.0_f ) );
						factor.x() -= rounding.x() * textureLodOffset( edgesTex, texcoord.xy(), 0.0_f, ivec2( 1_i, 0_i ) ).g();
						factor.x() -= rounding.y() * textureLodOffset( edgesTex, texcoord.zw(), 0.0_f, ivec2( 1_i, 1_i ) ).g();
						factor.y() -= rounding.x() * textureLodOffset( edgesTex, texcoord.xy(), 0.0_f, ivec2( -2_i, 0_i ) ).g();
						factor.y() -= rounding.y() * textureLodOffset( edgesTex, texcoord.zw(), 0.0_f, ivec2( -2_i, 1_i ) ).g();

						weights *= clamp( factor, vec2( 0.0_f ), vec2( 1.0_f ) );
					}
				}
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InOutVec2{ writer, "weights" }
				, InVec4{ writer, "texcoord" }
				, InVec2{ writer, "d" } );

			auto SMAABlendingWeightCalculationPS = writer.implementFunction< Vec4 >( "SMAABlendingWeightCalculationPS"
				, [&]( Vec2 const & texcoord
					, Vec2 const & pixcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & edgesTex
					, SampledImage2DRgba32 const & areaTex
					, SampledImage2DRgba32 const & searchTex
					, Vec4 const & subsampleIndices )
				{ // Just pass zero for SMAA 1x, see @SUBSAMPLE_INDICES.
					auto weights = writer.declLocale( "weights"
						, vec4( 0.0_f, 0.0_f, 0.0_f, 0.0_f ) );

					auto e = writer.declLocale( "e"
						, texture( edgesTex, texcoord ).rg() );

					IF( writer, e.g() > 0.0_f )
					{ // Edge at north
						if ( !config.data.disableDiagonalDetection )
						{
							// Diagonals have both north and west edges, so searching for them in
							// one of the boundaries is enough.
							weights.rg() = SMAACalculateDiagWeights( edgesTex, areaTex, texcoord, e, subsampleIndices );
							// We give priority to diagonals, so if we find a diagonal we skip 
							// horizontal/vertical processing.
							IF( writer, weights.r() == -weights.g() )
							{ // weights.r + weights.g == 0.0
								auto d = writer.declLocale< Vec2 >( "d" );

								// Find the distance to the left:
								auto coords = writer.declLocale< Vec3 >( "coords" );
								coords.x() = SMAASearchXLeft( edgesTex, searchTex, offset[0].xy(), offset[2].x() );
								coords.y() = offset[1].y(); // offset[1].y() = texcoord.y() - 0.25 * c3d_rtMetrics.y() (@CROSSING_OFFSET)
								d.x() = coords.x();

								// Now fetch the left crossing edges, two at a time using bilinear
								// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
								// discern what value each edge has:
								auto e1 = writer.declLocale( "e1"
									, textureLod( edgesTex, coords.xy(), 0.0_f ).r() );

								// Find the distance to the right:
								coords.z() = SMAASearchXRight( edgesTex, searchTex, offset[0].zw(), offset[2].y() );
								d.y() = coords.z();

								// We want the distances to be in pixel units (doing this here allow to
								// better interleave arithmetic and memory accesses):
								d = abs( round( fma( c3d_rtMetrics.zz(), d, -pixcoord.xx() ) ) );

								// SMAAArea below needs a sqrt, as the areas texture is compressed
								// quadratically:
								auto sqrt_d = writer.declLocale( "sqrt_d"
									, sqrt( d ) );

								// Fetch the right crossing edges:
								auto e2 = writer.declLocale( "e2"
									, textureLodOffset( edgesTex, coords.zy(), 0.0_f, ivec2( 1_i, 0_i ) ).r() );

								// Ok, we know how this pattern looks like, now it is time for getting
								// the actual area:
								weights.rg() = SMAAArea( areaTex, sqrt_d, e1, e2, subsampleIndices.y() );

								// Fix corners:
								coords.y() = texcoord.y();
								SMAADetectHorizontalCornerPattern( edgesTex, weights.rg(), vec4( coords.xy(), coords.zy() ), d );
							}
							ELSE
							{
								e.r() = 0.0_f; // Skip vertical processing.
							}
							FI;
						}
						else
						{
							auto d = writer.declLocale< Vec2 >( "d" );

							// Find the distance to the left:
							auto coords = writer.declLocale< Vec3 >( "coords" );
							coords.x() = SMAASearchXLeft( edgesTex, searchTex, offset[0].xy(), offset[2].x() );
							coords.y() = offset[1].y(); // offset[1].y() = texcoord.y() - 0.25 * c3d_rtMetrics.y() (@CROSSING_OFFSET)
							d.x() = coords.x();

							// Now fetch the left crossing edges, two at a time using bilinear
							// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
							// discern what value each edge has:
							auto e1 = writer.declLocale( "e1"
								, textureLod( edgesTex, coords.xy(), 0.0_f ).r() );

							// Find the distance to the right:
							coords.z() = SMAASearchXRight( edgesTex, searchTex, offset[0].zw(), offset[2].y() );
							d.y() = coords.z();

							// We want the distances to be in pixel units (doing this here allow to
							// better interleave arithmetic and memory accesses):
							d = abs( round( fma( c3d_rtMetrics.zz(), d, -pixcoord.xx() ) ) );

							// SMAAArea below needs a sqrt, as the areas texture is compressed
							// quadratically:
							auto sqrt_d = writer.declLocale( "sqrt_d"
								, sqrt( d ) );

							// Fetch the right crossing edges:
							auto e2 = writer.declLocale( "e2"
								, textureLodOffset( edgesTex, coords.zy(), 0.0_f, ivec2( 1_i, 0_i ) ).r() );

							// Ok, we know how this pattern looks like, now it is time for getting
							// the actual area:
							weights.rg() = SMAAArea( areaTex, sqrt_d, e1, e2, subsampleIndices.y() );

							// Fix corners:
							coords.y() = texcoord.y();
							SMAADetectHorizontalCornerPattern( edgesTex, weights.rg(), vec4( coords.xy(), coords.zy() ), d );
						}
					}
					FI;

					IF( writer, e.r() > 0.0_f )
					{ // Edge at west
						auto d = writer.declLocale< Vec2 >( "d" );

						// Find the distance to the top:
						auto coords = writer.declLocale< Vec3 >( "coords" );
						coords.y() = SMAASearchYUp( edgesTex, searchTex, offset[1].xy(), offset[2].z() );
						coords.x() = offset[0].x(); // offset[1].x() = texcoord.x() - 0.25 * c3d_rtMetrics.x();
						d.x() = coords.y();

						// Fetch the top crossing edges:
						auto e1 = writer.declLocale( "e1"
							, textureLod( edgesTex, coords.xy(), 0.0_f ).g() );

						// Find the distance to the bottom:
						coords.z() = SMAASearchYDown( edgesTex, searchTex, offset[1].zw(), offset[2].w() );
						d.y() = coords.z();

						// We want the distances to be in pixel units:
						d = abs( round( fma( c3d_rtMetrics.ww(), d, -pixcoord.yy() ) ) );

						// SMAAArea below needs a sqrt, as the areas texture is compressed 
						// quadratically:
						auto sqrt_d = writer.declLocale( "sqrt_d"
							, sqrt( d ) );

						// Fetch the bottom crossing edges:
						auto e2 = writer.declLocale( "e2"
							, textureLodOffset( edgesTex, coords.xz(), 0.0_f, ivec2( 0_i, 1_i ) ).g() );

						// Get the area for this direction:
						weights.ba() = SMAAArea( areaTex, sqrt_d, e1, e2, subsampleIndices.x() );

						// Fix corners:
						coords.x() = texcoord.x();
						SMAADetectVerticalCornerPattern( edgesTex, weights.ba(), vec4( coords.xy(), coords.xz() ), d );
					}
					FI;

					writer.returnStmt( weights );
				}
				, InVec2{ writer, "texcoord" }
				, InVec2{ writer, "pixcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "edgesTex" }
				, InSampledImage2DRgba32{ writer, "areaTex" }
				, InSampledImage2DRgba32{ writer, "searchTex" }
				, InVec4{ writer, "subsampleIndices" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = SMAABlendingWeightCalculationPS( vtx_texture
						, vtx_pixcoord
						, vtx_offset
						, c3d_edgesTex
						, c3d_areaTex
						, c3d_searchTex
						, c3d_subsampleIndices );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::SamplerPtr doCreateSampler( castor3d::Engine & engine
			, castor::String const & name )
		{
			auto & device = getCurrentRenderDevice( engine );
			ashes::SamplerCreateInfo sampler
			{
				0u,
				VK_FILTER_NEAREST,
				VK_FILTER_NEAREST,
				VK_SAMPLER_MIPMAP_MODE_NEAREST,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				0.0f,
				VK_FALSE,
				1.0f,
				VK_FALSE,
				VK_COMPARE_OP_NEVER,
				-1000.0f,
				1000.0f,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_FALSE
			};
			return device->createSampler( sampler );
		}
	}

	//*********************************************************************************************

	BlendingWeightCalculation::BlendingWeightCalculation( castor3d::RenderTarget & renderTarget
		, ashes::ImageView const & edgeDetectionView
		, castor3d::TextureLayoutSPtr depthView
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), VK_FILTER_LINEAR, TexcoordConfig{} }
		, m_edgeDetectionView{ edgeDetectionView }
		, m_surface{ *renderTarget.getEngine(), cuT( "SmaaBlendingWeightCalculation" ) }
		, m_pointSampler{ doCreateSampler( *renderTarget.getEngine(), cuT( "SMAA_Point" ) ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SmaaBlendingWeightCalculation" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaBlendingWeightCalculation" }
	{
		static constexpr VkFormat colourFormat = VK_FORMAT_R8G8B8A8_UNORM;

		VkExtent2D size{ m_edgeDetectionView.image->getDimensions().width
			, m_edgeDetectionView.image->getDimensions().height };

		m_ubo = castor3d::makeUniformBuffer< castor::Point4f >( m_renderSystem
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "SmaaBlendingWeightCalculation" );
		
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_R8G8_UNORM,
			{ size.width, size.height, 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ),
		};

		auto areaTexBuffer = PxBufferBase::create( Size{ AREATEX_WIDTH, AREATEX_HEIGHT }
			, PixelFormat::eR8A8_UNORM
			, areaTexBytes
			, PixelFormat::eR8A8_UNORM );
		m_areaTex = std::make_shared< castor3d::TextureLayout >( m_renderSystem
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SmaaAreaTex" ) );
		m_areaTex->getDefaultImage().initialiseSource( areaTexBuffer );
		m_areaTex->initialise();

		auto searchTexBuffer = PxBufferBase::create( Size{ SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT }
			, PixelFormat::eR8_UNORM
			, searchTexBytes
			, PixelFormat::eR8_UNORM );
		image->format = VK_FORMAT_R8_UNORM;
		m_searchTex = std::make_shared< castor3d::TextureLayout >( m_renderSystem
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SmaaSearchTex" ) );
		m_searchTex->getDefaultImage().initialiseSource( searchTexBuffer );
		m_searchTex->initialise();

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				colourFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
			{
				0u,
				depthView->getDefaultView()->format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				VkAttachmentReference{ 1u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attachments ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		setDebugObjectName( device, *m_renderPass, "BlendingWeightCalculation" );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader.shader = doBlendingWeightCalculationVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		m_pixelShader.shader = doBlendingWeightCalculationFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_vertexShader ) );
		stages.push_back( makeShaderState( device, m_pixelShader ) );

		ashes::PipelineDepthStencilStateCreateInfo dsstate{ 0u, VK_FALSE, VK_FALSE };
		dsstate->stencilTestEnable = VK_TRUE;
		dsstate->front.compareOp = VK_COMPARE_OP_EQUAL;
		dsstate->front.reference = 1u;
		dsstate->back = dsstate->front;
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( 1u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( 2u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		createPipeline( size
			, castor::Position{}
			, stages
			, m_edgeDetectionView
			, *m_renderPass
			, std::move( setLayoutBindings )
			, {}
			, std::move( dsstate ) );
		m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, colourFormat
			, depthView );
	}

	castor3d::CommandsSemaphore BlendingWeightCalculation::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		castor3d::CommandsSemaphore blendingWeightCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & blendingWeightCmd = *blendingWeightCommands.commandBuffer;

		blendingWeightCmd.begin();
		blendingWeightCmd.beginDebugBlock(
			{
				"SMAA BlendingWeightCalculation",
				castor3d::makeFloatArray( getRenderSystem()->getEngine()->getNextRainbowColour() ),
			} );
		timer.beginPass( blendingWeightCmd, passIndex );
		// Put edge detection image in shader input layout.
		blendingWeightCmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_edgeDetectionView.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

		blendingWeightCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, {
				castor3d::transparentBlackClearColor,
				castor3d::defaultClearDepthStencil,
			}
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( blendingWeightCmd );
		blendingWeightCmd.endRenderPass();
		timer.endPass( blendingWeightCmd, passIndex );
		blendingWeightCmd.endDebugBlock();
		blendingWeightCmd.end();

		return std::move( blendingWeightCommands );
	}

	void BlendingWeightCalculation::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "BlendingWeightCalculation" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_vertexShader.shader );
		visitor.visit( cuT( "BlendingWeightCalculation" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pixelShader.shader );
	}

	void BlendingWeightCalculation::update( castor::Point4f const & subsampleIndices )
	{
		auto & data = m_ubo->getData();
		data = subsampleIndices;
		m_ubo->upload();
	}

	void BlendingWeightCalculation::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( 0u )
			, *m_ubo );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_areaTex->getDefaultView()
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 2u )
			, m_searchTex->getDefaultView()
			, *m_pointSampler );
	}

	//*********************************************************************************************
}

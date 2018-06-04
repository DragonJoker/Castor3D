#include "BlendingWeightCalculation.hpp"

#include "AreaTex.h"
#include "SearchTex.h"
#include "SmaaUbo.hpp"

#include <Engine.hpp>

#include <Render/RenderPassTimer.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Buffer/UniformBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doBlendingWeightCalculationVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_renderTargetMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );
			auto c3d_maxSearchSteps = writer.declConstant( constants::MaxSearchSteps
				, Int( config.maxSearchSteps ) );

			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_pixcoord = writer.declOutput< Vec2 >( cuT( "vtx_pixcoord" ), 1u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( cuT( "vtx_offset" ), 2u, 3u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;
					vtx_pixcoord = vtx_texture * c3d_renderTargetMetrics.zw();

					// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
					vtx_offset[0] = fma( c3d_renderTargetMetrics.xyxy(), vec4( -0.25_f, -0.125, 1.25, -0.125 ), vec4( vtx_texture, vtx_texture ) );
					vtx_offset[1] = fma( c3d_renderTargetMetrics.xyxy(), vec4( -0.125_f, -0.25, -0.125, 1.25 ), vec4( vtx_texture, vtx_texture ) );

					// And these for the searches, they indicate the ends of the loops:
					vtx_offset[2] = fma( c3d_renderTargetMetrics.xyxy()
						, vec4( -2.0_f, 2.0, -2.0, 2.0 ) * writer.cast< Float >( c3d_maxSearchSteps )
						, vec4( vtx_offset[0].xz(), vtx_offset[1].yw() ) );
				} );
			return writer.finalise();
		}

		glsl::Shader doBlendingWeightCalculationFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			auto c3d_renderTargetMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );
			auto c3d_maxSearchStepsDiag = writer.declConstant( constants::MaxSearchStepsDiag
				, Int( config.maxSearchStepsDiag ) );
			auto c3d_cornerRounding = writer.declConstant( constants::CornerRounding
				, Int( config.cornerRounding ) );
			auto c3d_areaTexPixelSize = writer.declConstant( constants::AreaTexPixelSize
				, vec2( 1.0_f / 160.0_f, 1.0_f / 560.0_f ) );
			auto c3d_areaTexSubtexSize = writer.declConstant( constants::AreaTexSubtexSize
				, writer.paren( 1.0_f / 7.0_f ) );
			auto c3d_areaTexMaxDistance = writer.declConstant< Int >( constants::AreaTexMaxDistance
				, 16_i );
			auto c3d_areaTexMaxDistanceDiag = writer.declConstant< Int >( constants::AreaTexMaxDistanceDiag
				, 20_i );
			auto c3d_searchTexSize = writer.declConstant( constants::SearchTexSize
				, vec2( 66.0_f, 33.0_f ) );
			auto c3d_searchTexPackedSize = writer.declConstant( constants::SearchTexPackedSize
				, vec2( 64.0_f, 16.0_f ) );
			auto c3d_cornerRoundingNorm = writer.declConstant( constants::CornerRoundingNorm
				, writer.cast< Float >( c3d_cornerRounding ) / 100.0_f );

			// Shader inputs
			glsl::Ubo ubo{ writer, cuT( "Subsample" ), 0u, 0u };
			auto c3d_subsampleIndices = ubo.declMember< IVec4 >( constants::SubsampleIndices );
			ubo.end();
			auto c3d_areaTex = writer.declSampler< Sampler2D >( cuT( "c3d_areaTex" ), 1u, 0u );
			auto c3d_searchTex = writer.declSampler< Sampler2D >( cuT( "c3d_searchTex" ), 2u, 0u );
			auto c3d_edgesTex = writer.declSampler< Sampler2D >( cuT( "c3d_edgesTex" ), 3u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_pixcoord = writer.declInput< Vec2 >( cuT( "vtx_pixcoord" ), 1u );
			auto vtx_offset = writer.declInputArray< Vec4 >( cuT( "vtx_offset" ), 2u, 3u );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			/**
			* Conditional move:
			*/
			auto movc2 = writer.implementFunction< Void >( cuT( "movc2" )
				, [&]( BVec2 const & cond
					, Vec2 & variable
					, Vec2 const & value )
				{
					IF( writer, cond.x() )
					{
						variable.x() = value.x();
					}
					FI
						IF( writer, cond.y() )
					{
						variable.y() = value.y();
					}
					FI
				}
				, InBVec2{ &writer, cuT( "cond" ) }
				, InOutVec2{ &writer, cuT( "variable" ) }
				, InVec2{ &writer, cuT( "value" ) } );

			/**
			* Allows to decode two binary values from a bilinear-filtered access.
			*/
			auto decodeDiagBilinearAccess2 = writer.implementFunction< Vec2 >( cuT( "decodeDiagBilinearAccess2" )
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
					e.r() = e.r() * abs( e.r() * 5.0_f - 5.0 * 0.75 );
					writer.returnStmt( round( e ) );
				}
			, InVec2{ &writer, cuT( "e" ) } );

			auto decodeDiagBilinearAccess4 = writer.implementFunction< Vec4 >( cuT( "decodeDiagBilinearAccess4" )
				, [&]( Vec4 const & e )
				{
					e.rb() = e.rb() * abs( e.rb() * 5.0_f - 5.0 * 0.75 );
					writer.returnStmt( round( e ) );
				}
			, InVec4{ &writer, cuT( "e" ) } );

			//-----------------------------------------------------------------------------
			// Diagonal Search Functions

			/**
			* These functions allows to perform diagonal pattern searches.
			*/
			auto searchDiag1 = writer.implementFunction< Vec2 >( cuT( "searchDiag1" )
				, [&]( Vec2 const & texcoord
					, Vec2 const & dir
					, Vec2 & e )
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, vec4( texcoord, -1.0, 1.0 ) );
					auto t = writer.declLocale( cuT( "t" )
						, vec3( c3d_renderTargetMetrics.xy(), 1.0 ) );

					WHILE( writer, coord.z() < writer.cast< Float >( c3d_maxSearchStepsDiag - 1 ) && coord.w() > 0.9_f )
					{
						coord.xyz() = fma( t, vec3( dir, 1.0 ), coord.xyz() );
						e = texture( c3d_edgesTex, coord.xy(), 0.0_f ).rg();
						coord.w() = dot( e, vec2( 0.5_f ) );
					}
					ELIHW;

					writer.returnStmt( coord.zw() );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
					, InVec2{ &writer, cuT( "dir" ) }
				, OutVec2{ &writer, cuT( "e" ) } );

			auto searchDiag2 = writer.implementFunction< Vec2 >( cuT( "searchDiag2" )
				, [&]( Vec2 const & texcoord
					, Vec2 const & dir
					, Vec2 & e )
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, vec4( texcoord, -1.0, 1.0 ) );
					coord.x() += 0.25_f * c3d_renderTargetMetrics.x();
					auto t = writer.declLocale( cuT( "t" )
						, vec3( c3d_renderTargetMetrics.xy(), 1.0 ) );

					WHILE( writer, coord.z() < writer.cast< Float >( c3d_maxSearchStepsDiag - 1 ) && coord.w() > 0.9_f )
					{
						coord.xyz() = fma( t, vec3( dir, 1.0 ), coord.xyz() );

						// Fetch both edges at once using bilinear filtering:
						e = texture( c3d_edgesTex, coord.xy(), 0.0_f ).rg();
						e = decodeDiagBilinearAccess2( e );

						coord.w() = dot( e, vec2( 0.5_f ) );
					}
					ELIHW;

					writer.returnStmt( coord.zw() );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
					, InVec2{ &writer, cuT( "dir" ) }
				, OutVec2{ &writer, cuT( "e" ) } );

			/**
			* Similar to SMAAArea, this calculates the area corresponding to a certain
			* diagonal distance and crossing edges 'e'.
			*/
			auto areaDiag = writer.implementFunction< Vec2 >( cuT( "areaDiag" )
				, [&]( Vec2 const & dist
					, Vec2 const & e
					, Float const & offset )
				{
					auto texcoord = writer.declLocale( cuT( "texcoord" )
						, fma( vec2( writer.cast< Float >( c3d_areaTexMaxDistanceDiag ) ), e, dist ) );

					// We do a scale and bias for mapping to texel space:
					texcoord = fma( c3d_areaTexPixelSize, texcoord, 0.5_f * c3d_areaTexPixelSize );

					// Diagonal areas are on the second half of the texture:
					texcoord.x() += 0.5_f;

					// Move to proper place, according to the subpixel offset:
					texcoord.y() += c3d_areaTexSubtexSize * offset;

					// Do it!
					writer.returnStmt( texture( c3d_areaTex, texcoord, 0.0_f ).rg() );
				}
				, InVec2{ &writer, cuT( "dist" ) }
					, InVec2{ &writer, cuT( "e" ) }
				, InFloat{ &writer, cuT( "offset" ) } );

			/**
			* This searches for diagonal patterns and returns the corresponding weights.
			*/
			auto calculateDiagWeights = writer.implementFunction< Vec2 >( cuT( "calculateDiagWeights" )
				, [&]( Vec2 const & texcoord
					, Vec2 const & e )
				{
					auto weights = writer.declLocale< Vec2 >( cuT( "weights" )
						, vec2( 0.0_f ) );
					auto d = writer.declLocale< Vec4 >( cuT( "d" ) );
					auto end = writer.declLocale< Vec2 >( cuT( "end" ) );

					IF( writer, e.r() > 0.0 )
					{
						d.xz() = searchDiag1( texcoord, vec2( -1.0_f, 1.0 ), end );
						d.x() += writer.cast< Float >( end.y() > 0.9_f );
					}
					ELSE
					{
						d.xz() = vec2( 0.0_f );
					}
						FI

						d.yw() = searchDiag1( texcoord, vec2( 1.0_f, -1.0 ), end );

					IF( writer, d.x() + d.y() > 2.0_f ) // d.r + d.g + 1 > 3
					{
						auto coords = writer.declLocale( cuT( "coords" )
							, fma( vec4( -d.x() + 0.25, d.x(), d.y(), -d.y() - 0.25_f )
								, c3d_renderTargetMetrics.xyxy()
								, vec4( texcoord, texcoord ) ) );

						auto c = writer.declLocale< Vec4 >( cuT( "c" ) );
						c.xy() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( -1_i, 0 ) ).rg();
						c.zw() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 0 ) ).rg();
						c.yxwz() = decodeDiagBilinearAccess4( c.xyzw() );

						// Merge crossing edges at each side into a single value:
						auto cc = writer.declLocale( cuT( "cc" )
							, fma( vec2( 2.0_f ), c.xz(), c.yw() ) );

						// Remove the crossing edge if we didn't found the end of the line:
						writer << movc2( bvec2( step( vec2( 0.9_f ), d.zw() ) ), cc, vec2( 0.0_f ) ) << endi;

						weights += areaDiag( d.xy(), cc, writer.cast< Float >( c3d_subsampleIndices.z() ) );
					}
					FI;

					d.xz() = searchDiag2( texcoord, vec2( -1.0_f, -1.0 ), end );

					IF( writer, textureLodOffset( c3d_edgesTex, texcoord, 0.0_f, ivec2( 1_i, 0 ) ).r() > 0.0_f )
					{
						d.yw() = searchDiag2( texcoord, vec2( 1.0_f, 1.0 ), end );
						d.y() += writer.cast< Float >( end.y() > 0.9_f );
					}
					ELSE
					{
						d.yw() = vec2( 0.0_f );
					}
						FI

						IF( writer, d.x() + d.y() > 2.0_f ) // d.r + d.g + 1 > 3
					{
						auto coords = writer.declLocale( cuT( "coords" )
							, fma( vec4( -d.x(), -d.x(), d.y(), d.y() )
								, c3d_renderTargetMetrics.xyxy()
								, vec4( texcoord, texcoord ) ) );

						auto c = writer.declLocale< Vec4 >( cuT( "c" ) );
						c.x() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( -1_i, 0 ) ).g();
						c.y() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( 0_i, -1 ) ).r();
						c.zw() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 0 ) ).gr();

						// Merge crossing edges at each side into a single value:
						auto cc = writer.declLocale( cuT( "cc" )
							, fma( vec2( 2.0_f ), c.xz(), c.yw() ) );

						// Remove the crossing edge if we didn't found the end of the line:
						writer << movc2( bvec2( step( vec2( 0.9_f ), d.zw() ) ), cc, vec2( 0.0_f ) ) << endi;

						weights += areaDiag( d.xy(), cc, writer.cast< Float >( c3d_subsampleIndices.w() ) ).gr();
					}
					FI;

					writer.returnStmt( weights );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InVec2{ &writer, cuT( "e" ) } );

			//-----------------------------------------------------------------------------
			// Horizontal/Vertical Search Functions

			/**
			* This allows to determine how much length should we add in the last step
			* of the searches. It takes the bilinearly interpolated edge (see
			* @PSEUDO_GATHER4), and adds 0, 1 or 2, depending on which edges and
			* crossing edges are active.
			*/
			auto searchLength = writer.implementFunction< Float >( cuT( "searchLength" )
				, [&]( Vec2 const & e
					, Float const & offset )
				{
					// The texture is flipped vertically, with left and right cases taking half
					// of the space horizontally:
					auto scale = writer.declLocale( cuT( "scale" )
						, vec2( c3d_searchTexSize ) * vec2( 0.5_f, -1.0 ) );
					auto bias = writer.declLocale( cuT( "bias" )
						, vec2( c3d_searchTexSize ) * vec2( offset, 1.0 ) );

					// Scale and bias to access texel centers:
					scale += vec2( -1.0_f, 1.0 );
					bias += vec2( 0.5_f, -0.5 );

					// Convert from pixel coordinates to texcoords:
					// (We use SMAA_SEARCHTEX_PACKED_SIZE because the texture is cropped)
					scale *= 1.0_f / c3d_searchTexPackedSize;
					bias *= 1.0_f / c3d_searchTexPackedSize;

					// Lookup the search texture:
					writer.returnStmt( texture( c3d_searchTex, fma( scale, e, bias ), 0.0_f ).r() );
				}
				, InVec2{ &writer, cuT( "e" ) }
				, InFloat{ &writer, cuT( "offset" ) } );

			/**
			* Horizontal/vertical search functions for the 2nd pass.
			*/
			auto searchXLeft = writer.implementFunction< Float >( cuT( "searchXLeft" )
				, [&]( Vec2 texcoord
					, Float const & end )
				{
					/**
					* @PSEUDO_GATHER4
					* This texcoord has been offset by (-0.25, -0.125) in the vertex shader to
					* sample between edge, thus fetching four edges in a row.
					* Sampling with different offsets in each direction allows to disambiguate
					* which edges are active from the four fetched ones.
					*/
					auto e = writer.declLocale( cuT( "e" )
						, vec2( 0.0_f, 1.0 ) );

					WHILE( writer, "texcoord.x > end "
						"&& e.g > 0.8281"// Is there some edge not activated?
						"&& e.r == 0.0" )// Or is there a crossing edge that breaks the line?
					{
						e = texture( c3d_edgesTex, texcoord, 0.0_f ).rg();
						texcoord = fma( -vec2( 2.0_f, 0.0 ), c3d_renderTargetMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( cuT( "offset" )
						, fma( -255.0_f / 127.0, searchLength( e, 0.0_f ), 3.25_f ) );
					writer.returnStmt( fma( c3d_renderTargetMetrics.x(), offset, texcoord.x() ) );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InFloat{ &writer, cuT( "end" ) } );

			auto searchXRight = writer.implementFunction< Float >( cuT( "searchXRight" )
				, [&]( Vec2 texcoord
					, Float const & end )
				{
					auto e = writer.declLocale( cuT( "e" )
						, vec2( 0.0_f, 1.0 ) );

					WHILE( writer, "texcoord.x < end "
						"&& e.g > 0.8281"// Is there some edge not activated?
						"&& e.r == 0.0" )// Or is there a crossing edge that breaks the line?
					{
						e = texture( c3d_edgesTex, texcoord, 0.0_f ).rg();
						texcoord = fma( vec2( 2.0_f, 0.0 ), c3d_renderTargetMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( cuT( "offset" )
						, fma( -255.0_f / 127.0, searchLength( e, 0.5_f ), 3.25_f ) );
					writer.returnStmt( fma( -c3d_renderTargetMetrics.x(), offset, texcoord.x() ) );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InFloat{ &writer, cuT( "end" ) } );

			auto searchYUp = writer.implementFunction< Float >( cuT( "searchYUp" )
				, [&]( Vec2 texcoord
					, Float const & end )
				{
					auto e = writer.declLocale( cuT( "e" )
						, vec2( 1.0_f, 0.0 ) );

					WHILE( writer, "texcoord.y > end "
						"&& e.r > 0.8281"// Is there some edge not activated?
						"&& e.g == 0.0" )// Or is there a crossing edge that breaks the line?
					{
						e = texture( c3d_edgesTex, texcoord, 0.0_f ).rg();
						texcoord = fma( -vec2( 0.0_f, 2.0 ), c3d_renderTargetMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( cuT( "offset" )
						, fma( -255.0_f / 127.0, searchLength( e.gr(), 0.0_f ), 3.25_f ) );
					writer.returnStmt( fma( c3d_renderTargetMetrics.y(), offset, texcoord.y() ) );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InFloat{ &writer, cuT( "end" ) } );

			auto searchYDown = writer.implementFunction< Float >( cuT( "searchYDown" )
				, [&]( Vec2 texcoord
					, Float const & end )
				{
					auto e = writer.declLocale( cuT( "e" )
						, vec2( 1.0_f, 0.0 ) );

					WHILE( writer, "texcoord.y < end "
						"&& e.r > 0.8281"// Is there some edge not activated?
						"&& e.g == 0.0" )// Or is there a crossing edge that breaks the line?
					{
						e = texture( c3d_edgesTex, texcoord, 0.0_f ).rg();
						texcoord = fma( vec2( 0.0_f, 2.0 ), c3d_renderTargetMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( cuT( "offset" )
						, fma( -255.0_f / 127.0, searchLength( e.gr(), 0.5_f ), 3.25_f ) );
					writer.returnStmt( fma( -c3d_renderTargetMetrics.y(), offset, texcoord.y() ) );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InFloat{ &writer, cuT( "end" ) } );

			/**
			* Ok, we have the distance and both crossing edges. So, what are the areas
			* at each side of current edge?
			*/
			auto area = writer.implementFunction< Vec2 >( cuT( "area" )
				, [&]( Vec2 const & dist
					, Float const & e1
					, Float const & e2
					, Float const & offset )
				{
					// Rounding prevents precision errors of bilinear filtering:
					auto texcoord = writer.declLocale( cuT( "texcoord" )
						, fma( vec2( c3d_areaTexMaxDistance )
							, round( vec2( 4.0_f ) * vec2( e1, e2 ) )
							, dist ) );

					// We do a scale and bias for mapping to texel space:
					texcoord = fma( c3d_areaTexPixelSize, texcoord, c3d_areaTexPixelSize * 0.5_f );

					// Move to proper place, according to the subpixel offset:
					texcoord.y() = fma( c3d_areaTexSubtexSize, offset, texcoord.y() );

					// Do it!
					writer.returnStmt( texture( c3d_areaTex, texcoord, 0.0_f ).rg() );
				}
				, InVec2{ &writer, cuT( "dist" ) }
				, InFloat{ &writer, cuT( "e1" ) }
				, InFloat{ &writer, cuT( "e2" ) }
				, InFloat{ &writer, cuT( "offset" ) } );

			//-----------------------------------------------------------------------------
			// Corner Detection Functions
			
			auto detectHorizontalCornerPattern = writer.implementFunction< Void >( cuT( "detectHorizontalCornerPattern" )
				, [&]( Vec2 & weights
					, Vec4 const & texcoord
					, Vec2 const & d )
				{
					if ( !config.disableCornerDetection )
					{
						auto leftRight = writer.declLocale( cuT( "leftRight" )
							, step( d.xy(), d.yx() ) );
						auto rounding = writer.declLocale( cuT( "rounding" )
							, writer.paren( vec2( 1.0_f ) - c3d_cornerRoundingNorm ) * leftRight );

						rounding /= leftRight.x() + leftRight.y(); // Reduce blending for pixels in the center of a line.

						auto factor = writer.declLocale( cuT( "factor" )
							, vec2( 1.0_f, 1.0 ) );
						factor.x() -= rounding.x() * textureLodOffset( c3d_edgesTex, texcoord.xy(), 0.0_f, ivec2( 0_i, 1 ) ).r();
						factor.x() -= rounding.y() * textureLodOffset( c3d_edgesTex, texcoord.zw(), 0.0_f, ivec2( 1_i, 1 ) ).r();
						factor.y() -= rounding.x() * textureLodOffset( c3d_edgesTex, texcoord.xy(), 0.0_f, ivec2( 0_i, -2 ) ).r();
						factor.y() -= rounding.y() * textureLodOffset( c3d_edgesTex, texcoord.zw(), 0.0_f, ivec2( 1_i, -2 ) ).r();

						weights *= clamp( factor, vec2( 0.0_f ), vec2( 1.0_f ) );
					}
				}
				, InOutVec2{ &writer, cuT( "weights" ) }
				, InVec4{ &writer, cuT( "texcoord" ) }
				, InVec2{ &writer, cuT( "d" ) } );
			
			auto detectVerticalCornerPattern = writer.implementFunction< Void >( cuT( "detectVerticalCornerPattern" )
				, [&]( Vec2 weights
					, Vec4 const & texcoord
					, Vec2 const & d )
				{
					if ( !config.disableCornerDetection )
					{
						auto leftRight = writer.declLocale( cuT( "leftRight" )
							, step( d.xy(), d.yx() ) );
						auto rounding = writer.declLocale( cuT( "rounding" )
							, writer.paren( vec2( 1.0_f ) - c3d_cornerRoundingNorm ) * leftRight );

						rounding /= leftRight.x() + leftRight.y(); // Reduce blending for pixels in the center of a line.

						auto factor = writer.declLocale( cuT( "factor" )
							, vec2( 1.0_f, 1.0 ) );
						factor.x() -= rounding.x() * textureLodOffset( c3d_edgesTex, texcoord.xy(), 0.0_f, ivec2( 1_i, 0 ) ).g();
						factor.x() -= rounding.y() * textureLodOffset( c3d_edgesTex, texcoord.zw(), 0.0_f, ivec2( 1_i, 1 ) ).g();
						factor.y() -= rounding.x() * textureLodOffset( c3d_edgesTex, texcoord.xy(), 0.0_f, ivec2( -2_i, 0 ) ).g();
						factor.y() -= rounding.y() * textureLodOffset( c3d_edgesTex, texcoord.zw(), 0.0_f, ivec2( -2_i, 1 ) ).g();

						weights *= clamp( factor, vec2( 0.0_f ), vec2( 1.0_f ) );
					}
				}
				, InOutVec2{ &writer, cuT( "weights" ) }
				, InVec4{ &writer, cuT( "texcoord" ) }
				, InVec2{ &writer, cuT( "d" ) } );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto texcoord = writer.declLocale( cuT( "texcoord" )
						, vtx_texture );

					auto weights = writer.declLocale( cuT( "weights" )
						, vec4( 0.0_f ) );

					auto e = writer.declLocale( cuT( "e" )
						, texture( c3d_edgesTex, texcoord ).rg() );

					IF( writer, e.g() > 0.0 ) // Edge at north
					{
						if ( !config.disableDiagonalDetection )
						{
							// Diagonals have both north and west edges, so searching for them in
							// one of the boundaries is enough.
							weights.rg() = calculateDiagWeights( texcoord, e );

							// We give priority to diagonals, so if we find a diagonal we skip 
							// horizontal/vertical processing.
							IF( writer, weights.r() == weights.g() )
							{
								auto d = writer.declLocale< Vec2 >( cuT( "d" ) );

								// Find the distance to the left:
								auto coords = writer.declLocale< Vec3 >( cuT( "coords" ) );
								coords.x() = searchXLeft( vtx_offset[0].xy(), vtx_offset[2].x() );
								coords.y() = vtx_offset[1].y(); // vtx_offset[1].y() = texcoord.y() - 0.25_f * c3d_pixelSize.y() (@CROSSING_OFFSET)
								d.x() = coords.x();

								// Now fetch the left crossing edges, two at a time using bilinear
								// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
								// discern what value each edge has:
								auto e1 = writer.declLocale( cuT( "e1" )
									, texture( c3d_edgesTex, coords.xy(), 0.0_f ).r() );

								// Find the distance to the right:
								coords.z() = searchXRight( vtx_offset[0].zw(), vtx_offset[2].y() );
								d.y() = coords.z();

								// We want the distances to be in pixel units (doing this here allow to
								// better interleave arithmetic and memory accesses):
								d = abs( round( fma( c3d_renderTargetMetrics.zz(), d, -vtx_pixcoord.xx() ) ) );

								// SMAAArea below needs a sqrt, as the areas texture is compressed 
								// quadratically:
								auto sqrt_d = writer.declLocale( cuT( "sqrt_d" )
									, sqrt( d ) );

								// Fetch the right crossing edges:
								auto e2 = writer.declLocale( cuT( "e2" )
									, textureLodOffset( c3d_edgesTex, coords.zy(), 0.0_f, ivec2( 1_i, 0 ) ).r() );

								// Ok, we know how this pattern looks like, now it is time for getting
								// the actual area:
								weights.rg() = area( sqrt_d, e1, e2, writer.cast< Float >( c3d_subsampleIndices.y() ) );

								// Fix corners:
								coords.y() = texcoord.y();
								writer << detectHorizontalCornerPattern( weights.rg(), vec4( coords.xy(), coords.zy() ), d ) << endi;
							}
							ELSE
							{
								e.r() = 0.0_f; // Skip vertical processing.
							}
							FI;
						}
						else
						{
							auto d = writer.declLocale< Vec2 >( cuT( "d" ) );

							// Find the distance to the left:
							auto coords = writer.declLocale< Vec3 >( cuT( "coords" ) );
							coords.x() = searchXLeft( vtx_offset[0].xy(), vtx_offset[2].x() );
							coords.y() = vtx_offset[1].y(); // vtx_offset[1].y() = texcoord.y() - 0.25_f * c3d_pixelSize.y() (@CROSSING_OFFSET)
							d.x() = coords.x();

							// Now fetch the left crossing edges, two at a time using bilinear
							// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
							// discern what value each edge has:
							auto e1 = writer.declLocale( cuT( "e1" )
								, texture( c3d_edgesTex, coords.xy(), 0.0_f ).r() );

							// Find the distance to the right:
							coords.z() = searchXRight( vtx_offset[0].zw(), vtx_offset[2].y() );
							d.y() = coords.z();

							// We want the distances to be in pixel units (doing this here allow to
							// better interleave arithmetic and memory accesses):
							d = abs( round( fma( c3d_renderTargetMetrics.zz(), d, -vtx_pixcoord.xx() ) ) );

							// SMAAArea below needs a sqrt, as the areas texture is compressed 
							// quadratically:
							auto sqrt_d = writer.declLocale( cuT( "sqrt_d" )
								, sqrt( d ) );

							// Fetch the right crossing edges:
							auto e2 = writer.declLocale( cuT( "e2" )
								, textureLodOffset( c3d_edgesTex, coords.zy(), 0.0_f, ivec2( 1_i, 0 ) ).r() );

							// Ok, we know how this pattern looks like, now it is time for getting
							// the actual area:
							weights.rg() = area( sqrt_d, e1, e2, writer.cast< Float >( c3d_subsampleIndices.y() ) );

							// Fix corners:
							coords.y() = texcoord.y();
							writer << detectHorizontalCornerPattern( weights.rg(), vec4( coords.xy(), coords.zy() ), d ) << endi;
						}
					}
					FI;

					IF( writer, e.r() > 0.0_f )
					{ // Edge at west
						auto d = writer.declLocale< Vec2 >( cuT( "d" ) );

						// Find the distance to the top:
						auto coords = writer.declLocale< Vec3 >( cuT( "coords" ) );
						coords.y() = searchYUp( vtx_offset[1].xy(), vtx_offset[2].z() );
						coords.x() = vtx_offset[0].x(); // vtx_offset[1].x() = texcoord.x() - 0.25 * c3d_pixelSize.x();
						d.x() = coords.y();

						// Fetch the top crossing edges:
						auto e1 = writer.declLocale( cuT( "e1" )
							, texture( c3d_edgesTex, coords.xy(), 0.0_f ).g() );

						// Find the distance to the bottom:
						coords.z() = searchYDown( vtx_offset[1].zw(), vtx_offset[2].w() );
						d.y() = coords.z();

						// We want the distances to be in pixel units:
						d = abs( round( fma( c3d_renderTargetMetrics.ww(), d, -vtx_pixcoord.yy() ) ) );

						// SMAAArea below needs a sqrt, as the areas texture is compressed 
						// quadratically:
						auto sqrt_d = writer.declLocale( cuT( "sqrt_d" )
							, sqrt( d ) );

						// Fetch the bottom crossing edges:
						auto e2 = writer.declLocale( cuT( "e2" )
							, textureLodOffset( c3d_edgesTex, coords.xz(), 0.0_f, ivec2( 0_i, 1 ) ).g() );

						// Get the area for this direction:
						weights.ba() = area( sqrt_d, e1, e2, writer.cast< Float >( c3d_subsampleIndices.x() ) );

						// Fix corners:
						coords.x() = texcoord.x();
						writer << detectVerticalCornerPattern( weights.ba(), vec4( coords.xy(), coords.xz() ), d ) << endi;
					}
					FI;

					pxl_fragColour = weights;
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	BlendingWeightCalculation::BlendingWeightCalculation( castor3d::RenderTarget & renderTarget
		, renderer::TextureView const & edgeDetectionView
		, castor3d::TextureLayoutSPtr depthView
		, castor3d::SamplerSPtr sampler
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_edgeDetectionView{ edgeDetectionView }
		, m_surface{ *renderTarget.getEngine() }
	{
		renderer::Extent2D size{ m_edgeDetectionView.getTexture().getDimensions().width
			, m_edgeDetectionView.getTexture().getDimensions().height };
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		m_ubo = renderer::makeUniformBuffer< castor::Point4i >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		renderer::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = size.width;
		image.extent.height = size.height;
		image.extent.depth = 1u;
		image.format = renderer::Format::eR8G8_UNORM;
		image.imageType = renderer::TextureType::e2D;
		image.initialLayout = renderer::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = renderer::SampleCountFlag::e1;
		image.sharingMode = renderer::SharingMode::eExclusive;
		image.tiling = renderer::ImageTiling::eOptimal;
		image.usage = renderer::ImageUsageFlag::eSampled
			| renderer::ImageUsageFlag::eTransferDst;

		auto areaTexBuffer = PxBufferBase::create( Size{ AREATEX_WIDTH, AREATEX_HEIGHT }
			, PixelFormat::eA8L8
			, areaTexBytes
			, PixelFormat::eA8L8 );
		m_areaTex = std::make_shared< castor3d::TextureLayout >( renderSystem
			, image
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_areaTex->getDefaultImage().initialiseSource( areaTexBuffer );
		m_areaTex->initialise();

		auto searchTexBuffer = PxBufferBase::create( Size{ SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT }
			, PixelFormat::eL8
			, searchTexBytes
			, PixelFormat::eL8 );
		image.format = renderer::Format::eR8_UNORM;
		m_searchTex = std::make_shared< castor3d::TextureLayout >( renderSystem
			, image
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_searchTex->getDefaultImage().initialiseSource( searchTexBuffer );
		m_searchTex->initialise();

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = m_edgeDetectionView.getFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.attachments[1].format = renderer::Format::eS8_UINT;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eLoad;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader = doBlendingWeightCalculationVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		m_pixelShader = doBlendingWeightCalculationFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_vertexShader.getSource() );
		stages[1].module->loadShader( m_pixelShader.getSource() );

		renderer::DepthStencilState dsstate
		{
			0u,
			false,
			false,
			renderer::CompareOp::eLess,
			false,
			true,
			{
				renderer::StencilOp::eKeep,
				renderer::StencilOp::eKeep,
				renderer::StencilOp::eKeep,
				renderer::CompareOp::eEqual,
				0x00000000u,
				0xFFFFFFFFu,
				0x00000001u,
			},
		};
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		setLayoutBindings.emplace_back( 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );

		createPipeline( size
			, castor::Position{}
			, stages
			, m_edgeDetectionView
			, *m_renderPass
			, setLayoutBindings
			, {}
			, dsstate );
		m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, renderer::Format::eR8G8B8A8_UNORM
			, depthView );
	}

	castor3d::CommandsSemaphore BlendingWeightCalculation::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		castor3d::CommandsSemaphore blendingWeightCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & blendingWeightCmd = *blendingWeightCommands.commandBuffer;

		if ( blendingWeightCmd.begin() )
		{
			timer.beginPass( blendingWeightCmd, passIndex );
			// Put edge detection image in shader input layout.
			blendingWeightCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_edgeDetectionView.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

			blendingWeightCmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { renderer::ClearColorValue{}, renderer::DepthStencilClearValue{ 1.0f, 0 } }
				, renderer::SubpassContents::eInline );
			registerFrame( blendingWeightCmd );
			blendingWeightCmd.endRenderPass();
			timer.endPass( blendingWeightCmd, passIndex );
			blendingWeightCmd.end();
		}

		return std::move( blendingWeightCommands );
	}

	void BlendingWeightCalculation::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "BlendingWeightCalculation" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "BlendingWeightCalculation" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	void BlendingWeightCalculation::update( castor::Point4i const & subsampleIndices )
	{
		auto & data = m_ubo->getData();
		data = subsampleIndices;
		m_ubo->upload();
	}

	void BlendingWeightCalculation::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, *m_ubo );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_areaTex->getDefaultView()
			, m_sampler->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 2u )
			, m_searchTex->getDefaultView()
			, m_sampler->getSampler() );
	}

	//*********************************************************************************************
}

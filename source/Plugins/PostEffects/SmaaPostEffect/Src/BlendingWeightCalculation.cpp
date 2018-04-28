#include "BlendingWeightCalculation.hpp"

#include "SearchTex.h"
#include "AreaTex.h"

#include <Engine.hpp>

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

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doBlendingWeightCalculationVP( castor3d::RenderSystem & renderSystem
			, Point2f const & pixelSize
			, int maxSearchSteps )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto c3d_maxSearchSteps = writer.declConstant( constants::MaxSearchSteps
				, Int( maxSearchSteps ) );

			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_pixcoord = writer.declOutput< Vec2 >( cuT( "vtx_pixcoord" ), 1u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( cuT( "vtx_offset" ), 2u, 3u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;
					vtx_pixcoord = vtx_texture / c3d_pixelSize;

					// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
					vtx_offset[0] = vec4( vtx_texture, vtx_texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -0.25_f, -0.125, 1.25, -0.125 );
					vtx_offset[1] = vec4( vtx_texture, vtx_texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -0.125_f, -0.25, -0.125, 1.25 );

					// And these for the searches, they indicate the ends of the loops:
					vtx_offset[2] = vec4( vtx_offset[0].xz()
						, vtx_offset[1].yw() )
							+ vec4( -2.0_f, 2.0, -2.0, 2.0 ) * vec4( c3d_pixelSize, c3d_pixelSize ) * writer.cast< Float >( c3d_maxSearchSteps );
				} );
			return writer.finalise();
		}

		glsl::Shader doBlendingWeightCalculationFP( castor3d::RenderSystem & renderSystem
			, Point2f const & pixelSize
			, int cornerRounding
			, int maxSearchStepsDiag )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto c3d_maxSearchStepsDiag = writer.declConstant( constants::MaxSearchStepsDiag
				, Int( maxSearchStepsDiag ) );
			auto c3d_cornerRounding = writer.declConstant( constants::CornerRounding
				, Int( cornerRounding ) );
			auto c3d_areaTexPixelSize = writer.declConstant( constants::AreaTexPixelSize
				, vec2( 1.0_f / 160.0_f, 1.0_f / 560.0_f ) );
			auto c3d_areaTexSubtexSize = writer.declConstant( constants::AreaTexSubtexSize
				, 1.0_f / 7.0_f );
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

			//-----------------------------------------------------------------------------
			// Diagonal Search Functions

			/**
			* These functions allows to perform diagonal pattern searches.
			*/
			auto searchDiag1 = writer.implementFunction< Float >( cuT( "searchDiag1" )
				, [&]( Vec2 texcoord
					, Vec2 const & dir
					, Float const & c )
				{

					texcoord += dir * c3d_pixelSize;
					auto e = writer.declLocale( cuT( "e" )
						, vec2( 0.0_f ) );
					auto index = writer.declLocale< Float >( cuT( "i" ) );

					FOR( writer, Float, i, 0.0, "i < float( c3d_maxSearchStepsDiag )", "i++" )
					{
						e.rg() = texture( c3d_edgesTex, texcoord, 0.0_f ).rg();

						IF( writer, dot( e, vec2( 1.0_f ) ) < 1.9_f )
						{
							writer << "break" << endi << endl;
						}
						FI;

						texcoord += dir * c3d_pixelSize;
						index += 1.0_f;
					}
					ROF;

					writer.returnStmt( index + writer.cast< Float >( e.g() > 0.9_f ) * c );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InVec2{ &writer, cuT( "dir" ) }
				, InFloat{ &writer, cuT( "c" ) } );

			auto searchDiag2 = writer.implementFunction< Float >( cuT( "searchDiag2" )
				, [&]( Vec2 texcoord
					, Vec2 const & dir
					, Float const & c )
				{
					texcoord += dir * c3d_pixelSize;
					auto e = writer.declLocale( cuT( "e" )
						, vec2( 0.0_f ) );
					auto index = writer.declLocale< Float >( cuT( "i" ) );

					FOR( writer, Float, i, 0.0, "i < float( c3d_maxSearchStepsDiag )", "i++" )
					{
						e.g() = texture( c3d_edgesTex, texcoord, 0.0_f ).g();
						e.r() = textureLodOffset( c3d_edgesTex, texcoord, 0.0_f, ivec2( 1_i, 0 ) ).r();

						IF( writer, dot( e, vec2( 1.0_f ) ) < 1.9_f )
						{
							writer << "break" << endi << endl;
						}
						FI;

						texcoord += dir * c3d_pixelSize;
						index += 1.0_f;
					}
					ROF;

					writer.returnStmt( index + writer.cast< Float >( e.g() > 0.9_f ) * c );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InVec2{ &writer, cuT( "dir" ) }
				, InFloat{ &writer, cuT( "c" ) } );

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
						, writer.cast< Float >( c3d_areaTexMaxDistanceDiag ) * e + dist );

					// We do a scale and bias for mapping to texel space:
					texcoord = c3d_areaTexPixelSize * texcoord + writer.paren( 0.5_f * c3d_areaTexPixelSize );

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
				, [&]( Vec2 const & e )
				{
					auto weights = writer.declLocale( cuT( "weights" )
						, vec2( 0.0_f ) );

					auto d = writer.declLocale< Vec2 >( cuT( "d" ) );
					d.x() = writer.ternary( e.r() > 0.0f
						, searchDiag1( vtx_texture
							, vec2( -1.0_f, 1.0 )
							, 1.0_f )
						, 0.0_f );
					d.y() = searchDiag1( vtx_texture
						, vec2( 1.0_f, -1.0 )
						, 0.0_f );

					IF( writer, d.r() + d.g() > 2.0_f )
					{ // d.r + d.g + 1 > 3
						auto coords = writer.declLocale( cuT( "coords" )
							, fma( vec4( -d.r(), d.r(), d.g(), -d.g() )
								, vec4( c3d_pixelSize, c3d_pixelSize )
								, vec4( vtx_texture, vtx_texture ) ) );

						auto c = writer.declLocale< Vec4 >( cuT( "c" ) );
						c.x() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( -1_i, 0 ) ).g();
						c.y() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( 0_i, 0 ) ).r();
						c.z() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 0 ) ).g();
						c.w() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, -1 ) ).r();
						auto e = writer.declLocale( cuT( "e" )
							, 2.0_f * c.xz() + c.yw() );
						auto t = writer.declLocale( cuT( "t" )
							, writer.cast< Float >( c3d_maxSearchStepsDiag ) - 1.0_f );
						e *= step( d.rg(), vec2( t, t ) );

						weights += areaDiag( d, e, writer.cast< Float >( c3d_subsampleIndices.z() ) );
					}
					FI;

					d.x() = searchDiag2( vtx_texture, vec2( -1.0_f, -1.0 ), 0.0_f );
					auto right = writer.declLocale( cuT( "right" )
						, textureLodOffset( c3d_edgesTex, vtx_texture, 0.0_f, ivec2( 1_i, 0 ) ).r() );
					d.y() = writer.ternary( right > 0.0_f
						, searchDiag2( vtx_texture
							, vec2( 1.0_f, 1.0 )
							, 1.0_f )
						, 0.0_f );

					IF( writer, d.r() + d.g() > 2.0_f )
					{ // d.r + d.g + 1 > 3
						auto coords = writer.declLocale( cuT( "coords" )
							, fma( vec4( -d.r(), -d.r(), d.g(), d.g() )
								, vec4( c3d_pixelSize, c3d_pixelSize )
								, vec4( vtx_texture, vtx_texture ) ) );

						auto c = writer.declLocale< Vec4 >( cuT( "c" ) );
						c.x() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( -1_i, 0 ) ).g();
						c.y() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( 0_i, -1 ) ).r();
						c.zw() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 0 ) ).gr();
						auto e = writer.declLocale( cuT( "e" )
							, 2.0_f * c.xz() + c.yw() );
						auto t = writer.declLocale( cuT( "t" )
							, writer.cast< Float >( c3d_maxSearchStepsDiag ) - 1.0_f );
						e *= step( d.rg(), vec2( t, t ) );

						weights += areaDiag( d, e, writer.cast< Float >( c3d_subsampleIndices.w() ) ).gr();
					}
					FI;

					writer.returnStmt( weights );
				}
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
				, [&]( Vec2 e
					, Float const & bias
					, Float const & scale )
				{
					// Not required if searchTex accesses are set to point:
					// float2 SEARCH_TEX_PIXEL_SIZE = 1.0 / float2(66.0, 33.0);
					// e = float2(bias, 0.0) + 0.5 * SEARCH_TEX_PIXEL_SIZE + 
					//     e * float2(scale, 1.0) * float2(64.0, 32.0) * SEARCH_TEX_PIXEL_SIZE;
					e.r() = bias + e.r() * scale;
					writer.returnStmt( 255.0_f * texture( c3d_searchTex, e, 0.0_f ).r() );
				}
				, InVec2{ &writer, cuT( "e" ) }
				, InFloat{ &writer, cuT( "bias" ) }
				, InFloat{ &writer, cuT( "scale" ) } );

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
						texcoord -= vec2( 2.0_f, 0.0 ) * c3d_pixelSize;
					}
					ELIHW;

					// We correct the previous (-0.25, -0.125) offset we applied:
					texcoord.x() += 0.25 * c3d_pixelSize.x();

					// The searches are bias by 1, so adjust the coords accordingly:
					texcoord.x ()+= c3d_pixelSize.x();

					// Disambiguate the length added by the last step:
					texcoord.x() += 2.0 * c3d_pixelSize.x(); // Undo last step
					texcoord.x() -= c3d_pixelSize.x() * searchLength( e, 0.0_f, 0.5_f );

					writer.returnStmt( texcoord.x() );
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
						texcoord += vec2( 2.0_f, 0.0 ) * c3d_pixelSize;
					}
					ELIHW;

					texcoord.x() -= 0.25 * c3d_pixelSize.x();
					texcoord.x() -= c3d_pixelSize.x();
					texcoord.x() -= 2.0 * c3d_pixelSize.x();
					texcoord.x() += c3d_pixelSize.x() * searchLength( e, 0.5_f, 0.5_f );
					writer.returnStmt( texcoord.x() );
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
						texcoord -= vec2( 0.0_f, 2.0 ) * c3d_pixelSize;
					}
					ELIHW;

					texcoord.y() += 0.25 * c3d_pixelSize.y();
					texcoord.y() += c3d_pixelSize.y();
					texcoord.y() += 2.0 * c3d_pixelSize.y();
					texcoord.y() -= c3d_pixelSize.y() * searchLength( e.gr(), 0.5_f, 0.5_f );
					writer.returnStmt( texcoord.y() );
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
						texcoord += vec2( 0.0_f, 2.0 ) * c3d_pixelSize;
					}
					ELIHW;

					texcoord.y() -= 0.25 * c3d_pixelSize.y();
					texcoord.y() -= c3d_pixelSize.y();
					texcoord.y() -= 2.0 * c3d_pixelSize.y();
					texcoord.y() += c3d_pixelSize.y() * searchLength( e.gr(), 0.5_f, 0.5_f );
					writer.returnStmt( texcoord.y() );
				}
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InFloat{ &writer, cuT( "end" ) } );

			//-----------------------------------------------------------------------------
			// Corner Detection Functions
			
			auto detectHorizontalCornerPattern = writer.implementFunction< Void >( cuT( "detectHorizontalCornerPattern" )
				, [&]( Vec2 weights
					, Vec2 const & texcoord
					, Vec2 const & d )
				{
					if ( cornerRounding < 100 )
					{
						auto coords = writer.declLocale( cuT( "coords" )
							, fma( vec4( d.x(), 0.0, d.y(), 0.0 )
								, vec4( c3d_pixelSize, c3d_pixelSize )
								, vec4( texcoord, texcoord) ) );
						auto e = writer.declLocale< Vec2 >( cuT( "e" ) );
						e.r() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( 0_i, 1 ) ).r();
						auto left = writer.declLocale( cuT( "left" )
							, abs( d.x() ) < abs( d.y() ) );
						e.g() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( 0_i, -2 ) ).r();

						IF( writer, left )
						{
							weights *= clamp( vec2( writer.cast< Float >( c3d_cornerRounding ) / 100.0 + 1.0 ) - e
								, vec2( 0.0_f )
								, vec2( 1.0_f ) );
						}
						FI;

						e.r() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 1 ) ).r();
						e.g() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, -2 ) ).r();

						IF( writer, "!left" )
						{
							weights *= clamp( vec2( writer.cast< Float >( c3d_cornerRounding ) / 100.0 + 1.0 ) - e
								, vec2( 0.0_f )
								, vec2( 1.0_f ) );
						}
						FI;
					}
				}
				, InOutVec2{ &writer, cuT( "weights" ) }
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InVec2{ &writer, cuT( "d" ) } );
			
			auto detectVerticalCornerPattern = writer.implementFunction< Void >( cuT( "detectVerticalCornerPattern" )
				, [&]( Vec2 weights
					, Vec2 const & texcoord
					, Vec2 const & d )
				{
					if ( cornerRounding < 100 )
					{
						auto coords = writer.declLocale( cuT( "coords" )
							, fma( vec4( 0.0_f, d.x(), 0.0, d.y() )
								, vec4( c3d_pixelSize, c3d_pixelSize )
								, vec4( texcoord, texcoord ) ) );
						auto e = writer.declLocale< Vec2 >( cuT( "e" ) );
						e.r() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( 1_i, 0 ) ).g();
						auto left = writer.declLocale( cuT( "left" )
							, abs( d.x() ) < abs( d.y() ) );
						e.g() = textureLodOffset( c3d_edgesTex, coords.xy(), 0.0_f, ivec2( -2_i, 0 ) ).g();

						IF( writer, left )
						{
							weights *= clamp( vec2( writer.cast< Float >( c3d_cornerRounding ) / 100.0 + 1.0 ) - e
								, vec2( 0.0_f )
								, vec2( 1.0_f ) );
						}
						FI;

						e.r() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( 1_i, 1 ) ).g();
						e.g() = textureLodOffset( c3d_edgesTex, coords.zw(), 0.0_f, ivec2( -2_i, 1 ) ).g();

						IF( writer, "!left" )
						{
							weights *= clamp( vec2( writer.cast< Float >( c3d_cornerRounding ) / 100.0 + 1.0 ) - e
								, vec2( 0.0_f )
								, vec2( 1.0_f ) );
						}
						FI;
					}
				}
				, InOutVec2{ &writer, cuT( "weights" ) }
				, InVec2{ &writer, cuT( "texcoord" ) }
				, InVec2{ &writer, cuT( "d" ) } );

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
						, fma( vec2( writer.cast< Float >( c3d_areaTexMaxDistance ) ), round( 4.0_f * vec2( e1, e2 ) ), dist ) );

					// We do a scale and bias for mapping to texel space:
					texcoord = fma( c3d_areaTexPixelSize, texcoord, 0.5_f * c3d_areaTexPixelSize );

					// Move to proper place, according to the subpixel offset:
					texcoord.y() = fma( c3d_areaTexSubtexSize, offset, texcoord.y() );

					// Do it!
					writer.returnStmt( texture( c3d_areaTex, texcoord, 0.0_f ).rg() );
				}
				, InVec2{ &writer, cuT( "dist" ) }
				, InFloat{ &writer, cuT( "e1" ) }
				, InFloat{ &writer, cuT( "e2" ) }
				, InFloat{ &writer, cuT( "offset" ) } );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto weights = writer.declLocale( cuT( "weights" )
						, vec4( 0.0_f, 0.0, 0.0, 0.0 ) );

					auto e = writer.declLocale( cuT( "e" )
						, texture( c3d_edgesTex, vtx_texture ).rg() );

					IF( writer, e.g() > 0.0 )
					{ // Edge at north
						if ( maxSearchStepsDiag > 0 )
						{
							// Diagonals have both north and west edges, so searching for them in
							// one of the boundaries is enough.
							weights.rg() = calculateDiagWeights( e );

							// We give priority to diagonals, so if we find a diagonal we skip 
							// horizontal/vertical processing.
							IF( writer, dot( weights.rg(), vec2( 1.0_f, 1.0 ) ) == 0.0_f )
							{
								auto d = writer.declLocale< Vec2 >( cuT( "d" ) );

								// Find the distance to the left:
								auto coords = writer.declLocale< Vec2 >( cuT( "coords" ) );
								coords.x() = searchXLeft( vtx_offset[0].xy(), vtx_offset[2].x() );
								coords.y() = vtx_offset[1].y(); // vtx_offset[1].y() = vtx_texture.y() - 0.25_f * c3d_pixelSize.y() (@CROSSING_OFFSET)
								d.x() = coords.x();

								// Now fetch the left crossing edges, two at a time using bilinear
								// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
								// discern what value each edge has:
								auto e1 = writer.declLocale( cuT( "e1" )
									, texture( c3d_edgesTex, coords, 0.0_f ).r() );

								// Find the distance to the right:
								coords.x() = searchXRight( vtx_offset[0].zw(), vtx_offset[2].y() );
								d.y() = coords.x();

								// We want the distances to be in pixel units (doing this here allow to
								// better interleave arithmetic and memory accesses):
								d = d / c3d_pixelSize.x() - vtx_pixcoord.x();

								// SMAAArea below needs a sqrt, as the areas texture is compressed 
								// quadratically:
								auto sqrt_d = writer.declLocale( cuT( "sqrt_d" )
									, sqrt( abs( d ) ) );

								// Fetch the right crossing edges:
								auto e2 = writer.declLocale( cuT( "e2" )
									, textureLodOffset( c3d_edgesTex, coords, 0.0_f, ivec2( 1_i, 0 ) ).r() );

								// Ok, we know how this pattern looks like, now it is time for getting
								// the actual area:
								weights.rg() = area( sqrt_d, e1, e2, writer.cast< Float >( c3d_subsampleIndices.y() ) );

								// Fix corners:
								detectHorizontalCornerPattern( weights.rg(), vtx_texture, d );
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
							auto coords = writer.declLocale< Vec2 >( cuT( "coords" ) );
							coords.x() = searchXLeft( vtx_offset[0].xy(), vtx_offset[2].x() );
							coords.y() = vtx_offset[1].y(); // vtx_offset[1].y() = vtx_texture.y() - 0.25_f * c3d_pixelSize.y() (@CROSSING_OFFSET)
							d.x() = coords.x();

							// Now fetch the left crossing edges, two at a time using bilinear
							// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
							// discern what value each edge has:
							auto e1 = writer.declLocale( cuT( "e1" )
								, texture( c3d_edgesTex, coords, 0.0_f ).r() );

							// Find the distance to the right:
							coords.x() = searchXRight( vtx_offset[0].zw(), vtx_offset[2].y() );
							d.y() = coords.x();

							// We want the distances to be in pixel units (doing this here allow to
							// better interleave arithmetic and memory accesses):
							d = d / c3d_pixelSize.x() - vtx_pixcoord.x();

							// SMAAArea below needs a sqrt, as the areas texture is compressed 
							// quadratically:
							auto sqrt_d = writer.declLocale( cuT( "sqrt_d" )
								, sqrt( abs( d ) ) );

							// Fetch the right crossing edges:
							auto e2 = writer.declLocale( cuT( "e2" )
								, textureLodOffset( c3d_edgesTex, coords, 0.0_f, ivec2( 1_i, 0 ) ).r() );

							// Ok, we know how this pattern looks like, now it is time for getting
							// the actual area:
							weights.rg() = area( sqrt_d, e1, e2, writer.cast< Float >( c3d_subsampleIndices.y() ) );

							// Fix corners:
							detectHorizontalCornerPattern( weights.rg(), vtx_texture, d );
						}
					}
					FI;

					IF( writer, e.r() > 0.0_f )
					{ // Edge at west
						auto d = writer.declLocale< Vec2 >( cuT( "d" ) );

						// Find the distance to the top:
						auto coords = writer.declLocale< Vec2 >( cuT( "coords" ) );
						coords.y() = searchYUp( vtx_offset[1].xy(), vtx_offset[2].z() );
						coords.x() = vtx_offset[0].x(); // vtx_offset[1].x() = vtx_texture.x() - 0.25 * c3d_pixelSize.x();
						d.x() = coords.y();

						// Fetch the top crossing edges:
						auto e1 = writer.declLocale( cuT( "e1" )
							, texture( c3d_edgesTex, coords, 0.0_f ).g() );

						// Find the distance to the bottom:
						coords.y() = searchYDown( vtx_offset[1].zw(), vtx_offset[2].w() );
						d.y() = coords.y();

						// We want the distances to be in pixel units:
						d = d / c3d_pixelSize.y() - vtx_pixcoord.y();

						// SMAAArea below needs a sqrt, as the areas texture is compressed 
						// quadratically:
						auto sqrt_d = writer.declLocale( cuT( "sqrt_d" )
							, sqrt( abs( d ) ) );

						// Fetch the bottom crossing edges:
						auto e2 = writer.declLocale( cuT( "e2" )
							, textureLodOffset( c3d_edgesTex, coords, 0.0_f, ivec2( 0_i, 1 ) ).g() );

						// Get the area for this direction:
						weights.ba() = area( sqrt_d, e1, e2, writer.cast< Float >( c3d_subsampleIndices.x() ) );

						// Fix corners:
						detectVerticalCornerPattern( weights.ba(), vtx_texture, d );
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
		, int maxSearchSteps
		, int cornerRounding
		, int maxSearchStepsDiag )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_surface{ *renderTarget.getEngine() }
		, m_edgeDetectionView{ edgeDetectionView }
	{
		renderer::Extent2D size{ m_edgeDetectionView.getTexture().getDimensions().width
			, m_edgeDetectionView.getTexture().getDimensions().height };
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

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
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

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
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point2f{ 1.0f / size.width, 1.0f / size.height };
		auto vertex = doBlendingWeightCalculationVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, maxSearchSteps );
		auto fragment = doBlendingWeightCalculationFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, cornerRounding
			, maxSearchStepsDiag );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vertex.getSource() );
		stages[1].module->loadShader( fragment.getSource() );

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
			, sampler
			, m_edgeDetectionView.getFormat()
			, depthView );
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

#include "SmaaPostEffect.hpp"

#include "SearchTex.h"
#include "AreaTex.h"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <FrameBuffer/BackBuffers.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/RasteriserState.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doGetEdgeDetectionVP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto position = writer.declAttribute< Vec4 >( castor3d::ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( castor3d::ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_offset = writer.declOutput< Vec4 >( cuT( "vtx_offset" ), 3u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_projection * position;
					vtx_texture = texture;

					vtx_offset[0] = vec4( texture, texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -1.0_f, 0.0, 0.0, -1.0 );
					vtx_offset[1] = vec4( texture, texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( 1.0_f, 0.0, 0.0, 1.0 );
					vtx_offset[2] = vec4( texture, texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -2.0_f, 0.0, 0.0, -2.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetEdgeDetectionFP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize
			, float threshold
			, bool predication
			, float predicationThreshold
			, float predicationScale
			, float predicationStrength )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( threshold ) );
			auto c3d_predicationThreshold = writer.declConstant( constants::PredicationThreshold
				, Float( predicationThreshold ) );
			auto c3d_predicationScale = writer.declConstant( constants::PredicationScale
				, Float( predicationScale ) );
			auto c3d_predicationStrength = writer.declConstant( constants::PredicationStrength
				, Float( predicationStrength ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_offset = writer.declInput< Vec4 >( cuT( "vtx_offset" ), 3u );
			auto c3d_colourTex = writer.declSampler< Sampler2D >( cuT( "c3d_colourTex" ), castor3d::MinTextureIndex + 0u );
			auto c3d_predicationTex = writer.declSampler< Sampler2D >( cuT( "c3d_predicationTex" ), castor3d::MinTextureIndex + 1u, predication );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			/**
			* Gathers current pixel, and the top-left neighbors.
			*/
			Function< Vec2 > calculatePredicatedThreshold;

			if ( predication )
			{
				auto gatherNeighbours = writer.implementFunction< Vec3 >( cuT( "gatherNeighbours" )
					, [&]()
					{
						writer.returnStmt( textureGather( c3d_predicationTex
							, vtx_texture + c3d_pixelSize * vec2( -0.5_f, -0.5 ) ).grb() );
					} );

				/**
				* Adjusts the threshold by means of predication.
				*/
				calculatePredicatedThreshold = writer.implementFunction< Vec2 >( cuT( "calculatePredicatedThreshold" )
					, [&]()
					{
						auto neighbours = writer.declLocale( cuT( "neighbours" )
							, gatherNeighbours() );
						auto delta = writer.declLocale( cuT( "delta" )
							, abs( neighbours.xx() - neighbours.yz() ) );
						auto edges = writer.declLocale( cuT( "edges" )
							, step( vec2( c3d_predicationThreshold ), delta ) );
						writer.returnStmt( c3d_predicationScale
							* c3d_threshold
							* writer.paren( vec2( 1.0_f ) - c3d_predicationStrength * edges ) );
					} );
			}

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					// Calculate the threshold:
					if ( predication )
					{
						auto threshold = writer.declLocale( cuT( "threshold" )
							, calculatePredicatedThreshold() );
					}
					else
					{
						auto threshold = writer.declLocale( cuT( "threshold" )
							, vec2( c3d_threshold ) );
					}

					auto threshold = writer.getBuiltin< Vec2 >( cuT( "threshold" ) );

					// Calculate lumas:
					auto weights = writer.declLocale( cuT( "weights" )
						, vec3( 0.2126_f, 0.7152, 0.0722 ) );
					auto L = writer.declLocale( cuT( "L" )
						, dot( texture( c3d_colourTex, vtx_texture ).rgb(), weights ) );
					auto Lleft = writer.declLocale( cuT( "Lleft" )
						, dot( texture( c3d_colourTex, vtx_offset[0].xy() ).rgb(), weights ) );
					auto Ltop = writer.declLocale( cuT( "Ltop" )
						, dot( texture( c3d_colourTex, vtx_offset[0].zw() ).rgb(), weights ) );

					// We do the usual threshold:
					auto delta = writer.declLocale< Vec4 >( cuT( "delta" ) );
					delta.xy() = abs( vec2( L ) - vec2( Lleft, Ltop ) );
					auto edges = writer.declLocale( cuT( "edges" )
						, step( threshold, delta.xy() ) );

					// Then discard if there is no edge:
					IF( writer, dot( edges, vec2( 1.0_f ) ) == 0.0 )
					{
						writer.discard();
					}
					FI;

					// Calculate right and bottom deltas:
					auto Lright = writer.declLocale( cuT( "Lright" )
						, dot( texture( c3d_colourTex, vtx_offset[1].xy() ).rgb(), weights ) );
					auto Lbottom = writer.declLocale( cuT( "Lbottom" )
						, dot( texture( c3d_colourTex, vtx_offset[1].zw() ).rgb(), weights ) );
					delta.zw() = abs( vec2( L ) - vec2( Lright, Lbottom ) );

					// Calculate the maximum delta in the direct neighborhood:
					auto maxDelta = writer.declLocale( cuT( "maxDelta" )
						, max( delta.xy(), delta.zw() ) );
					maxDelta = max( maxDelta.xx(), maxDelta.yy() );

					// Calculate left-left and top-top deltas:
					auto Lleftleft = writer.declLocale( cuT( "Lleftleft" )
						, dot( texture( c3d_colourTex, vtx_offset[2].xy() ).rgb(), weights ) );
					auto Ltoptop = writer.declLocale( cuT( "Ltoptop" )
						, dot( texture( c3d_colourTex, vtx_offset[2].zw() ).rgb(), weights ) );
					delta.zw() = abs( vec2( Lleft, Ltop ) - vec2( Lleftleft, Ltoptop ) );

					// Calculate the final maximum delta:
					maxDelta = max( maxDelta.xy(), delta.zw() );

					/**
					* Each edge with a delta in luma of less than 50% of the maximum luma
					* surrounding this pixel is discarded. This allows to eliminate spurious
					* crossing edges, and is based on the fact that, if there is too much
					* contrast in a direction, that will hide contrast in the other
					* neighbors.
					* This is done after the discard intentionally as this situation doesn't
					* happen too frequently (but it's important to do as it prevents some
					* edges from going undetected).
					*/
					edges.xy() *= step( 0.5_f * maxDelta, delta.xy() );

					pxl_fragColour = vec4( edges, 0.0_f, 0.0_f );
				} );
			return writer.finalise();
		}

		glsl::Shader doBlendingWeightCalculationVP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize
			, int maxSearchSteps )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto c3d_maxSearchSteps = writer.declConstant( constants::MaxSearchSteps
				, Int( maxSearchSteps ) );
			auto position = writer.declAttribute< Vec4 >( castor3d::ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( castor3d::ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_pixcoord = writer.declOutput< Vec2 >( cuT( "vtx_pixcoord" ) );
			auto vtx_offset = writer.declOutput< Vec4 >( cuT( "vtx_offset" ), 3u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_projection * position;
					vtx_texture = texture;
					vtx_pixcoord = texture / c3d_pixelSize;

					// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
					vtx_offset[0] = vec4( texture, texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -0.25_f, -0.125, 1.25, -0.125 );
					vtx_offset[1] = vec4( texture, texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -0.125_f, -0.25, -0.125, 1.25 );

					// And these for the searches, they indicate the ends of the loops:
					vtx_offset[2] = vec4( vtx_offset[0].xz()
						, vtx_offset[1].yw() )
							+ vec4( -2.0_f, 2.0, -2.0, 2.0 ) * vec4( c3d_pixelSize, c3d_pixelSize ) * writer.cast< Float >( c3d_maxSearchSteps );
				} );
			return writer.finalise();
		}

		glsl::Shader doBlendingWeightCalculationFP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize
			, int cornerRounding
			, int maxSearchStepsDiag )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_pixcoord = writer.declInput< Vec2 >( cuT( "vtx_pixcoord" ) );
			auto vtx_offset = writer.declInput< Vec4 >( cuT( "vtx_offset" ), 3u );
			auto c3d_edgesTex = writer.declSampler< Sampler2D >( cuT( "c3d_edgesTex" ), castor3d::MinTextureIndex + 0u );
			auto c3d_areaTex = writer.declSampler< Sampler2D >( cuT( "c3d_areaTex" ), castor3d::MinTextureIndex + 1u );
			auto c3d_searchTex = writer.declSampler< Sampler2D >( cuT( "c3d_searchTex" ), castor3d::MinTextureIndex + 2u );
			auto c3d_subsampleIndices = writer.declUniform< IVec4 >( constants::SubsampleIndices );
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
			auto c3d_searchTexSize = writer.declConstant( cuT( "c3d_searchTexSize" )
				, vec2( 66.0_f, 33.0_f ) );
			auto c3d_searchTexPackedSize = writer.declConstant( cuT( "c3d_searchTexPackedSize" )
				, vec2( 64.0_f, 16.0_f ) );
			auto c3d_cornerRoundingNorm = writer.declConstant( cuT( "c3d_cornerRoundingNorm" )
				, writer.cast< Float >( c3d_cornerRounding ) / 100.0_f );

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
							IF( writer, dot( weights.rg(), vec2( 1.0_f, 1.0 ) ) == vec2( 0.0_f ) )
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

		glsl::Shader doGetNeighbourhoodBlendingVP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto position = writer.declAttribute< Vec4 >( castor3d::ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( castor3d::ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_offset = writer.declOutput< Vec4 >( cuT( "vtx_offset" ), 2u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_projection * position;
					vtx_texture = texture;

					vtx_offset[0] = vec4( texture, texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -1.0_f, 0.0, 0.0, -1.0 );
					vtx_offset[1] = vec4( texture, texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( 1.0_f, 0.0, 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetNeighbourhoodBlendingFP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize
			, bool reprojection )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_offset = writer.declInput< Vec4 >( cuT( "vtx_offset" ), 2u );
			auto c3d_colourTex = writer.declSampler< Sampler2D >( cuT( "c3d_colourTex" ), castor3d::MinTextureIndex + 0u );
			auto c3d_blendTex = writer.declSampler< Sampler2D >( cuT( "c3d_blendTex" ), castor3d::MinTextureIndex + 1u );
			auto c3d_velocityTex = writer.declSampler< Sampler2D >( cuT( "c3d_velocityTex" ), castor3d::MinTextureIndex + 2u, reprojection );
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			auto movc2 = writer.implementFunction< Void >( cuT( "movc2" )
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
				, InBVec2{ &writer, cuT( "cond" ) }
				, InOutVec2{ &writer, cuT( "variable" ) }
				, InVec2{ &writer, cuT( "value" ) } );

			auto movc4 = writer.implementFunction< Void >( cuT( "movc2" )
				, [&]( BVec4 const & cond
					, Vec4 variable
					, Vec4 const & value )
				{
					movc2( cond.xy(), variable.xy(), value.xy() );
					movc2( cond.zw(), variable.zw(), value.zw() );
				}
				, InBVec4{ &writer, cuT( "cond" ) }
				, InOutVec4{ &writer, cuT( "variable" ) }
				, InVec4{ &writer, cuT( "value" ) } );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto texcoord = writer.declLocale( cuT( "texcoord" )
						, vtx_texture );
					// Fetch the blending weights for current pixel:
					auto a = writer.declLocale< Vec4 >( cuT( "a" ) );
					a.xz() = texture( c3d_blendTex, texcoord ).xz();
					a.y() = texture( c3d_blendTex, vtx_offset[1].zw() ).g();
					a.w() = texture( c3d_blendTex, vtx_offset[1].xy() ).a();

					// Is there any blending weight with a value greater than 0.0?
					IF( writer, dot( a, vec4( 1.0_f ) ) < 1e-5_f )
					{
						pxl_fragColour = texture( c3d_colourTex, texcoord, 0.0_f );

						if ( reprojection )
						{
							auto velocity = writer.declLocale( cuT( "velocity" )
								, texture( c3d_velocityTex, texcoord ).rg() );

							// Pack velocity into the alpha channel:
							pxl_fragColour.a() = sqrt( 5.0_f * length( velocity ) );
						}
					}
					ELSE
					{
						auto color = writer.declLocale( cuT( "color" )
							, vec4( 0.0_f, 0.0, 0.0, 0.0 ) );

						// Up to 4 lines can be crossing a pixel (one through each edge). We
						// favor blending by choosing the line with the maximum weight for each
						// direction:
						auto offset = writer.declLocale< Vec2 >( cuT( "offset" ) );
						offset.x() = writer.ternary( a.a() > a.b()
							, a.a()
							, -a.b() ); // left vs. right 
						offset.y() = writer.ternary( a.g() > a.r()
							, a.g()
							, -a.r() ); // top vs. bottom

						// Then we go in the direction that has the maximum weight:
						IF( writer, abs( offset.x() ) > abs( offset.y() ) ) // horizontal vs. vertical
						{
							offset.y() = 0.0_f;
						}
						ELSE
						{
							offset.x() = 0.0_f;
						}
						FI;

						if ( reprojection )
						{
							// Fetch the opposite color and lerp by hand:
							auto C = writer.declLocale( cuT( "C" )
								, texture( c3d_colourTex, texcoord, 0.0_f ) );
							texcoord += sign( offset ) * c3d_pixelSize;
							auto Cop = writer.declLocale( cuT( "Cop" )
								, texture( c3d_colourTex, texcoord, 0.0_f ) );
							auto s = writer.declLocale( cuT( "s" )
								, writer.ternary( abs( offset.x() ) > abs( offset.y() )
									, abs( offset.x() )
									, abs( offset.y() ) ) );

							// Unpack the velocity values:
							C.a() *= C.a();
							Cop.a() *= Cop.a();

							// Lerp the colors:
							auto Caa = writer.declLocale( cuT( "Caa" )
								, mix( C, Cop, s ) );

							// Unpack velocity and return the resulting value:
							Caa.a() = sqrt( Caa.a() );
							pxl_fragColour = Caa;
						}
						else
						{
							// We exploit bilinear filtering to mix current pixel with the chosen
							// neighbor:
							texcoord += offset * c3d_pixelSize;
							pxl_fragColour = texture( c3d_colourTex, texcoord, 0.0_f );
						}
					}
					FI;
				} );
			return writer.finalise();
		}

		glsl::Shader doGetReprojectVP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto position = writer.declAttribute< Vec4 >( castor3d::ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( castor3d::ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_projection * position;
					vtx_texture = texture;
				} );
			return writer.finalise();
		}

		glsl::Shader doGetReprojectFP( castor3d::RenderSystem * renderSystem
			, Point2f const & pixelSize
			, float reprojectionWeightScale
			, bool reprojection )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto c3d_currentColourTex = writer.declSampler< Sampler2D >( cuT( "c3d_currentColourTex" ), castor3d::MinTextureIndex + 0u );
			auto c3d_previousColourTex = writer.declSampler< Sampler2D >( cuT( "c3d_previousColourTex" ), castor3d::MinTextureIndex + 1u );
			auto c3d_velocityTex = writer.declSampler< Sampler2D >( cuT( "c3d_velocityTex" ), castor3d::MinTextureIndex + 2u, reprojection );
			auto c3d_reprojectionWeightScale = writer.declConstant( cuT( "c3d_reprojectionWeightScale" )
				, Float( reprojectionWeightScale )
				, reprojection );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					if ( reprojection )
					{
						// Velocity is assumed to be calculated for motion blur, so we need to
						// inverse it for reprojection:
						auto velocity = writer.declLocale( cuT( "velocity" )
							, -texture( c3d_velocityTex, vtx_texture ).rg() );

						// Fetch current pixel:
						auto current = writer.declLocale( cuT( "current" )
							, texture( c3d_currentColourTex, vtx_texture ) );

						// Reproject current coordinates and fetch previous pixel:
						auto previous = writer.declLocale( cuT( "previous" )
							, texture( c3d_previousColourTex, vtx_texture + velocity ) );

						// Attenuate the previous pixel if the velocity is different:
						auto delta = writer.declLocale( cuT( "delta" )
							, abs( current.a() * current.a() - previous.a() * previous.a() ) / 5.0 );
						auto weight = writer.declLocale( cuT( "weight" )
							, 0.5_f * clamp( 1.0_f - writer.paren( sqrt( delta ) * c3d_reprojectionWeightScale ), 0.0_f, 1.0_f ) );

						// Blend the pixels according to the calculated weight:
						pxl_fragColour = mix( current, previous, weight );
					}
					else
					{
						// Just blend the pixels:
						auto current = writer.declLocale( cuT( "current" )
							, texture( c3d_currentColourTex, vtx_texture ) );
						auto previous = writer.declLocale( cuT( "previous" )
							, texture( c3d_previousColourTex, vtx_texture ) );
						pxl_fragColour = mix( current, previous, 0.5 );
					}
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "smaa" );
	String PostEffect::Name = cuT( "SMAA PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, renderTarget
			, renderSystem
			, parameters
			, true }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_areaTex{ *renderSystem.getEngine() }
		, m_searchTex{ *renderSystem.getEngine() }
		, m_edgeDetectionSurface{ *renderSystem.getEngine() }
		, m_blendingWeightCalculationSurface{ *renderSystem.getEngine() }
		, m_reprojectSurface{ *renderSystem.getEngine() }
	{
		String preset;

		if ( !parameters.get( cuT( "preset" ), preset ) )
		{
			preset = "high";
		}

		if ( preset == cuT( "low" ) )
		{
			m_smaaThreshold = 0.15f;
			m_smaaMaxSearchSteps = 4;
			m_smaaMaxSearchStepsDiag = 0;
			m_smaaCornerRounding = 100;
		}
		else if ( preset == cuT( "medium" ) )
		{
			m_smaaThreshold = 0.1f;
			m_smaaMaxSearchSteps = 8;
			m_smaaMaxSearchStepsDiag = 0;
			m_smaaCornerRounding = 100;
		}
		else if ( preset == cuT( "high" ) )
		{
			m_smaaThreshold = 0.1f;
			m_smaaMaxSearchSteps = 16;
			m_smaaMaxSearchStepsDiag = 8;
			m_smaaCornerRounding = 25;
		}
		else if ( preset == cuT( "ultra" ) )
		{
			m_smaaThreshold = 0.05f;
			m_smaaMaxSearchSteps = 32;
			m_smaaMaxSearchStepsDiag = 16;
			m_smaaCornerRounding = 25;
		}
		else if ( preset == cuT( "custom" ) )
		{
			parameters.get( cuT( "threshold" ), m_smaaThreshold );
			parameters.get( cuT( "maxSearchSteps" ), m_smaaMaxSearchSteps );
			parameters.get( cuT( "maxSearchStepsDiag" ), m_smaaMaxSearchStepsDiag );
			parameters.get( cuT( "cornerRounding" ), m_smaaCornerRounding );
		}

		String mode;

		if ( parameters.get( cuT( "mode" ), mode ) )
		{
			if ( mode == cuT( "T2X" ) )
			{
				m_mode = Mode::eT2X;
			}
			else if ( mode == cuT( "S2X" ) )
			{
				m_mode = Mode::eS2X;
			}
			else if ( mode == cuT( "4X" ) )
			{
				m_mode = Mode::e4X;
			}
		}

		if ( m_mode == Mode::eT2X )
		{
			parameters.get( cuT( "reprojection" ), m_reprojection );
			parameters.get( cuT( "reprojectionWeightScale" ), m_reprojectionWeightScale );
		}

		doInitialiseSubsampleIndices();
		String name = cuT( "SMAA_Point" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_pointSampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_pointSampler->setInterpolationMode( castor3d::InterpolationFilter::eMin, castor3d::InterpolationMode::eNearest );
			m_pointSampler->setInterpolationMode( castor3d::InterpolationFilter::eMag, castor3d::InterpolationMode::eNearest );
			m_pointSampler->setWrappingMode( castor3d::TextureUVW::eU, castor3d::WrapMode::eClampToBorder );
			m_pointSampler->setWrappingMode( castor3d::TextureUVW::eV, castor3d::WrapMode::eClampToBorder );
			m_pointSampler->setWrappingMode( castor3d::TextureUVW::eW, castor3d::WrapMode::eClampToBorder );
		}
		else
		{
			m_pointSampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		name = cuT( "SMAA_Linear" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_linearSampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_linearSampler->setInterpolationMode( castor3d::InterpolationFilter::eMin, castor3d::InterpolationMode::eLinear );
			m_linearSampler->setInterpolationMode( castor3d::InterpolationFilter::eMag, castor3d::InterpolationMode::eLinear );
			m_linearSampler->setWrappingMode( castor3d::TextureUVW::eU, castor3d::WrapMode::eClampToBorder );
			m_linearSampler->setWrappingMode( castor3d::TextureUVW::eV, castor3d::WrapMode::eClampToBorder );
			m_linearSampler->setWrappingMode( castor3d::TextureUVW::eW, castor3d::WrapMode::eClampToBorder );
		}
		else
		{
			m_linearSampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, parameters );
	}

	bool PostEffect::initialise()
	{
		bool result = false;
		doInitialiseEdgeDetection();
		doInitialiseBlendingWeightCalculation();
		doInitialiseNeighbourhoodBlending();

		if ( m_mode == Mode::eT2X )
		{
			doInitialiseReproject();
		}

		return true;
	}

	void PostEffect::cleanup()
	{
		m_matrixUbo.getUbo().cleanup();

		m_edgeDetectionSurface.cleanup();
		m_depthAttach.reset();
		m_depthBuffer->cleanup();
		m_depthBuffer.reset();
		m_blendingWeightCalculationSurface.cleanup();

		for ( auto & surface : m_neighbourhoodBlendingSurface )
		{
			surface.cleanup();
		}

		if ( m_reprojectPipeline )
		{
			m_reprojectSurface.cleanup();
			m_reprojectPipeline->cleanup();
		}

		m_edgeDetectionPipeline->cleanup();
		m_blendingWeightCalculationPipeline->cleanup();
		m_neighbourhoodBlendingPipeline->cleanup();
		m_areaTex.cleanup();
		m_searchTex.cleanup();
	}

	bool PostEffect::apply( castor3d::FrameBuffer & framebuffer )
	{
		auto prvIndex = m_subsampleIndex;
		auto curIndex = ( m_subsampleIndex + 1 ) % uint32_t( m_jitters.size() );
		auto srgbAttach = framebuffer.getAttachment( castor3d::AttachmentPoint::eColour, 0 );
		REQUIRE( srgbAttach && srgbAttach->getAttachmentType() == castor3d::AttachmentType::eTexture );
		auto & srgbTexture = *std::static_pointer_cast< castor3d::TextureAttachment >( srgbAttach )->getTexture();
		auto & rgbTexture = m_renderTarget.getTechnique()->getResult();
		castor3d::TextureLayoutSPtr result;

		switch ( m_mode )
		{
		case Mode::e1X:
			doMainPass( prvIndex, curIndex, rgbTexture, srgbTexture, nullptr );
			result = m_neighbourhoodBlendingSurface[curIndex].m_colourTexture.getTexture();
			break;

		case Mode::eT2X:
			doMainPass( prvIndex, curIndex, rgbTexture, srgbTexture, &m_renderTarget.getTechnique()->getDepth() );
			doReproject( prvIndex, curIndex, rgbTexture );
			result = m_reprojectSurface.m_colourTexture.getTexture();
			break;

		case Mode::eS2X:
			break;

		case Mode::e4X:
			break;
		}

		if ( result )
		{
			framebuffer.bind( castor3d::FrameBufferTarget::eDraw );
			getRenderSystem()->getCurrentContext()->renderTexture( srgbTexture.getDimensions()
				, *result );
			framebuffer.unbind();
			m_subsampleIndex = curIndex;
			m_renderTarget.setJitter( m_jitters[m_subsampleIndex] );
		}

		return true;
	}

	bool PostEffect::doWriteInto( TextFile & p_file )
	{
		return true;
	}

	void PostEffect::doInitialiseSubsampleIndices()
	{
		switch ( m_mode )
		{
		case Mode::e1X:
			m_smaaSubsampleIndices[0] = Point4i{ 0, 0, 0, 0 };
			m_maxSubsampleIndices = 1u;
			break;

		case Mode::eT2X:
		case Mode::eS2X:
			/***
			* Sample positions (bottom-to-top y axis):
			*   _______
			*  | S1    |  S0:  0.25    -0.25
			*  |       |  S1: -0.25     0.25
			*  |____S0_|
			*/
			m_smaaSubsampleIndices[0] = Point4i{ 1, 1, 1, 0 }; // S0
			m_smaaSubsampleIndices[1] = Point4i{ 2, 2, 2, 0 }; // S1
			m_maxSubsampleIndices = 2u;
			// (it's 1 for the horizontal slot of S0 because horizontal
			//  blending is reversed: positive numbers point to the right)
			break;

		case Mode::e4X:
			/***
			* Sample positions (bottom-to-top y axis):
			*   ________
			*  |  S1    |  S0:  0.3750   -0.1250
			*  |      S0|  S1: -0.1250    0.3750
			*  |S3      |  S2:  0.1250   -0.3750
			*  |____S2__|  S3: -0.3750    0.1250
			*/
			m_smaaSubsampleIndices[0] = Point4i{ 5, 3, 1, 3 }; // S0
			m_smaaSubsampleIndices[1] = Point4i{ 4, 6, 2, 3 }; // S1
			m_smaaSubsampleIndices[2] = Point4i{ 3, 5, 1, 4 }; // S2
			m_smaaSubsampleIndices[3] = Point4i{ 6, 4, 2, 4 }; // S3
			m_maxSubsampleIndices = 4u;
			break;
		}
		switch ( m_mode )
		{
		case Mode::e1X:
		case Mode::eS2X:
			m_jitters.emplace_back( 0.0, 0.0 );
			break;

		case Mode::eT2X:
			m_jitters.emplace_back( 0.25, -0.25 );
			m_jitters.emplace_back( -0.25, 0.25 );
			break;

		case Mode::e4X:
			m_jitters.emplace_back( 0.125, -0.125 );
			m_jitters.emplace_back( -0.125, 0.125 );
			break;
		}
	}

	void PostEffect::doInitialiseEdgeDetection()
	{
		auto size = m_renderTarget.getSize();
		auto pixelSize = Point2f{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto vertex = doGetEdgeDetectionVP( getRenderSystem()
			, pixelSize );
		auto fragment = doGetEdgeDetectionFP( getRenderSystem()
			, pixelSize
			, m_smaaThreshold
			, m_mode == Mode::eT2X
			, m_smaaPredicationThreshold
			, m_smaaPredicationScale
			, m_smaaPredicationStrength );
		auto program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, fragment );
		program->initialise();

		castor3d::DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( castor3d::WritingMask::eZero );
		dsstate.setStencilTest( true );
		dsstate.setStencilReadMask( 0xFFFFFFFFu );
		dsstate.setStencilFrontRef( 0x00000001u );
		dsstate.setStencilFrontFunc( castor3d::StencilFunc::eNever );
		dsstate.setStencilFrontOps( castor3d::StencilOp::eReplace, castor3d::StencilOp::eKeep, castor3d::StencilOp::eKeep );
		castor3d::RasteriserState rsstate;
		rsstate.setCulledFaces( castor3d::Culling::eBack );
		m_edgeDetectionPipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, castor3d::BlendState{}
			, castor3d::MultisampleState{}
			, *program
			, castor3d::PipelineFlags{} );
		m_edgeDetectionPipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		m_edgeDetectionSurface.initialise( m_renderTarget
			, m_renderTarget.getSize()
			, castor3d::MinTextureIndex
			, m_linearSampler );
		m_depthBuffer = getRenderSystem()->createTexture( castor3d::TextureType::eTwoDimensions
			, castor3d::AccessType::eNone
			, castor3d::AccessType::eRead | castor3d::AccessType::eWrite
			, PixelFormat::eS8
			, m_renderTarget.getSize() );
		m_depthBuffer->initialise();
		m_depthAttach = m_edgeDetectionSurface.m_fbo->createAttachment( m_depthBuffer );

		m_edgeDetectionSurface.m_fbo->bind();
		m_edgeDetectionSurface.m_fbo->attach( castor3d::AttachmentPoint::eStencil, 0u, m_depthAttach, m_depthBuffer->getType() );
		REQUIRE( m_edgeDetectionSurface.m_fbo->isComplete() );
		m_edgeDetectionSurface.m_fbo->unbind();

	}

	void PostEffect::doInitialiseBlendingWeightCalculation()
	{
		auto size = m_renderTarget.getSize();
		auto pixelSize = Point2f{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
		auto areaTexBuffer = PxBufferBase::create( Size{ AREATEX_WIDTH, AREATEX_HEIGHT }
			, PixelFormat::eA8L8
			, areaTexBytes
			, PixelFormat::eA8L8 );
		auto areaTexture = getRenderSystem()->createTexture( castor3d::TextureType::eTwoDimensions
			, castor3d::AccessType::eNone
			, castor3d::AccessType::eRead );
		areaTexture->getImage().initialiseSource( areaTexBuffer );
		m_areaTex.setTexture( areaTexture );
		m_areaTex.setSampler( m_linearSampler );
		m_areaTex.setIndex( castor3d::MinTextureIndex + 1u );
		m_areaTex.initialise();

		auto searchTexBuffer = PxBufferBase::create( Size{ SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT }
			, PixelFormat::eL8
			, searchTexBytes
			, PixelFormat::eL8 );
		auto searchTexture = getRenderSystem()->createTexture( castor3d::TextureType::eTwoDimensions
			, castor3d::AccessType::eNone
			, castor3d::AccessType::eRead );
		searchTexture->getImage().initialiseSource( searchTexBuffer );
		m_searchTex.setTexture( searchTexture );
		m_searchTex.setSampler( m_pointSampler );
		m_searchTex.setIndex( castor3d::MinTextureIndex + 2u );
		m_searchTex.initialise();

		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto vertex = doBlendingWeightCalculationVP( getRenderSystem()
			, pixelSize
			, m_smaaMaxSearchSteps );
		auto fragment = doBlendingWeightCalculationFP( getRenderSystem()
			, pixelSize
			, m_smaaCornerRounding
			, m_smaaMaxSearchStepsDiag );
		auto program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, fragment );
		program->initialise();

		castor3d::DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( castor3d::WritingMask::eZero );
		dsstate.setStencilTest( true );
		dsstate.setStencilReadMask( 0x00000000u );
		dsstate.setStencilFrontFunc( castor3d::StencilFunc::eEqual );
		castor3d::RasteriserState rsstate;
		rsstate.setCulledFaces( castor3d::Culling::eBack );
		m_blendingWeightCalculationPipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, castor3d::BlendState{}
			, castor3d::MultisampleState{}
			, *program
			, castor3d::PipelineFlags{} );
		m_blendingWeightCalculationPipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		m_blendingWeightCalculationSurface.initialise( m_renderTarget
			, m_renderTarget.getSize()
			, castor3d::MinTextureIndex + 1
			, m_linearSampler );
	}

	void PostEffect::doInitialiseNeighbourhoodBlending()
	{
		auto size = m_renderTarget.getSize();
		auto pixelSize = Point2f{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto vertex = doGetNeighbourhoodBlendingVP( getRenderSystem()
			, pixelSize );
		auto fragment = doGetNeighbourhoodBlendingFP( getRenderSystem()
			, pixelSize
			, m_mode == Mode::eT2X && m_reprojection );
		auto program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, fragment );
		m_subsampleIndicesUniform = program->createUniform< castor3d::UniformType::eVec4i >( constants::SubsampleIndices
			, castor3d::ShaderType::ePixel );
		program->initialise();

		castor3d::DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( castor3d::WritingMask::eZero );
		castor3d::RasteriserState rsstate;
		rsstate.setCulledFaces( castor3d::Culling::eBack );
		m_neighbourhoodBlendingPipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, castor3d::BlendState{}
			, castor3d::MultisampleState{}
			, *program
			, castor3d::PipelineFlags{} );
		m_neighbourhoodBlendingPipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		for ( uint32_t i = 0; i < m_maxSubsampleIndices; ++i )
		{
			m_neighbourhoodBlendingSurface.emplace_back( *getRenderSystem()->getEngine() );
			m_neighbourhoodBlendingSurface.back().initialise( m_renderTarget
				, m_renderTarget.getSize()
				, castor3d::MinTextureIndex + 1u
				, m_linearSampler
				, PixelFormat::eA8R8G8B8_SRGB );
			m_neighbourhoodBlendingSurface.back().m_fbo->setSRGB( true );
		}
	}

	void PostEffect::doInitialiseReproject()
	{
		auto size = m_renderTarget.getSize();
		auto pixelSize = Point2f{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto vertex = doGetReprojectVP( getRenderSystem()
			, pixelSize );
		auto fragment = doGetReprojectFP( getRenderSystem()
			, pixelSize
			, m_reprojectionWeightScale
			, m_mode == Mode::eT2X && m_reprojection );
		auto program = cache.getNewProgram( false );
		program->createObject( castor3d::ShaderType::eVertex );
		program->createObject( castor3d::ShaderType::ePixel );
		program->setSource( castor3d::ShaderType::eVertex, vertex );
		program->setSource( castor3d::ShaderType::ePixel, fragment );
		program->initialise();

		castor3d::DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( castor3d::WritingMask::eZero );
		castor3d::RasteriserState rsstate;
		rsstate.setCulledFaces( castor3d::Culling::eBack );
		m_reprojectPipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, castor3d::BlendState{}
			, castor3d::MultisampleState{}
			, *program
			, castor3d::PipelineFlags{} );
		m_reprojectPipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		m_reprojectSurface.initialise( m_renderTarget
			, m_renderTarget.getSize()
			, castor3d::MinTextureIndex + 1
			, m_linearSampler );
	}

	void PostEffect::doEdgesDetectionPass( uint32_t prvIndex
		, uint32_t curIndex
		, castor3d::TextureLayout const & gammaSrc
		, castor3d::TextureLayout const * depthStencil )
	{
		auto & surface = m_edgeDetectionSurface;
		surface.m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
		surface.m_fbo->clear( castor3d::BufferComponent::eColour | castor3d::BufferComponent::eStencil );

		if ( !depthStencil )
		{
			getRenderSystem()->getCurrentContext()->renderTexture( surface.m_size
				, gammaSrc
				, *m_edgeDetectionPipeline
				, m_matrixUbo );
		}
		else
		{
			depthStencil->bind( castor3d::MinTextureIndex + 1u );
			m_linearSampler->bind( castor3d::MinTextureIndex + 1u );
			getRenderSystem()->getCurrentContext()->renderTexture( surface.m_size
				, gammaSrc
				, *m_edgeDetectionPipeline
				, m_matrixUbo );
			m_linearSampler->unbind( castor3d::MinTextureIndex + 1u );
			depthStencil->unbind( castor3d::MinTextureIndex + 1u );
		}

		surface.m_fbo->unbind();
	}

	void PostEffect::doBlendingWeightsCalculationPass( uint32_t prvIndex
		, uint32_t curIndex )
	{
		auto & surface = m_blendingWeightCalculationSurface;
		surface.m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
		surface.m_fbo->clear( castor3d::BufferComponent::eColour );
		m_subsampleIndicesUniform->setValue( m_smaaSubsampleIndices[m_subsampleIndex] );
		m_areaTex.bind();
		m_searchTex.bind();
		getRenderSystem()->getCurrentContext()->renderTexture( surface.m_size
			, *m_edgeDetectionSurface.m_colourTexture.getTexture()
			, *m_blendingWeightCalculationPipeline
			, m_matrixUbo );
		m_areaTex.unbind();
		m_searchTex.unbind();
		surface.m_fbo->unbind();
	}

	void PostEffect::doNeighbourhoodBlendingPass( uint32_t prvIndex
		, uint32_t curIndex
		, castor3d::TextureLayout const & gammaSrc )
	{
		auto & surface = m_neighbourhoodBlendingSurface[curIndex];
		surface.m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
		surface.m_fbo->clear( castor3d::BufferComponent::eColour );
		m_blendingWeightCalculationSurface.m_colourTexture.bind();

		if ( m_mode == Mode::eT2X )
		{
			m_renderTarget.getVelocity().getTexture()->bind( castor3d::MinTextureIndex + 2u );
			m_renderTarget.getVelocity().getSampler()->bind( castor3d::MinTextureIndex + 2u );
			getRenderSystem()->getCurrentContext()->renderTexture( surface.m_size
				, gammaSrc
				, *m_neighbourhoodBlendingPipeline
				, m_matrixUbo );
			m_renderTarget.getVelocity().getTexture()->unbind( castor3d::MinTextureIndex + 2u );
			m_renderTarget.getVelocity().getSampler()->unbind( castor3d::MinTextureIndex + 2u );
		}
		else
		{
			getRenderSystem()->getCurrentContext()->renderTexture( surface.m_size
				, gammaSrc
				, *m_neighbourhoodBlendingPipeline
				, m_matrixUbo );
		}

		m_blendingWeightCalculationSurface.m_colourTexture.unbind();
		surface.m_fbo->unbind();
	}

	void PostEffect::doMainPass( uint32_t prvIndex
		, uint32_t curIndex
		, castor3d::TextureLayout const & hdrSrc
		, castor3d::TextureLayout const & srgbSrc
		, castor3d::TextureLayout const * depthStencil )
	{
		doEdgesDetectionPass( prvIndex, curIndex, srgbSrc, depthStencil );
		doBlendingWeightsCalculationPass( prvIndex, curIndex );
		doNeighbourhoodBlendingPass( prvIndex, curIndex, srgbSrc );
	}

	void PostEffect::doReproject( uint32_t prvIndex
		, uint32_t curIndex
		, castor3d::TextureLayout const & gammaSrc )
	{
		auto & surface = m_reprojectSurface;
		surface.m_fbo->bind( castor3d::FrameBufferTarget::eDraw );
		surface.m_fbo->clear( castor3d::BufferComponent::eColour );
		m_neighbourhoodBlendingSurface[prvIndex].m_colourTexture.bind();

		if ( m_mode == Mode::eT2X && m_reprojection )
		{
			m_renderTarget.getVelocity().getTexture()->bind( castor3d::MinTextureIndex + 2u );
			m_renderTarget.getVelocity().getSampler()->bind( castor3d::MinTextureIndex + 2u );
			getRenderSystem()->getCurrentContext()->renderTexture( surface.m_size
				, *m_neighbourhoodBlendingSurface[curIndex].m_colourTexture.getTexture()
				, *m_reprojectPipeline
				, m_matrixUbo );
			m_renderTarget.getVelocity().getTexture()->unbind( castor3d::MinTextureIndex + 2u );
			m_renderTarget.getVelocity().getSampler()->unbind( castor3d::MinTextureIndex + 2u );
		}
		else
		{
			getRenderSystem()->getCurrentContext()->renderTexture( surface.m_size
				, *m_neighbourhoodBlendingSurface[curIndex].m_colourTexture.getTexture()
				, *m_reprojectPipeline
				, m_matrixUbo );
		}

		m_neighbourhoodBlendingSurface[prvIndex].m_colourTexture.unbind();
		surface.m_fbo->unbind();
	}
}

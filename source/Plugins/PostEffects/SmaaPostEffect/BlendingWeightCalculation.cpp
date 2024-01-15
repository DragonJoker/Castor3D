#include "SmaaPostEffect/BlendingWeightCalculation.hpp"

#include "SmaaPostEffect/AreaTex.h"
#include "SmaaPostEffect/SearchTex.h"
#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace smaa
{
	namespace bwcalc
	{
		namespace c3d = castor3d::shader;

		enum Idx : uint32_t
		{
			AreaTexIdx = SmaaUboIdx + 1,
			SearchTexIdx,
			EdgesTexIdx,
		};

		template< sdw::var::Flag FlagT >
		using VertexStructT = sdw::IOStructInstanceHelperT< FlagT
			, "SMAABW_Vertex"
			, sdw::IOVec2Field< "texcoord", 0u >
			, sdw::IOVec2Field< "pixcoord", 1u >
			, sdw::IOVec4ArrayField< "offset", 2u, 3u > >;

		template< sdw::var::Flag FlagT >
		struct VertexT
			: public VertexStructT< FlagT >
		{
			VertexT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: VertexStructT< FlagT >{ writer, std::move( expr ), enabled }
			{
			}

			auto texcoord()const { return this->template getMember< "texcoord" >(); }
			auto pixcoord()const { return this->template getMember< "pixcoord" >(); }
			auto offset()const { return this->template getMember< "offset" >(); }
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Smaa( writer, SmaaUboIdx, 0u );
			auto c3d_areaTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_areaTex", AreaTexIdx, 0u );
			auto c3d_searchTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_searchTex", SearchTexIdx, 0u );
			auto c3d_edgesTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_edgesTex", EdgesTexIdx, 0u );

			/**
			 * Blend Weight Calculation Vertex Shader
			 */
			auto SMAABlendingWeightCalculationVS = writer.implementFunction< sdw::Void >( "SMAABlendingWeightCalculationVS"
				, [&]( sdw::Vec2 const & texCoord
					, sdw::Vec2 pixcoord
					, sdw::Vec4Array offset )
				{
					pixcoord = texCoord * c3d_smaaData.rtMetrics.zw();

					// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
					offset[0] = fma( c3d_smaaData.rtMetrics.xyxy(), vec4( -0.25_f, -0.125f, 1.25f, -0.125f ), vec4( texCoord.xy(), texCoord.xy() ) );
					offset[1] = fma( c3d_smaaData.rtMetrics.xyxy(), vec4( -0.125_f, -0.25f, -0.125f, 1.25f ), vec4( texCoord.xy(), texCoord.xy() ) );

					// And these for the searches, they indicate the ends of the loops:
					offset[2] = fma( c3d_smaaData.rtMetrics.xxyy()
						, vec4( -2.0_f, 2.0_f, -2.0_f, 2.0_f ) * writer.cast< sdw::Float >( c3d_smaaData.maxSearchSteps )
						, vec4( offset[0].xz(), offset[1].yw() ) );
				}
				, sdw::InVec2{ writer, "texCoord" }
				, sdw::OutVec2{ writer, "pixcoord" }
				, sdw::OutVec4Array{ writer, "offset", 3u } );

			/**
			 * Conditional move:
			 */
			auto SMAAMovc = writer.implementFunction< sdw::Void >( "SMAAMovc"
				, [&]( sdw::BVec2 const & cond
					, sdw::Vec2 variable
					, sdw::Vec2 const & value )
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
				, sdw::InBVec2{ writer, "cond" }
				, sdw::InOutVec2{ writer, "variable" }
				, sdw::InVec2{ writer, "value" } );

			/**
			 * Allows to decode two binary values from a bilinear-filtered access.
			 */
			auto SMAADecodeDiagBilinearAccess2 = writer.implementFunction< sdw::Vec2 >( "SMAADecodeDiagBilinearAccess2"
				, [&]( sdw::Vec2 const & e )
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
				, sdw::InVec2{ writer, "e" } );

			auto SMAADecodeDiagBilinearAccess4 = writer.implementFunction< sdw::Vec4 >( "SMAADecodeDiagBilinearAccess4"
				, [&]( sdw::Vec4 const & e )
				{
					e.rb() = e.rb() * abs( 5.0_f * e.rb() - vec2( 5.0_f * 0.75_f ) );
					writer.returnStmt( round( e ) );
				}
				, sdw::InVec4{ writer, "e" } );

			/**
			 * These functions allows to perform diagonal pattern searches.
			 */
			auto SMAASearchDiag1 = writer.implementFunction< sdw::Vec2 >( "SMAASearchDiag1"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::Vec2 const & texcoord
					, sdw::Vec2 const & dir
					, sdw::Vec2 e )
				{
					auto coord = writer.declLocale( "coord"
						, vec4( texcoord, -1.0_f, 1.0_f ) );
					auto t = writer.declLocale( "t"
						, vec3( c3d_smaaData.rtMetrics.xy(), 1.0_f ) );

					WHILE( writer, coord.z() < writer.cast< sdw::Float >( c3d_smaaData.maxSearchStepsDiag - 1 )
						&& coord.w() > 0.9_f )
					{
						coord.xyz() = fma( t, vec3( dir, 1.0_f ), coord.xyz() );
						e = edgesTex.lod( coord.xy(), 0.0_f ).rg();
						coord.w() = dot( e, vec2( 0.5_f, 0.5_f ) );
					}
					ELIHW;

					writer.returnStmt( coord.zw() );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec2{ writer, "dir" }
				, sdw::OutVec2{ writer, "e" } );

			auto SMAASearchDiag2 = writer.implementFunction< sdw::Vec2 >( "SMAASearchDiag2"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::Vec2 const & texcoord
					, sdw::Vec2 const & dir
					, sdw::Vec2 e )
				{
					auto coord = writer.declLocale( "coord"
						, vec4( texcoord, -1.0_f, 1.0_f ) );
					coord.x() += 0.25_f * c3d_smaaData.rtMetrics.x(); // See @SearchDiag2Optimization
					auto t = writer.declLocale( "t"
						, vec3( c3d_smaaData.rtMetrics.xy(), 1.0_f ) );

					WHILE( writer, coord.z() < writer.cast< sdw::Float >( c3d_smaaData.maxSearchStepsDiag - 1 )
						&& coord.w() > 0.9_f )
					{
						coord.xyz() = fma( t, vec3( dir, 1.0_f ), coord.xyz() );

						// @SearchDiag2Optimization
						// Fetch both edges at once using bilinear filtering:
						e = edgesTex.lod( coord.xy(), 0.0_f ).rg();
						e = SMAADecodeDiagBilinearAccess2( e );

						// Non-optimized version:
						// e.g = textureLod(edgesTex, coord.xy, 0.0_f).g;
						// e.r = textureLodOffset(edgesTex, coord.xy, 0.0_f, ivec2(1, 0)).r;

						coord.w() = dot( e, vec2( 0.5_f, 0.5_f ) );
					}
					ELIHW;

					writer.returnStmt( coord.zw() );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec2{ writer, "dir" }
				, sdw::OutVec2{ writer, "e" } );

			/**
			 * Similar to SMAAArea, this calculates the area corresponding to a certain
			 * diagonal distance and crossing edges 'e'.
			 */
			auto SMAAAreaDiag = writer.implementFunction< sdw::Vec2 >( "SMAAAreaDiag"
				, [&]( sdw::CombinedImage2DRgba32 const & areaTex
					, sdw::Vec2 const & dist
					, sdw::Vec2 const & e
					, sdw::Float const & offset )
				{
					auto texcoord = writer.declLocale( "texcoord"
						, fma( vec2( c3d_smaaData.areaTexMaxDistanceDiag, c3d_smaaData.areaTexMaxDistanceDiag ), e, dist ) );

					// We do a scale and bias for mapping to texel space:
					texcoord = fma( c3d_smaaData.areaTexPixelSize, texcoord, 0.5_f * c3d_smaaData.areaTexPixelSize );

					// Diagonal areas are on the second half of the texture:
					texcoord.x() += 0.5_f;

					// Move to proper place, according to the subpixel offset:
					texcoord.y() += c3d_smaaData.areaTexSubtexSize * offset;

					// Do it!
					writer.returnStmt( areaTex.lod( texcoord, 0.0_f ).rg() );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "areaTex" }
				, sdw::InVec2{ writer, "dist" }
				, sdw::InVec2{ writer, "e" }
				, sdw::InFloat{ writer, "offset" } );

			/**
			 * This searches for diagonal patterns and returns the corresponding weights.
			 */
			auto SMAACalculateDiagWeights = writer.implementFunction< sdw::Vec2 >( "SMAACalculateDiagWeights"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::CombinedImage2DRgba32 const & areaTex
					, sdw::Vec2 const & texcoord
					, sdw::Vec2 const & e
					, sdw::Vec4 const & subsampleIndices )
				{
					auto weights = writer.declLocale( "weights"
						, vec2( 0.0_f, 0.0_f ) );

					// Search for the line ends:
					auto d = writer.declLocale< sdw::Vec4 >( "d"
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
								, c3d_smaaData.rtMetrics.xyxy()
								, vec4( texcoord.xy(), texcoord.xy() ) ) );
						auto c = writer.declLocale< sdw::Vec4 >( "c" );
						c.xy() = edgesTex.lod( coords.xy(), 0.0_f, ivec2( -1_i, 0_i ) ).rg();
						c.zw() = edgesTex.lod( coords.zw(), 0.0_f, ivec2( 1_i, 0_i ) ).rg();
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

					IF( writer, edgesTex.lod( texcoord, 0.0_f, ivec2( 1_i, 0_i ) ).r() > 0.0_f )
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
								, c3d_smaaData.rtMetrics.xyxy()
								, vec4( texcoord.xy(), texcoord.xy() ) ) );
						auto c = writer.declLocale< sdw::Vec4 >( "c" );
						c.x() = edgesTex.lod( coords.xy(), 0.0_f, ivec2( -1_i, 0_i ) ).g();
						c.y() = edgesTex.lod( coords.xy(), 0.0_f, ivec2( 0_i, -1_i ) ).r();
						c.zw() = edgesTex.lod( coords.zw(), 0.0_f, ivec2( 1_i, 0_i ) ).gr();
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
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "areaTex" }
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec2{ writer, "e" }
				, sdw::InVec4{ writer, "subsampleIndices" } );

			//-----------------------------------------------------------------------------
			// Horizontal/Vertical Search Functions

			/**
			 * This allows to determine how much length should we add in the last step
			 * of the searches. It takes the bilinearly interpolated edge (see
			 * @PSEUDO_GATHER4), and adds 0, 1 or 2, depending on which edges and
			 * crossing edges are active.
			 */
			auto SMAASearchLength = writer.implementFunction< sdw::Float >( "SMAASearchLength"
				, [&]( sdw::CombinedImage2DRgba32 const & searchTex
					, sdw::Vec2 const & e
					, sdw::Float const & offset )
				{
					// The texture is flipped vertically, with left and right cases taking half
					// of the space horizontally:
					auto scale = writer.declLocale( "scale"
						, c3d_smaaData.searchTexSize * vec2( 0.5_f, -1.0_f ) );
					auto bias = writer.declLocale( "bias"
						, c3d_smaaData.searchTexSize * vec2( offset, 1.0_f ) );

					// Scale and bias to access texel centers:
					scale += vec2( -1.0_f, 1.0_f );
					bias += vec2( 0.5_f, -0.5_f );

					// Convert from pixel coordinates to texcoords:
					// (We use SMAA_SEARCHTEX_PACKED_SIZE because the texture is cropped)
					scale *= vec2( 1.0_f ) / c3d_smaaData.searchTexPackedSize;
					bias *= vec2( 1.0_f ) / c3d_smaaData.searchTexPackedSize;

					// Lookup the search texture:
					writer.returnStmt( searchTex.lod( fma( scale, e, bias ), 0.0_f ).r() );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "searchTex" }
				, sdw::InVec2{ writer, "e" }
				, sdw::InFloat{ writer, "offset" } );

			/**
			 * Horizontal/vertical search functions for the 2nd pass.
			 */
			auto SMAASearchXLeft = writer.implementFunction< sdw::Float >( "SMAASearchXLeft"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::CombinedImage2DRgba32 const & searchTex
					, sdw::Vec2 texcoord
					, sdw::Float end )
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
						e = edgesTex.lod( texcoord, 0.0_f ).rg();
						texcoord = fma( -vec2( 2.0_f, 0.0_f ), c3d_smaaData.rtMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( "offset"
						, fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e, 0.0_f ), 3.25_f ) );
					writer.returnStmt( fma( c3d_smaaData.rtMetrics.x(), offset, texcoord.x() ) );

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
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "searchTex" }
				, sdw::PVec2{ writer, "texcoord" }
				, sdw::PFloat{ writer, "end" } );

			auto SMAASearchXRight = writer.implementFunction< sdw::Float >( "SMAASearchXRight"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::CombinedImage2DRgba32 const & searchTex
					, sdw::Vec2 texcoord
					, sdw::Float end )
				 {
					 auto e = writer.declLocale( "e"
						 , vec2( 0.0_f, 1.0_f ) );
					 WHILE ( writer, texcoord.x() < end
						 && e.g() > 0.8281_f // Is there some edge not activated?
						 && e.r() == 0.0_f )
					 { // Or is there a crossing edge that breaks the line?
						 e = edgesTex.lod( texcoord, 0.0_f ).rg();
						 texcoord = fma( vec2( 2.0_f, 0.0_f ), c3d_smaaData.rtMetrics.xy(), texcoord );
					 }
					 ELIHW;

					 auto offset = writer.declLocale( "offset"
						 , fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e, 0.5_f ), 3.25_f ) );
					 writer.returnStmt( fma( -c3d_smaaData.rtMetrics.x(), offset, texcoord.x() ) );
				 }
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "searchTex" }
				, sdw::PVec2{ writer, "texcoord" }
				, sdw::PFloat{ writer, "end" } );

			auto SMAASearchYUp = writer.implementFunction< sdw::Float >( "SMAASearchYUp"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::CombinedImage2DRgba32 const & searchTex
					, sdw::Vec2 texcoord
					, sdw::Float end )
				{
					auto e = writer.declLocale( "e"
						 , vec2( 1.0_f, 0.0_f ) );
					WHILE ( writer, texcoord.y() > end
						&& e.r() > 0.8281_f // Is there some edge not activated?
						&& e.g() == 0.0_f )
					{ // Or is there a crossing edge that breaks the line?
						e = edgesTex.lod( texcoord, 0.0_f ).rg();
						texcoord = fma( -vec2( 0.0_f, 2.0_f ), c3d_smaaData.rtMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( "offset"
						, fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e.gr(), 0.0_f ), 3.25_f ) );
					writer.returnStmt( fma( c3d_smaaData.rtMetrics.y(), offset, texcoord.y() ) );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "searchTex" }
				, sdw::PVec2{ writer, "texcoord" }
				, sdw::PFloat{ writer, "end" } );

			auto SMAASearchYDown = writer.implementFunction< sdw::Float >( "SMAASearchYDown"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::CombinedImage2DRgba32 const & searchTex
					, sdw::Vec2 texcoord
					, sdw::Float end )
				{
					auto e = writer.declLocale( "e"
						 , vec2( 1.0_f, 0.0_f ) );
					WHILE ( writer, texcoord.y() < end
						&& e.r() > 0.8281_f // Is there some edge not activated?
						&& e.g() == 0.0_f )
					{ // Or is there a crossing edge that breaks the line?
						e = edgesTex.lod( texcoord, 0.0_f ).rg();
						texcoord = fma( vec2( 0.0_f, 2.0_f ), c3d_smaaData.rtMetrics.xy(), texcoord );
					}
					ELIHW;

					auto offset = writer.declLocale( "offset"
						, fma( -( 255.0_f / 127.0_f ), SMAASearchLength( searchTex, e.gr(), 0.5_f ), 3.25_f ) );
					writer.returnStmt( fma( -c3d_smaaData.rtMetrics.y(), offset, texcoord.y() ) );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "searchTex" }
				, sdw::PVec2{ writer, "texcoord" }
				, sdw::PFloat{ writer, "end" } );

			 /**
			  * Ok, we have the distance and both crossing edges. So, what are the areas
			  * at each side of current edge?
			  */
			auto SMAAArea = writer.implementFunction< sdw::Vec2 >( "SMAAArea"
				, [&]( sdw::CombinedImage2DRgba32 const & areaTex
					, sdw::Vec2 const & dist
					, sdw::Float const & e1
					, sdw::Float const & e2
					, sdw::Float const & offset )
				{
					// Rounding prevents precision errors of bilinear filtering:
					auto texcoord = writer.declLocale( "texcoord"
						, fma( vec2( c3d_smaaData.areaTexMaxDistance, c3d_smaaData.areaTexMaxDistance ), round( 4.0_f * vec2( e1, e2 ) ), dist ) );

					// We do a scale and bias for mapping to texel space:
					texcoord = fma( c3d_smaaData.areaTexPixelSize, texcoord, 0.5_f * c3d_smaaData.areaTexPixelSize );

					// Move to proper place, according to the subpixel offset:
					texcoord.y() = fma( c3d_smaaData.areaTexSubtexSize, offset, texcoord.y() );

					// Do it!
					writer.returnStmt( areaTex.lod( texcoord, 0.0_f ).rg() );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "areaTex" }
				, sdw::InVec2{ writer, "dist" }
				, sdw::InFloat{ writer, "e1" }
				, sdw::InFloat{ writer, "e2" }
				, sdw::InFloat{ writer, "offset" } );

			//-----------------------------------------------------------------------------
			// Corner Detection Functions

			auto SMAADetectHorizontalCornerPattern = writer.implementFunction< sdw::Void >( "SMAADetectHorizontalCornerPattern"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::Vec2 weights
					, sdw::Vec4 const & texcoord
					, sdw::Vec2 const & d )
				{
					IF( writer, c3d_smaaData.disableCornerDetection == 0 )
					{
						auto leftRight = writer.declLocale( "leftRight"
							, step( d.xy(), d.yx() ) );
						auto rounding = writer.declLocale( "rounding"
							, ( 1.0_f - c3d_smaaData.cornerRoundingNorm ) * leftRight );

						rounding /= leftRight.x() + leftRight.y(); // Reduce blending for pixels in the center of a line.

						auto factor = writer.declLocale( "factor"
							, vec2( 1.0_f, 1.0_f ) );
						factor.x() -= rounding.x() * edgesTex.lod( texcoord.xy(), 0.0_f, ivec2( 0_i, 1_i ) ).r();
						factor.x() -= rounding.y() * edgesTex.lod( texcoord.zw(), 0.0_f, ivec2( 1_i, 1_i ) ).r();
						factor.y() -= rounding.x() * edgesTex.lod( texcoord.xy(), 0.0_f, ivec2( 0_i, -2_i ) ).r();
						factor.y() -= rounding.y() * edgesTex.lod( texcoord.zw(), 0.0_f, ivec2( 1_i, -2_i ) ).r();

						weights *= clamp( factor, vec2( 0.0_f ), vec2( 1.0_f ) );
					}
					FI;
				}
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InOutVec2{ writer, "weights" }
				, sdw::InVec4{ writer, "texcoord" }
				, sdw::InVec2{ writer, "d" } );

			auto SMAADetectVerticalCornerPattern = writer.implementFunction< sdw::Void >( "SMAADetectVerticalCornerPattern"
				, [&]( sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::Vec2 weights
					, sdw::Vec4 const & texcoord
					, sdw::Vec2 const & d )
				{
					IF( writer, c3d_smaaData.disableCornerDetection == 0 )
					{
						auto leftRight = writer.declLocale( "leftRight"
							, step( d.xy(), d.yx() ) );
						auto rounding = writer.declLocale( "rounding"
							, ( 1.0_f - c3d_smaaData.cornerRoundingNorm ) * leftRight );

						rounding /= leftRight.x() + leftRight.y();

						auto factor = writer.declLocale( "factor"
							, vec2( 1.0_f, 1.0_f ) );
						factor.x() -= rounding.x() * edgesTex.lod( texcoord.xy(), 0.0_f, ivec2( 1_i, 0_i ) ).g();
						factor.x() -= rounding.y() * edgesTex.lod( texcoord.zw(), 0.0_f, ivec2( 1_i, 1_i ) ).g();
						factor.y() -= rounding.x() * edgesTex.lod( texcoord.xy(), 0.0_f, ivec2( -2_i, 0_i ) ).g();
						factor.y() -= rounding.y() * edgesTex.lod( texcoord.zw(), 0.0_f, ivec2( -2_i, 1_i ) ).g();

						weights *= clamp( factor, vec2( 0.0_f ), vec2( 1.0_f ) );
					}
					FI;
				}
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InOutVec2{ writer, "weights" }
				, sdw::InVec4{ writer, "texcoord" }
				, sdw::InVec2{ writer, "d" } );

			auto SMAABlendingWeightCalculationPS = writer.implementFunction< sdw::Vec4 >( "SMAABlendingWeightCalculationPS"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec2 const & pixcoord
					, sdw::Vec4Array const & offset
					, sdw::CombinedImage2DRgba32 const & edgesTex
					, sdw::CombinedImage2DRgba32 const & areaTex
					, sdw::CombinedImage2DRgba32 const & searchTex
					, sdw::Vec4 const & subsampleIndices )
				{ // Just pass zero for SMAA 1x, see @SUBSAMPLE_INDICES.
					auto weights = writer.declLocale( "weights"
						, vec4( 0.0_f, 0.0_f, 0.0_f, 0.0_f ) );

					auto e = writer.declLocale( "e"
						, edgesTex.sample( texcoord ).rg() );

					IF( writer, e.g() > 0.0_f )
					{ // Edge at north
						IF( writer, c3d_smaaData.disableDiagonalDetection == 0 )
						{
							// Diagonals have both north and west edges, so searching for them in
							// one of the boundaries is enough.
							weights.rg() = SMAACalculateDiagWeights( edgesTex, areaTex, texcoord, e, subsampleIndices );
							// We give priority to diagonals, so if we find a diagonal we skip 
							// horizontal/vertical processing.
							IF( writer, weights.r() == -weights.g() )
							{ // weights.r + weights.g == 0.0
								auto d = writer.declLocale< sdw::Vec2 >( "d" );

								// Find the distance to the left:
								auto coords = writer.declLocale< sdw::Vec3 >( "coords" );
								coords.x() = SMAASearchXLeft( edgesTex, searchTex, offset[0].xy(), offset[2].x() );
								coords.y() = offset[1].y(); // offset[1].y() = texcoord.y() - 0.25 * c3d_smaaData.rtMetrics.y() (@CROSSING_OFFSET)
								d.x() = coords.x();

								// Now fetch the left crossing edges, two at a time using bilinear
								// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
								// discern what value each edge has:
								auto e1 = writer.declLocale( "e1"
									, edgesTex.lod( coords.xy(), 0.0_f ).r() );

								// Find the distance to the right:
								coords.z() = SMAASearchXRight( edgesTex, searchTex, offset[0].zw(), offset[2].y() );
								d.y() = coords.z();

								// We want the distances to be in pixel units (doing this here allow to
								// better interleave arithmetic and memory accesses):
								d = abs( round( fma( c3d_smaaData.rtMetrics.zz(), d, -pixcoord.xx() ) ) );

								// SMAAArea below needs a sqrt, as the areas texture is compressed
								// quadratically:
								auto sqrt_d = writer.declLocale( "sqrt_d"
									, sqrt( d ) );

								// Fetch the right crossing edges:
								auto e2 = writer.declLocale( "e2"
									, edgesTex.lod( coords.zy(), 0.0_f, ivec2( 1_i, 0_i ) ).r() );

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
						ELSE
						{
							auto d = writer.declLocale< sdw::Vec2 >( "d" );

							// Find the distance to the left:
							auto coords = writer.declLocale< sdw::Vec3 >( "coords" );
							coords.x() = SMAASearchXLeft( edgesTex, searchTex, offset[0].xy(), offset[2].x() );
							coords.y() = offset[1].y(); // offset[1].y() = texcoord.y() - 0.25 * c3d_smaaData.rtMetrics.y() (@CROSSING_OFFSET)
							d.x() = coords.x();

							// Now fetch the left crossing edges, two at a time using bilinear
							// filtering. Sampling at -0.25 (see @CROSSING_OFFSET) enables to
							// discern what value each edge has:
							auto e1 = writer.declLocale( "e1"
								, edgesTex.lod( coords.xy(), 0.0_f ).r() );

							// Find the distance to the right:
							coords.z() = SMAASearchXRight( edgesTex, searchTex, offset[0].zw(), offset[2].y() );
							d.y() = coords.z();

							// We want the distances to be in pixel units (doing this here allow to
							// better interleave arithmetic and memory accesses):
							d = abs( round( fma( c3d_smaaData.rtMetrics.zz(), d, -pixcoord.xx() ) ) );

							// SMAAArea below needs a sqrt, as the areas texture is compressed
							// quadratically:
							auto sqrt_d = writer.declLocale( "sqrt_d"
								, sqrt( d ) );

							// Fetch the right crossing edges:
							auto e2 = writer.declLocale( "e2"
								, edgesTex.lod( coords.zy(), 0.0_f, ivec2( 1_i, 0_i ) ).r() );

							// Ok, we know how this pattern looks like, now it is time for getting
							// the actual area:
							weights.rg() = SMAAArea( areaTex, sqrt_d, e1, e2, subsampleIndices.y() );

							// Fix corners:
							coords.y() = texcoord.y();
							SMAADetectHorizontalCornerPattern( edgesTex, weights.rg(), vec4( coords.xy(), coords.zy() ), d );
						}
						FI;
					}
					FI;

					IF( writer, e.r() > 0.0_f )
					{ // Edge at west
						auto d = writer.declLocale< sdw::Vec2 >( "d" );

						// Find the distance to the top:
						auto coords = writer.declLocale< sdw::Vec3 >( "coords" );
						coords.y() = SMAASearchYUp( edgesTex, searchTex, offset[1].xy(), offset[2].z() );
						coords.x() = offset[0].x(); // offset[1].x() = texcoord.x() - 0.25 * c3d_smaaData.rtMetrics.x();
						d.x() = coords.y();

						// Fetch the top crossing edges:
						auto e1 = writer.declLocale( "e1"
							, edgesTex.lod( coords.xy(), 0.0_f ).g() );

						// Find the distance to the bottom:
						coords.z() = SMAASearchYDown( edgesTex, searchTex, offset[1].zw(), offset[2].w() );
						d.y() = coords.z();

						// We want the distances to be in pixel units:
						d = abs( round( fma( c3d_smaaData.rtMetrics.ww(), d, -pixcoord.yy() ) ) );

						// SMAAArea below needs a sqrt, as the areas texture is compressed 
						// quadratically:
						auto sqrt_d = writer.declLocale( "sqrt_d"
							, sqrt( d ) );

						// Fetch the bottom crossing edges:
						auto e2 = writer.declLocale( "e2"
							, edgesTex.lod( coords.xz(), 0.0_f, ivec2( 0_i, 1_i ) ).g() );

						// Get the area for this direction:
						weights.ba() = SMAAArea( areaTex, sqrt_d, e1, e2, subsampleIndices.x() );

						// Fix corners:
						coords.x() = texcoord.x();
						SMAADetectVerticalCornerPattern( edgesTex, weights.ba(), vec4( coords.xy(), coords.xz() ), d );
					}
					FI;

					writer.returnStmt( weights );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec2{ writer, "pixcoord" }
				, sdw::InVec4Array{ writer, "offset", 3u }
				, sdw::InCombinedImage2DRgba32{ writer, "edgesTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "areaTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "searchTex" }
				, sdw::InVec4{ writer, "subsampleIndices" } );

			writer.implementEntryPointT< c3d::PosUv2FT, VertexT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< VertexT > out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
					out.texcoord() = in.uv();
					out.offset()[0] = vec4( 0.0_f );
					out.offset()[1] = vec4( 0.0_f );
					out.offset()[2] = vec4( 0.0_f );
					SMAABlendingWeightCalculationVS( out.texcoord(), out.pixcoord(), out.offset() );
				} );

			writer.implementEntryPointT< VertexT, c3d::Colour4FT >( [&]( sdw::FragmentInT< VertexT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					out.colour() = SMAABlendingWeightCalculationPS( in.texcoord()
						, in.pixcoord()
						, in.offset()
						, c3d_edgesTex
						, c3d_areaTex
						, c3d_searchTex
						, c3d_smaaData.subsampleIndices );
				} );
			return writer.getBuilder().releaseShader();
		}

		static crg::ImageViewId createImage( crg::FramePassGroup & graph
			, crg::ResourcesCache & resources
			, castor3d::RenderDevice const & device
			, std::string const & name
			, VkFormat format
			, VkExtent3D const & dimensions
			, castor::ArrayView< const unsigned char > const & bytes )
		{
			auto & context = device.makeContext();
			auto imageId = graph.createImage( crg::ImageData{ name
				, 0u
				, VK_IMAGE_TYPE_2D
				, format
				, dimensions
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) } );
			auto result = graph.createView( crg::ImageViewData{ name
				, imageId
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, imageId.data->info.format
				, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
			auto staging = device->createStagingTexture( format, dimensions );
			auto image = std::make_unique< ashes::Image >( *device
				, resources.createImage( context, imageId )
				, ashes::ImageCreateInfo{ imageId.data->info } );
			ashes::ImageView view{ ashes::ImageViewCreateInfo{ result.data->info }
				, resources.createImageView( context, result )
				, image.get() };
			auto data = device.graphicsData();
			staging->uploadTextureData( *data->queue
				, *data->commandPool
				, { result.data->info.subresourceRange.aspectMask
					, result.data->info.subresourceRange.baseMipLevel
					, result.data->info.subresourceRange.baseArrayLayer
					, result.data->info.subresourceRange.layerCount }
				, format
				, { 0, 0, 0 }
				, dimensions
				, bytes.data()
				, view );
			return result;
		}
	}

	//*********************************************************************************************

	BlendingWeightCalculation::BlendingWeightCalculation( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, crg::ImageViewId const & edgeDetectionView
		, crg::ImageViewId const & stencilView
		, SmaaConfig const & config
		, bool const * enabled )
		: m_device{ device }
		, m_graph{ graph }
		, m_resources{ renderTarget.getResources() }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_areaView{ bwcalc::createImage( m_graph
			, m_resources
			, m_device
			, "SMBWArea"
			, VK_FORMAT_R8G8_UNORM
			, { AREATEX_WIDTH, AREATEX_HEIGHT, 1u }
			, castor::makeArrayView( std::begin( areaTexBytes ), std::end( areaTexBytes ) ) ) }
		, m_searchView{ bwcalc::createImage( m_graph
			, m_resources
			, m_device
			, "SMBWSearch"
			, VK_FORMAT_R8_UNORM
			, { SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, 1u }
			, castor::makeArrayView( std::begin( searchTexBytes ), std::end( searchTexBytes ) ) ) }
		, m_result{ m_device
			, m_resources
			, "SMBWRes"
			, 0u
			, m_extent
			, 1u
			, 1u
			, VK_FORMAT_R8G8B8A8_UNORM
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) }
		, m_shader{ "SmaaBlendingWeight", bwcalc::getProgram( device ) }
		, m_stages{ makeProgramStates( m_device, m_shader ) }
		, m_pass{ m_graph.createPass( "BlendingWeight"
			, [this, &device, enabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				ashes::PipelineDepthStencilStateCreateInfo dsState{ 0u, VK_FALSE, VK_FALSE };
				dsState->stencilTestEnable = VK_TRUE;
				dsState->front.compareOp = VK_COMPARE_OP_EQUAL;
				dsState->front.reference = 1u;
				dsState->back = dsState->front;
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.depthStencilState( dsState )
					.enabled( enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} ) }
	{
		m_graph.addInput( m_areaView
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		m_graph.addInput( m_searchView
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addDependency( previousPass );
		ubo.createPassBinding( m_pass
			, SmaaUboIdx );
		m_pass.addSampledView( m_areaView
			, bwcalc::AreaTexIdx
			, linearSampler );
		m_pass.addSampledView( m_searchView
			, bwcalc::SearchTexIdx );
		m_pass.addSampledView( edgeDetectionView
			, bwcalc::EdgesTexIdx
			, linearSampler );
		m_pass.addInputStencilView( stencilView );
		m_pass.addOutputColourView( m_result.targetViewId
			, castor3d::transparentBlackClearColor );
		m_result.create();
	}

	BlendingWeightCalculation::~BlendingWeightCalculation()
	{
		m_resources.destroyImageView( m_areaView );
		m_resources.destroyImage( m_areaView.data->image );
		m_resources.destroyImageView( m_searchView );
		m_resources.destroyImage( m_searchView.data->image );
		m_result.destroy();
	}

	void BlendingWeightCalculation::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
		visitor.visit( "SMAA BlendingWeight Result"
			, m_result
			, m_graph.getFinalLayoutState( m_result.sampledViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}

	//*********************************************************************************************
}

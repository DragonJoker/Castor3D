#include "SmaaPostEffect/LumaEdgeDetection.hpp"

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
	namespace lumaed
	{
		namespace c3ds = c3d::shader;

		enum class Bindings : uint32_t
		{
			ColorTexIdx = uint32_t( smaa::Bindings::SmaaUboIdx ) + 1u,
			PredicationTexIdx,
		};

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device
			, bool predication )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Shader inputs
			C3D_Smaa( writer, smaa::Bindings::SmaaUboIdx, 0u );
			auto c3d_colourTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_colourTex", Bindings::ColorTexIdx, 0u );
			auto c3d_predicationTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_predicationTex", Bindings::PredicationTexIdx, 0u, predication );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< sdw::Vec3 >( "SMAAGatherNeighbours"
				, [&writer, &c3d_smaaData]( sdw::Vec2 const & texcoord
					, sdw::CombinedImage2DRgba32 const & tex )
				{
					writer.returnStmt( tex.gather( texcoord + c3d_smaaData.rtMetrics.xy() * vec2( -0.5_f, -0.5_f ), 0_i ).grb() );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InCombinedImage2DRgba32{ writer, "tex" } );

			/**
			 * Adjusts the threshold by means of predication.
			 */
			auto SMAACalculatePredicatedThreshold = writer.implementFunction< sdw::Vec2 >( "SMAACalculatePredicatedThreshold"
				, [&writer, &SMAAGatherNeighbours, &c3d_smaaData]( sdw::Vec2 const & texcoord
					, sdw::CombinedImage2DRgba32 const & predicationTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, predicationTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_smaaData.predicationThreshold ), delta ) );
					writer.returnStmt( c3d_smaaData.predicationScale * c3d_smaaData.threshold * ( 1.0_f - c3d_smaaData.predicationStrength * edges ) );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InCombinedImage2DRgba32{ writer, "predicationTex" } );

			/**
			 * Luma Edge Detection
			 *
			 * IMPORTANT NOTICE: luma edge detection requires gamma-corrected colors, and
			 * thus 'colorTex' should be a non-sRGB texture.
			 */
			auto SMAALumaEdgeDetectionPS = writer.implementFunction< sdw::Vec2 >( "SMAALumaEdgeDetectionPS"
				, [&writer, &SMAACalculatePredicatedThreshold, &c3d_predicationTex, &c3d_smaaData, &c3d_colourTex
					, &predication]( sdw::Vec2 const & texcoord
						, sdw::Vec4Array const & offset )
				{
					// Calculate the threshold:
					auto threshold = writer.declLocale< sdw::Vec2 >( "threshold"
						, ( predication
							? SMAACalculatePredicatedThreshold( texcoord, c3d_predicationTex )
							: vec2( c3d_smaaData.threshold, c3d_smaaData.threshold ) ) );

					// Calculate lumas:
					auto weights = writer.declLocale( "weights"
						, vec3( 0.2126_f, 0.7152_f, 0.0722_f ) );
					auto L = writer.declLocale( "L"
						, dot( c3d_colourTex.sample( texcoord ).rgb(), weights ) );

					auto Lleft = writer.declLocale( "Lleft"
						, dot( c3d_colourTex.sample( offset[0].xy() ).rgb(), weights ) );
					auto Ltop = writer.declLocale( "Ltop"
						, dot( c3d_colourTex.sample( offset[0].zw() ).rgb(), weights ) );

					// We do the usual threshold:
					auto delta = writer.declLocale< sdw::Vec4 >( "delta" );
					delta.xy() = abs( L - vec2( Lleft, Ltop ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( threshold ), delta.xy() ) );

					// Then discard if there is no edge:
					sdwIF( writer, dot( edges, vec2( 1.0_f, 1.0_f ) ) == 0.0_f )
					{
						writer.demote();
					}
					sdwFI

					// Calculate right and bottom deltas:
					auto Lright = writer.declLocale( "Lright"
						, dot( c3d_colourTex.sample( offset[1].xy() ).rgb(), weights ) );
					auto Lbottom = writer.declLocale( "Lbottom"
						, dot( c3d_colourTex.sample( offset[1].zw() ).rgb(), weights ) );
					delta.zw() = abs( L - vec2( Lright, Lbottom ) );

					// Calculate the maximum delta in the direct neighborhood:
					auto maxDelta = writer.declLocale( "maxDelta"
						, max( delta.xy(), delta.zw() ) );

					// Calculate left-left and top-top deltas:
					auto Lleftleft = writer.declLocale( "Lleftleft"
						, dot( c3d_colourTex.sample( offset[2].xy() ).rgb(), weights ) );
					auto Ltoptop = writer.declLocale( "Ltoptop"
						, dot( c3d_colourTex.sample( offset[2].zw() ).rgb(), weights ) );
					delta.zw() = abs( vec2( Lleft, Ltop ) - vec2( Lleftleft, Ltoptop ) );

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

			writer.implementEntryPointT< EDVertexT, c3ds::Colour4FT >( [&SMAALumaEdgeDetectionPS]( sdw::FragmentInT< EDVertexT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					out.colour() = vec4( 0.0_f );
					out.colour().xy() = SMAALumaEdgeDetectionPS( in.texcoord(), in.offset() );
				} );

			return c3d::makeRawUnique< sdw::Shader >( c3d::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	LumaEdgeDetection::LumaEdgeDetection( crg::FramePassGroup & graph
		, c3d::RenderTarget & renderTarget
		, c3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, c3d::Texture const & colourView
		, c3d::Texture const * predication
		, SmaaConfig const & config
		, bool const * enabled
		, uint32_t const * passIndex )
		: EdgeDetection{ graph
			, renderTarget
			, device
			, ubo
			, config
			, lumaed::getProgram( device, predication != nullptr )
			, enabled
			, passIndex
			, uint32_t( colourView.size() ) }
	{
		crg::SamplerDesc linearSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest };
		m_pass.addInputSampledT( *colourView.getSampledLastAttach(), lumaed::Bindings::ColorTexIdx, linearSampler );
		if ( predication )
			m_pass.addInputSampledT( *predication->getSampledLastAttach(), lumaed::Bindings::PredicationTexIdx, linearSampler );
	}
}

#include "SmaaPostEffect/DepthEdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

namespace smaa
{
	namespace dpthed
	{
		enum Idx : uint32_t
		{
			DepthTexIdx = SmaaUboIdx + 1,
		};

		static std::unique_ptr< ast::Shader > doGetEdgeDetectionFP( castor3d::Engine const & engine )
		{
			using namespace sdw;
			FragmentWriter writer;
			castor3d::shader::Utils utils{ writer };

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 1u, 3u );
			C3D_Smaa( writer, SmaaUboIdx, 0u );
			auto c3d_depthObjTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObjTex", DepthTexIdx, 0u );

			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< Vec3 >( "SMAAGatherNeighbours"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, CombinedImage2DRgba32 const & depthObjTex )
				{
					writer.returnStmt( depthObjTex.gather( texcoord + c3d_smaaData.rtMetrics.xy() * vec2( -0.5_f, -0.5_f ), 0_i ).grb() );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InCombinedImage2DRgba32{ writer, "depthObjTex" } );

			/**
			 * Depth Edge Detection
			 */
			auto SMAADepthEdgeDetectionPS = writer.implementFunction< Vec2 >( "SMAADepthEdgeDetectionPS"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, CombinedImage2DRgba32 const & depthTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, offset, depthTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_smaaData.depthThreshold ), delta ) );

					IF( writer, dot( edges, vec2( 1.0_f, 1.0_f ) ) == 0.0_f )
					{
						writer.demote();
					}
					FI;

					writer.returnStmt( edges );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InCombinedImage2DRgba32{ writer, "depthTex" } );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					outColour = vec4( 0.0_f );
					outColour.xy() = SMAADepthEdgeDetectionPS( utils.topDownToBottomUp( vtx_texture ), vtx_offset, c3d_depthObjTex );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	DepthEdgeDetection::DepthEdgeDetection( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, crg::ImageViewId const & depthObj
		, SmaaConfig const & config
		, bool const * enabled )
		: EdgeDetection{ graph
			, previousPass
			, renderTarget
			, device
			, ubo
			, config
			, dpthed::doGetEdgeDetectionFP( *renderTarget.getEngine() )
			, enabled
			, nullptr
			, 1u }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addSampledView( depthObj
			, dpthed::DepthTexIdx
			, linearSampler );
	}
}

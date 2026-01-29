#include "SmaaPostEffect/DepthEdgeDetection.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
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
	namespace dpthed
	{
		namespace c3ds = c3d::shader;

		enum class Bindings : uint32_t
		{
			DepthTexIdx = uint32_t( smaa::Bindings::SmaaUboIdx ) + 1u,
		};

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };
			c3ds::Utils utils{ writer };

			// Shader inputs
			C3D_Smaa( writer, smaa::Bindings::SmaaUboIdx, 0u );
			auto c3d_depthObjTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObjTex", Bindings::DepthTexIdx, 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< sdw::Vec3 >( "SMAAGatherNeighbours"
				, [&writer, &c3d_smaaData]( sdw::Vec2 const & texcoord
					, sdw::CombinedImage2DRgba32 const & depthObjTex )
				{
					writer.returnStmt( depthObjTex.gather( texcoord + c3d_smaaData.rtMetrics.xy() * vec2( -0.5_f, -0.5_f ), 0_i ).grb() );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InCombinedImage2DRgba32{ writer, "depthObjTex" } );

			/**
			 * Depth Edge Detection
			 */
			auto SMAADepthEdgeDetectionPS = writer.implementFunction< sdw::Vec2 >( "SMAADepthEdgeDetectionPS"
				, [&writer, &SMAAGatherNeighbours, &c3d_smaaData]( sdw::Vec2 const & texcoord
					, sdw::CombinedImage2DRgba32 const & depthTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, depthTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_smaaData.depthThreshold ), delta ) );

					sdwIF( writer, dot( edges, vec2( 1.0_f, 1.0_f ) ) == 0.0_f )
					{
						writer.demote();
					}
					sdwFI

					writer.returnStmt( edges );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InCombinedImage2DRgba32{ writer, "depthTex" } );

			EdgeDetection::getVertexProgram( writer, c3d_smaaData );

			writer.implementEntryPointT< EDVertexT, c3ds::Colour4FT >( [&utils, &SMAADepthEdgeDetectionPS, &c3d_depthObjTex]( sdw::FragmentInT< EDVertexT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					out.colour() = vec4( 0.0_f );
					out.colour().xy() = SMAADepthEdgeDetectionPS( utils.topDownToBottomUp( in.texcoord() ), c3d_depthObjTex );
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	DepthEdgeDetection::DepthEdgeDetection( crg::FramePassGroup & graph
		, c3d::RenderTarget & renderTarget
		, c3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, c3d::Texture const & depthObj
		, SmaaConfig const & config
		, bool const * enabled )
		: EdgeDetection{ graph
			, renderTarget
			, device
			, ubo
			, config
			, dpthed::getProgram( device )
			, enabled
			, nullptr
			, 1u }
	{
		m_pass.addInputSampledT( *depthObj.getSampledLastAttach(), dpthed::Bindings::DepthTexIdx
			, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest } );
	}
}

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
		namespace c3d = castor3d::shader;

		enum Idx : uint32_t
		{
			DepthTexIdx = SmaaUboIdx + 1,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };
			c3d::Utils utils{ writer };

			// Shader inputs
			C3D_Smaa( writer, SmaaUboIdx, 0u );
			auto c3d_depthObjTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObjTex", DepthTexIdx, 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< sdw::Vec3 >( "SMAAGatherNeighbours"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec4Array const & offset
					, sdw::CombinedImage2DRgba32 const & depthObjTex )
				{
					writer.returnStmt( depthObjTex.gather( texcoord + c3d_smaaData.rtMetrics.xy() * vec2( -0.5_f, -0.5_f ), 0_i ).grb() );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec4Array{ writer, "offset", 3u }
				, sdw::InCombinedImage2DRgba32{ writer, "depthObjTex" } );

			/**
			 * Depth Edge Detection
			 */
			auto SMAADepthEdgeDetectionPS = writer.implementFunction< sdw::Vec2 >( "SMAADepthEdgeDetectionPS"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec4Array const & offset
					, sdw::CombinedImage2DRgba32 const & depthTex )
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
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec4Array{ writer, "offset", 3u }
				, sdw::InCombinedImage2DRgba32{ writer, "depthTex" } );

			EdgeDetection::getVertexProgram( writer, c3d_smaaData );

			writer.implementEntryPointT< EDVertexT, c3d::Colour4FT >( [&]( sdw::FragmentInT< EDVertexT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					out.colour() = vec4( 0.0_f );
					out.colour().xy() = SMAADepthEdgeDetectionPS( utils.topDownToBottomUp( in.texcoord() ), in.offset(), c3d_depthObjTex );
				} );
			return writer.getBuilder().releaseShader();
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
			, dpthed::getProgram( device )
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

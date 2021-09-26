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

using namespace castor;

namespace smaa
{
	namespace
	{
		enum Idx : uint32_t
		{
			DepthTexIdx,
		};

		std::unique_ptr< ast::Shader > doGetEdgeDetectionFP( castor3d::Engine const & engine
			, castor::Size const & size
			, SmaaConfig const & config )
		{
			auto renderTargetMetrics = Point4f{ 1.0f / float( size.getWidth() )
				, 1.0f / float( size.getHeight() )
				, float( size.getWidth() )
				, float( size.getHeight() ) };

			using namespace sdw;
			FragmentWriter writer;

			// Shader constants
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( config.data.threshold ) );
			auto c3d_depthThreshold = writer.declConstant( constants::DepthThreshold
				, c3d_threshold * 0.1_f );
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			castor3d::shader::Utils utils{ writer, engine };
			utils.declareInvertVec2Y();

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( "vtx_offset", 1u, 3u );
			auto c3d_depthTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_depthTex", DepthTexIdx, 0u );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			/**
			 * Gathers current pixel, and the top-left neighbors.
			 */
			auto SMAAGatherNeighbours = writer.implementFunction< Vec3 >( "SMAAGatherNeighbours"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & tex )
				{
					writer.returnStmt( tex.gather( texcoord + c3d_rtMetrics.xy() * vec2( -0.5_f, -0.5_f ) ).grb() );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "tex" } );

			/**
			 * Depth Edge Detection
			 */
			auto SMAADepthEdgeDetectionPS = writer.implementFunction< Vec2 >( "SMAADepthEdgeDetectionPS"
				, [&]( Vec2 const & texcoord
					, Array< Vec4 > const & offset
					, SampledImage2DRgba32 const & depthTex )
				{
					auto neighbours = writer.declLocale( "neighbours"
						, SMAAGatherNeighbours( texcoord, offset, depthTex ) );
					auto delta = writer.declLocale( "delta"
						, abs( neighbours.xx() - neighbours.yz() ) );
					auto edges = writer.declLocale( "edges"
						, step( vec2( c3d_depthThreshold ), delta ) );

					IF( writer, dot( edges, vec2( 1.0_f, 1.0_f ) ) == 0.0_f )
					{
						writer.discard();
					}
					FI;

					writer.returnStmt( edges );
				}
				, InVec2{ writer, "texcoord" }
				, InVec4Array{ writer, "offset", 3u }
				, InSampledImage2DRgba32{ writer, "depthTex" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = vec4( 0.0_f );
					pxl_fragColour.xy() = SMAADepthEdgeDetectionPS( utils.topDownToBottomUp( vtx_texture ), vtx_offset, c3d_depthTex );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		crg::ImageViewData doCreateDepthView( crg::ImageViewId const & depthView )
		{
			return crg::ImageViewData{ "SMDEDTgt"
				, depthView.data->image
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, depthView.data->info.format
				, { VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u } };
		}
	}

	//*********************************************************************************************

	DepthEdgeDetection::DepthEdgeDetection( crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & depthView
		, SmaaConfig const & config
		, bool const * enabled )
		: EdgeDetection{ previousPass
			, renderTarget
			, device
			, config
			, doGetEdgeDetectionFP( *renderTarget.getEngine(), renderTarget.getSize(), config )
			, enabled }
		, m_depthView{ renderTarget.getGraph().createView( doCreateDepthView( depthView ) ) }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addSampledView( m_pass.mergeViews( { m_depthView, depthView } )
			, DepthTexIdx
			, {}
			, linearSampler );
	}
}

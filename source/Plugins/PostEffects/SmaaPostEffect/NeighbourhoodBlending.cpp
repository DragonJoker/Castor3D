#include "SmaaPostEffect/NeighbourhoodBlending.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>
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
			ColorTexIdx = SmaaUboIdx + 1,
			BlendTexIdx,
			VelocityTexIdx,
		};

		std::unique_ptr< ast::Shader > doGetNeighbourhoodBlendingVP()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader constants
			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );
			UBO_SMAA( writer, SmaaUboIdx, 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declOutput< Vec4 >( "vtx_offset", 1u );

			auto SMAANeighborhoodBlendingVS = writer.implementFunction< sdw::Void >( "SMAANeighborhoodBlendingVS"
				, [&]( Vec2 const & texcoord
					, Vec4 offset )
				{
					offset = fma( c3d_smaaData.rtMetrics.xyxy()
						, vec4( 1.0_f, 0.0_f, 0.0_f, 1.0_f )
						, vec4( texcoord.xy(), texcoord.xy() ) );
				}
				, InVec2{ writer, "texcoord" }
				, OutVec4{ writer, "offset" } );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
					vtx_offset = vec4( 0.0_f );
					SMAANeighborhoodBlendingVS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > doGetNeighbourhoodBlendingFP( bool reprojection )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInput< Vec4 >( "vtx_offset", 1u );
			UBO_SMAA( writer, SmaaUboIdx, 0u );
			auto c3d_colourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_colourTex", ColorTexIdx, 0u );
			auto c3d_blendTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_blendTex", BlendTexIdx, 0u );
			auto c3d_velocityTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_velocityTex", VelocityTexIdx, 0u, reprojection );

			/**
			 * Conditional move:
			 */
			auto SMAAMovc2 = writer.implementFunction< sdw::Void >( "SMAAMovc2"
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

			auto SMAAMovc4 = writer.implementFunction< sdw::Void >( "SMAAMovc4"
				, [&]( BVec4 const & cond
					, Vec4 variable
					, Vec4 const & value )
				{
					SMAAMovc2( cond.xy(), variable.xy(), value.xy() );
					SMAAMovc2( cond.zw(), variable.zw(), value.zw() );
				}
				, InBVec4{ writer, "cond" }
				, InOutVec4{ writer, "variable" }
				, InVec4{ writer, "value" } );

			auto SMAANeighborhoodBlendingPS = writer.implementFunction< Vec4 >( "SMAANeighborhoodBlendingPS"
				, [&]( Vec2 const & texcoord
					, Vec4 const & offset
					, SampledImage2DRgba32 const & colorTex
					, SampledImage2DRgba32 const & blendTex )
				{
					// Fetch the blending weights for current pixel:
					auto a = writer.declLocale< Vec4 >( "a" );
					a.x() = blendTex.sample( offset.xy() ).a(); // Right
					a.y() = blendTex.sample( offset.zw() ).g(); // Top
					a.wz() = blendTex.sample( texcoord ).xz(); // Bottom / Left

					// Is there any blending weight with a value greater than 0.0?
					IF ( writer, dot( a, vec4( 1.0_f, 1.0_f, 1.0_f, 1.0_f ) ) < 1e-5_f )
					{
						auto color = writer.declLocale( "color"
							, colorTex.lod( texcoord, 0.0_f ) );

						if ( reprojection )
						{
							auto velocity = writer.declLocale( "velocity"
								, c3d_velocityTex.lod( texcoord, 0.0_f ).rg() );

							// Pack velocity into the alpha channel:
							color.a() = sqrt( 5.0_f * length( velocity ) );
						}

						writer.returnStmt( color );
					}
					ELSE
					{
						auto h = writer.declLocale( "h"
							, max( a.x(), a.z() ) > max( a.y(), a.w() ) ); // max(horizontal) > max(vertical)

						// Calculate the blending offsets:
						auto blendingOffset = writer.declLocale( "blendingOffset"
							, vec4( 0.0_f, a.y(), 0.0_f, a.w() ) );
						auto blendingWeight = writer.declLocale( "blendingWeight"
							, a.yw() );
						SMAAMovc4( bvec4( h, h, h, h ), blendingOffset, vec4( a.x(), 0.0_f, a.z(), 0.0_f ) );
						SMAAMovc2( bvec2( h, h ), blendingWeight, a.xz() );
						blendingWeight /= dot( blendingWeight, vec2( 1.0_f, 1.0_f ) );

						// Calculate the texture coordinates:
						auto blendingCoord = writer.declLocale( "blendingCoord"
							, fma( blendingOffset
								, vec4( c3d_smaaData.rtMetrics.xy(), -c3d_smaaData.rtMetrics.xy() )
								, vec4( texcoord.xy(), texcoord.xy() ) ) );

						// We exploit bilinear filtering to mix current pixel with the chosen
						// neighbor:
						auto color = writer.declLocale( "color"
							, blendingWeight.x() * colorTex.lod( blendingCoord.xy(), 0.0_f ) );
						color += blendingWeight.y() * colorTex.lod( blendingCoord.zw(), 0.0_f );

						if ( reprojection )
						{
							// Antialias velocity for proper reprojection in a later stage:
							auto velocity = writer.declLocale( "velocity"
								, blendingWeight.x() * c3d_velocityTex.lod( blendingCoord.xy(), 0.0_f ).rg() );
							velocity += blendingWeight.y() * c3d_velocityTex.lod( blendingCoord.zw(), 0.0_f ).rg();

							// Pack velocity into the alpha channel:
							color.a() = sqrt( 5.0_f * length( velocity ) );
						}

						writer.returnStmt( color );
					}
					FI;
				}
				, InVec2{ writer, "texcoord" }
				, InVec4{ writer, "offset" }
				, InSampledImage2DRgba32{ writer, "colourTex" }
				, InSampledImage2DRgba32{ writer, "blendTex" } );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					pxl_fragColour = SMAANeighborhoodBlendingPS( vtx_texture, vtx_offset, c3d_colourTex, c3d_blendTex );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	NeighbourhoodBlending::NeighbourhoodBlending( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, crg::ImageViewId const & sourceView
		, crg::ImageViewId const & blendView
		, crg::ImageViewId const * velocityView
		, SmaaConfig const & config
		, bool const * enabled )
		: m_device{ device }
		, m_graph{ graph }
		, m_sourceView{ sourceView }
		, m_blendView{ blendView }
		, m_velocityView{ velocityView }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SmaaNeighbourhood", doGetNeighbourhoodBlendingVP() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaNeighbourhood", doGetNeighbourhoodBlendingFP( velocityView != nullptr ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_pass{ m_graph.createPass( "SmaaNeighbourhood"
			, [this, &device, &config, enabled]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.passIndex( &config.subsampleIndex )
					.enabled( enabled )
					.build( pass, context, graph, { config.maxSubsampleIndices } );
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/SMAA"
					, result->getTimer() );
				return result;
			} ) }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addDependency( previousPass );
		ubo.createPassBinding( m_pass
			, SmaaUboIdx );
		m_pass.addSampledView( m_sourceView
			, ColorTexIdx
			, {}
			, linearSampler );
		m_pass.addSampledView( m_blendView
			, BlendTexIdx
			, {}
			, linearSampler );

		if ( m_velocityView )
		{
			m_pass.addSampledView( *m_velocityView
				, VelocityTexIdx
				, {}
				, linearSampler );
		}

		for ( uint32_t i = 0; i < config.maxSubsampleIndices; ++i )
		{
			m_images.emplace_back( std::make_shared< castor3d::Texture >( m_device
				, m_graph.getHandler()
				, "SMNBRes" + std::to_string( i )
				, 0u
				, m_extent
				, 1u
				, 1u
				, VK_FORMAT_R8G8B8A8_SRGB
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) ) );
			auto & image = m_images.back();
			image->create();
			m_imageViews.push_back( image->wholeViewId );
		}

		m_pass.addOutputColourView( m_imageViews
			, castor3d::transparentBlackClearColor );
	}

	NeighbourhoodBlending::~NeighbourhoodBlending()
	{
		for ( auto & image : m_images )
		{
			image->destroy();
		}
	}

	void NeighbourhoodBlending::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );

		for ( uint32_t i = 0; i < m_images.size(); ++i )
		{
			visitor.visit( "SMAA NeighbourhoodBlending " + std::to_string( i )
				, *m_images[i]
				, m_graph.getFinalLayoutState( m_images[i]->wholeViewId ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}
	}

	//*********************************************************************************************
}

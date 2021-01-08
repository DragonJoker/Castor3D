#include "SmaaPostEffect/NeighbourhoodBlending.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
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
		std::unique_ptr< ast::Shader > doGetNeighbourhoodBlendingVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader constants
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declOutput< Vec4 >( "vtx_offset", 1u );
			auto out = writer.getOut();

			auto SMAANeighborhoodBlendingVS = writer.implementFunction< sdw::Void >( "SMAANeighborhoodBlendingVS"
				, [&]( Vec2 const & texcoord
					, Vec4 offset )
				{
					offset = fma( c3d_rtMetrics.xyxy()
						, vec4( 1.0_f, 0.0_f, 0.0_f, 1.0_f )
						, vec4( texcoord.xy(), texcoord.xy() ) );
				}
				, InVec2{ writer, "texcoord" }
				, OutVec4{ writer, "offset" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
					vtx_offset = vec4( 0.0_f );
					SMAANeighborhoodBlendingVS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		enum Idx : uint32_t
		{
			ColorTexIdx,
			BlendTexIdx,
			VelocityTexIdx,
		};

		std::unique_ptr< ast::Shader > doGetNeighbourhoodBlendingFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config
			, bool reprojection )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader constants
			auto c3d_rtMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declInput< Vec4 >( "vtx_offset", 1u );
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
								, vec4( c3d_rtMetrics.xy(), -c3d_rtMetrics.xy() )
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

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = SMAANeighborhoodBlendingPS( vtx_texture, vtx_offset, c3d_colourTex, c3d_blendTex );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::rq::BindingDescriptionArray createBindings( bool reprojection )
		{
			castor3d::rq::BindingDescriptionArray result
			{
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
			};

			if ( reprojection )
			{
				result.push_back( { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D } );
			}

			return result;
		}
	}

	//*********************************************************************************************

	NeighbourhoodBlending::NeighbourhoodBlending( castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, ashes::ImageView const & sourceView
		, ashes::ImageView const & blendView
		, ashes::ImageView const * velocityView
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ device
			, "SmaaNeighbourhoodBlending"
			, VK_FILTER_LINEAR
			, { createBindings( velocityView )
				, ashes::nullopt
				, castor3d::rq::Texcoord{} } }
		, m_sourceView{ sourceView }
		, m_blendView{ blendView }
		, m_velocityView{ velocityView }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName() }
	{
		static constexpr VkFormat colourFormat = VK_FORMAT_R8G8B8A8_SRGB;

		VkExtent2D size{ sourceView.image->getDimensions().width, sourceView.image->getDimensions().height };
		auto & renderSystem = m_renderSystem;

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				colourFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			}
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				ashes::nullopt,
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attachments ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = m_device->createRenderPass( getName()
			, std::move( createInfo ) );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader.shader = doGetNeighbourhoodBlendingVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		m_pixelShader.shader = doGetNeighbourhoodBlendingFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config
			, velocityView != nullptr );

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( m_device, m_vertexShader ) );
		stages.push_back( makeShaderState( m_device, m_pixelShader ) );

		ashes::WriteDescriptorSetArray writes
		{
			makeDescriptorWrite( m_sourceView
				, getSampler().getSampler()
				, ColorTexIdx ),
			makeDescriptorWrite( m_blendView
				, getSampler().getSampler()
				, BlendTexIdx ),
		};

		if ( m_velocityView )
		{
			writes.emplace_back( makeDescriptorWrite( *m_velocityView
				, getSampler().getSampler()
				, VelocityTexIdx ) );
		}

		createPipelineAndPass( size
			, castor::Position{}
			, stages
			, *m_renderPass
			, writes
			, {} );

		for ( uint32_t i = 0; i < config.maxSubsampleIndices; ++i )
		{
			m_surfaces.emplace_back( *renderTarget.getEngine()
				, getName() );
			m_surfaces.back().initialise( m_device
				, *m_renderPass
				, castor::Size{ size.width, size.height }
				, colourFormat );
		}
	}

	castor3d::CommandsSemaphore NeighbourhoodBlending::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex
		, uint32_t index )
	{
		castor3d::CommandsSemaphore neighbourhoodBlendingCommands
		{
			m_device.graphicsCommandPool->createCommandBuffer( getName() ),
			m_device->createSemaphore( getName() )
		};
		auto & neighbourhoodBlendingCmd = *neighbourhoodBlendingCommands.commandBuffer;

		neighbourhoodBlendingCmd.begin();
		neighbourhoodBlendingCmd.beginDebugBlock(
			{
				"SMAA NeighbourhoodBlending",
				castor3d::makeFloatArray( getRenderSystem()->getEngine()->getNextRainbowColour() ),
			} );
		timer.beginPass( neighbourhoodBlendingCmd, passIndex );
		neighbourhoodBlendingCmd.beginRenderPass( *m_renderPass
			, *m_surfaces[index].frameBuffer
			, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerPass( neighbourhoodBlendingCmd );
		neighbourhoodBlendingCmd.endRenderPass();
		timer.endPass( neighbourhoodBlendingCmd, passIndex );
		neighbourhoodBlendingCmd.endDebugBlock();
		neighbourhoodBlendingCmd.end();

		return std::move( neighbourhoodBlendingCommands );
	}

	void NeighbourhoodBlending::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	//*********************************************************************************************
}

#include "NeighbourhoodBlending.hpp"

#include "SmaaUbo.hpp"
#include "SMAA.hpp"

#include <Engine.hpp>

#include <Render/RenderPassTimer.hpp>
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
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		std::unique_ptr< sdw::Shader > doGetNeighbourhoodBlendingVP( castor3d::RenderSystem & renderSystem
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
			auto texcoord = writer.declInput< Vec2 >( "texcoord", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_offset = writer.declOutput< Vec4 >( "vtx_offset", 1u );
			auto out = writer.getOut();

			auto SMAANeighborhoodBlendingVS = writer.implementFunction< sdw::Void >( "SMAANeighborhoodBlendingVS"
				, [&]( Vec2 const & texcoord
					, Vec4 offset )
				{
					offset = fma( c3d_rtMetrics.xyxy()
						, vec4( 1.0_f, 0.0, 0.0, 1.0 )
						, vec4( texcoord.xy(), texcoord.xy() ) );
				}
				, InVec2{ writer, "texcoord" }
				, OutVec4{ writer, "offset" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;
					SMAANeighborhoodBlendingVS( vtx_texture, vtx_offset );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > doGetNeighbourhoodBlendingFP( castor3d::RenderSystem & renderSystem
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
			auto c3d_colourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_colourTex", 0u, 0u );
			auto c3d_blendTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_blendTex", 1u, 0u );
			auto c3d_velocityTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_velocityTex", 2u, 0u, reprojection );

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
					a.x() = texture( blendTex, offset.xy() ).a(); // Right
					a.y() = texture( blendTex, offset.zw() ).g(); // Top
					a.wz() = texture( blendTex, texcoord ).xz(); // Bottom / Left

					// Is there any blending weight with a value greater than 0.0?
					IF ( writer, dot( a, vec4( 1.0_f, 1.0, 1.0, 1.0 ) ) < 1e-5_f )
					{
						auto color = writer.declLocale( "color"
							, textureLod( colorTex, texcoord, 0.0_f ) );

						if ( reprojection )
						{
							auto velocity = writer.declLocale( "velocity"
								, textureLod( c3d_velocityTex, texcoord, 0.0_f ).rg() );

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
							, vec4( 0.0_f, a.y(), 0.0, a.w() ) );
						auto blendingWeight = writer.declLocale( "blendingWeight"
							, a.yw() );
						SMAAMovc4( bvec4( h, h, h, h ), blendingOffset, vec4( a.x(), 0.0, a.z(), 0.0 ) );
						SMAAMovc2( bvec2( h, h ), blendingWeight, a.xz() );
						blendingWeight /= dot( blendingWeight, vec2( 1.0_f, 1.0 ) );

						// Calculate the texture coordinates:
						auto blendingCoord = writer.declLocale( "blendingCoord"
							, fma( blendingOffset
								, vec4( c3d_rtMetrics.xy(), -c3d_rtMetrics.xy() )
								, vec4( texcoord.xy(), texcoord.xy() ) ) );

						// We exploit bilinear filtering to mix current pixel with the chosen
						// neighbor:
						auto color = writer.declLocale( "color"
							, blendingWeight.x() * textureLod( colorTex, blendingCoord.xy(), 0.0_f ) );
						color += blendingWeight.y() * textureLod( colorTex, blendingCoord.zw(), 0.0_f );

						if ( reprojection )
						{
							// Antialias velocity for proper reprojection in a later stage:
							auto velocity = writer.declLocale( "velocity"
								, blendingWeight.x() * textureLod( c3d_velocityTex, blendingCoord.xy(), 0.0_f ).rg() );
							velocity += blendingWeight.y() * textureLod( c3d_velocityTex, blendingCoord.zw(), 0.0_f ).rg();

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
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	NeighbourhoodBlending::NeighbourhoodBlending( castor3d::RenderTarget & renderTarget
		, ashes::TextureView const & sourceView
		, ashes::TextureView const & blendView
		, ashes::TextureView const * velocityView
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), false, false }
		, m_sourceView{ sourceView }
		, m_blendView{ blendView }
		, m_velocityView{ velocityView }
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "SmaaNeighbourhoodBlending" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "SmaaNeighbourhoodBlending" }
	{
		ashes::Extent2D size{ sourceView.getTexture().getDimensions().width, sourceView.getTexture().getDimensions().height };
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = ashes::Format::eR8G8B8A8_SRGB;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader.shader = doGetNeighbourhoodBlendingVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config );
		m_pixelShader.shader = doGetNeighbourhoodBlendingFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, config
			, velocityView != nullptr );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( renderSystem.compileShader( m_vertexShader ) );
		stages[1].module->loadShader( renderSystem.compileShader( m_pixelShader ) );

		ashes::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		auto * view = &m_blendView;

		if ( m_velocityView )
		{
			setLayoutBindings.emplace_back( 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
			view = m_velocityView;
		}

		createPipeline( size
			, castor::Position{}
			, stages
			, *view
			, *m_renderPass
			, setLayoutBindings
			, {} );

		for ( uint32_t i = 0; i < config.maxSubsampleIndices; ++i )
		{
			m_surfaces.emplace_back( *renderTarget.getEngine() );
			m_surfaces.back().initialise( *m_renderPass
				, castor::Size{ size.width, size.height }
				, ashes::Format::eR8G8B8A8_SRGB );
		}
	}

	castor3d::CommandsSemaphore NeighbourhoodBlending::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex
		, uint32_t index )
	{
		auto & device = getCurrentDevice( m_renderSystem );
		castor3d::CommandsSemaphore neighbourhoodBlendingCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & neighbourhoodBlendingCmd = *neighbourhoodBlendingCommands.commandBuffer;

		neighbourhoodBlendingCmd.begin();
		timer.beginPass( neighbourhoodBlendingCmd, passIndex );
		// Put blending weights image in shader input layout.
		neighbourhoodBlendingCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_blendView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

		if ( m_velocityView )
		{
			// Put velocity image in shader input layout.
			neighbourhoodBlendingCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eFragmentShader
				, m_velocityView->makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
		}

		neighbourhoodBlendingCmd.beginRenderPass( *m_renderPass
			, *m_surfaces[index].frameBuffer
			, { ashes::ClearColorValue{} }
			, ashes::SubpassContents::eInline );
		registerFrame( neighbourhoodBlendingCmd );
		neighbourhoodBlendingCmd.endRenderPass();
		timer.endPass( neighbourhoodBlendingCmd, passIndex );
		neighbourhoodBlendingCmd.end();

		return std::move( neighbourhoodBlendingCommands );
	}

	void NeighbourhoodBlending::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "NeighbourhoodBlending" )
			, ashes::ShaderStageFlag::eVertex
			, *m_vertexShader.shader );
		visitor.visit( cuT( "NeighbourhoodBlending" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
	}

	void NeighbourhoodBlending::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_sourceView
			, m_sampler->getSampler() );

		if ( m_velocityView )
		{
			descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
				, m_blendView
				, m_sampler->getSampler() );
		}
	}

	//*********************************************************************************************
}

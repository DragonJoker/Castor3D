#include "NeighbourhoodBlending.hpp"

#include "SmaaUbo.hpp"

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

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doGetNeighbourhoodBlendingVP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_renderTargetMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declOutput< Vec4 >( cuT( "vtx_offset" ), 1u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;

					vtx_offset = fma( c3d_renderTargetMetrics.xyxy(), vec4( 1.0_f, 0.0, 0.0, 1.0 ), vec4( vtx_texture, vtx_texture ) );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetNeighbourhoodBlendingFP( castor3d::RenderSystem & renderSystem
			, Point4f const & renderTargetMetrics
			, bool reprojection )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			auto c3d_renderTargetMetrics = writer.declConstant( constants::RenderTargetMetrics
				, vec4( Float( renderTargetMetrics[0] ), renderTargetMetrics[1], renderTargetMetrics[2], renderTargetMetrics[3] ) );

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declInput< Vec4 >( cuT( "vtx_offset" ), 1u );
			auto c3d_colourTex = writer.declSampler< Sampler2D >( cuT( "c3d_colourTex" ), 0u, 0u );
			auto c3d_blendTex = writer.declSampler< Sampler2D >( cuT( "c3d_blendTex" ), 1u, 0u );
			auto c3d_velocityTex = writer.declSampler< Sampler2D >( cuT( "c3d_velocityTex" ), 2u, 0u, reprojection );

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

			auto movc4 = writer.implementFunction< Void >( cuT( "movc4" )
				, [&]( BVec4 const & cond
					, Vec4 variable
					, Vec4 const & value )
				{
					writer << movc2( cond.xy(), variable.xy(), value.xy() ) << endi;
					writer << movc2( cond.zw(), variable.zw(), value.zw() ) << endi;
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
					a.x() = texture( c3d_blendTex, vtx_offset.xy() ).a(); // Right
					a.y() = texture( c3d_blendTex, vtx_offset.zw() ).g(); // Top
					a.wz() = texture( c3d_blendTex, texcoord ).xz(); // Bottom / Left

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
						auto h = writer.declLocale( cuT( "h" )
							, max( a.x(), a.z() ) > max( a.y(), a.w() ) ); // max(horizontal) > max(vertical)

						// Calculate the blending offsets:
						auto blendingOffset = writer.declLocale( cuT( "blendingOffset" )
							, vec4( 0.0_f, a.y(), 0.0, a.w() ) );
						auto blendingWeight = writer.declLocale( cuT( "blendingWeight" )
							, a.yw() );
						writer << movc4( bvec4( h, h, h, h ), blendingOffset, vec4( a.x(), 0.0, a.z(), 0.0 ) ) << endi;
						writer << movc2( bvec2( h, h ), blendingWeight, a.xz() ) << endi;
						blendingWeight /= dot( blendingWeight, vec2( 1.0_f, 1.0 ) );

						// Calculate the texture coordinates:
						auto blendingCoord = writer.declLocale( cuT( "blendingCoord" )
							, fma( blendingOffset, vec4( c3d_renderTargetMetrics.xy(), -c3d_renderTargetMetrics.xy() ), vec4( texcoord, texcoord ) ) );

						// We exploit bilinear filtering to mix current pixel with the chosen
						// neighbor:
						pxl_fragColour = vec4( blendingWeight.x() ) * texture( c3d_colourTex, blendingCoord.xy(), 0.0 );
						pxl_fragColour += vec4( blendingWeight.y() ) * texture( c3d_colourTex, blendingCoord.zw(), 0.0 );

						if ( reprojection )
						{
							// Antialias velocity for proper reprojection in a later stage:
							auto velocity = writer.declLocale( cuT( "velocity" )
								, vec2( blendingWeight.x() ) * texture( c3d_velocityTex, blendingCoord.xy(), 0.0 ).rg() );
							velocity += vec2( blendingWeight.y() ) * texture( c3d_velocityTex, blendingCoord.zw(), 0.0 ).rg();

							// Pack velocity into the alpha channel:
							pxl_fragColour.a() = sqrt( 5.0 * length( velocity ) );
						}
					}
					FI;
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	NeighbourhoodBlending::NeighbourhoodBlending( castor3d::RenderTarget & renderTarget
		, renderer::TextureView const & sourceView
		, renderer::TextureView const & blendView
		, renderer::TextureView const * velocityView
		, castor3d::SamplerSPtr sampler
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_sourceView{ sourceView }
		, m_blendView{ blendView }
		, m_velocityView{ velocityView }
	{
		renderer::Extent2D size{ sourceView.getTexture().getDimensions().width, sourceView.getTexture().getDimensions().height };
		auto & renderSystem = *renderTarget.getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = renderer::Format::eR8G8B8A8_SRGB;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite | renderer::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader = doGetNeighbourhoodBlendingVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize );
		m_pixelShader = doGetNeighbourhoodBlendingFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, velocityView != nullptr );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_vertexShader.getSource() );
		stages[1].module->loadShader( m_pixelShader.getSource() );

		renderer::DescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		auto * view = &m_blendView;

		if ( m_velocityView )
		{
			setLayoutBindings.emplace_back( 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
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
				, renderer::Format::eR8G8B8A8_SRGB );
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

		if ( neighbourhoodBlendingCmd.begin() )
		{
			timer.beginPass( neighbourhoodBlendingCmd, passIndex );
			// Put blending weights image in shader input layout.
			neighbourhoodBlendingCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_blendView.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

			if ( m_velocityView )
			{
				// Put velocity image in shader input layout.
				neighbourhoodBlendingCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::PipelineStageFlag::eFragmentShader
					, m_velocityView->makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			}

			neighbourhoodBlendingCmd.beginRenderPass( *m_renderPass
				, *m_surfaces[index].frameBuffer
				, { renderer::ClearColorValue{} }
				, renderer::SubpassContents::eInline );
			registerFrame( neighbourhoodBlendingCmd );
			neighbourhoodBlendingCmd.endRenderPass();
			timer.endPass( neighbourhoodBlendingCmd, passIndex );
			neighbourhoodBlendingCmd.end();
		}

		return std::move( neighbourhoodBlendingCommands );
	}

	void NeighbourhoodBlending::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "NeighbourhoodBlending" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "NeighbourhoodBlending" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	void NeighbourhoodBlending::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
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

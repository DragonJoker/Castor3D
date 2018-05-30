#include "NeighbourhoodBlending.hpp"

#include <Engine.hpp>

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

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;

namespace smaa
{
	namespace
	{
		glsl::Shader doGetNeighbourhoodBlendingVP( castor3d::RenderSystem & renderSystem
			, Point2f const & pixelSize )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declOutputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 2u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;

					vtx_offset[0] = vec4( vtx_texture, vtx_texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -1.0_f, 0.0, 0.0, -1.0 );
					vtx_offset[1] = vec4( vtx_texture, vtx_texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( 1.0_f, 0.0, 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetNeighbourhoodBlendingFP( castor3d::RenderSystem & renderSystem
			, Point2f const & pixelSize
			, bool reprojection )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();

			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 2u );
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

			auto movc4 = writer.implementFunction< Void >( cuT( "movc2" )
				, [&]( BVec4 const & cond
					, Vec4 variable
					, Vec4 const & value )
				{
					movc2( cond.xy(), variable.xy(), value.xy() );
					movc2( cond.zw(), variable.zw(), value.zw() );
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
					a.xz() = texture( c3d_blendTex, texcoord ).xz();
					a.y() = texture( c3d_blendTex, vtx_offset[1].zw() ).g();
					a.w() = texture( c3d_blendTex, vtx_offset[1].xy() ).a();

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
						auto color = writer.declLocale( cuT( "color" )
							, vec4( 0.0_f, 0.0, 0.0, 0.0 ) );

						// Up to 4 lines can be crossing a pixel (one through each edge). We
						// favor blending by choosing the line with the maximum weight for each
						// direction:
						auto offset = writer.declLocale< Vec2 >( cuT( "offset" ) );
						offset.x() = writer.ternary( a.a() > a.b()
							, a.a()
							, -a.b() ); // left vs. right 
						offset.y() = writer.ternary( a.g() > a.r()
							, a.g()
							, -a.r() ); // top vs. bottom

						// Then we go in the direction that has the maximum weight:
						IF( writer, abs( offset.x() ) > abs( offset.y() ) ) // horizontal vs. vertical
						{
							offset.y() = 0.0_f;
						}
						ELSE
						{
							offset.x() = 0.0_f;
						}
						FI;

						if ( reprojection )
						{
							// Fetch the opposite color and lerp by hand:
							auto C = writer.declLocale( cuT( "C" )
								, texture( c3d_colourTex, texcoord, 0.0_f ) );
							texcoord += sign( offset ) * c3d_pixelSize;
							auto Cop = writer.declLocale( cuT( "Cop" )
								, texture( c3d_colourTex, texcoord, 0.0_f ) );
							auto s = writer.declLocale( cuT( "s" )
								, writer.ternary( abs( offset.x() ) > abs( offset.y() )
									, abs( offset.x() )
									, abs( offset.y() ) ) );

							// Unpack the velocity values:
							C.a() *= C.a();
							Cop.a() *= Cop.a();

							// Lerp the colors:
							auto Caa = writer.declLocale( cuT( "Caa" )
								, mix( C, Cop, s ) );

							// Unpack velocity and return the resulting value:
							Caa.a() = sqrt( Caa.a() );
							pxl_fragColour = Caa;
						}
						else
						{
							// We exploit bilinear filtering to mix current pixel with the chosen
							// neighbor:
							texcoord += offset * c3d_pixelSize;
							pxl_fragColour = texture( c3d_colourTex, texcoord, 0.0_f );
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
		, uint32_t maxSubsampleIndices )
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
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		auto pixelSize = Point2f{ 1.0f / size.width, 1.0f / size.height };
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

		for ( uint32_t i = 0; i < maxSubsampleIndices; ++i )
		{
			m_surfaces.emplace_back( *renderTarget.getEngine() );
			m_surfaces.back().initialise( *m_renderPass
				, castor::Size{ size.width, size.height }
				, renderer::Format::eR8G8B8A8_SRGB );
		}
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

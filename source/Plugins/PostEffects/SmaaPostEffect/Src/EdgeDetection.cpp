#include "EdgeDetection.hpp"

#include <Engine.hpp>

#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>

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
		glsl::Shader doGetEdgeDetectionVP( castor3d::RenderSystem & renderSystem
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
			auto vtx_offset = writer.declOutputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 3u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = vec4( position, 0.0, 1.0 );
					vtx_texture = texcoord;

					vtx_offset[0] = vec4( vtx_texture, vtx_texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -1.0_f, 0.0, 0.0, -1.0 );
					vtx_offset[1] = vec4( vtx_texture, vtx_texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( 1.0_f, 0.0, 0.0, 1.0 );
					vtx_offset[2] = vec4( vtx_texture, vtx_texture ) + vec4( c3d_pixelSize, c3d_pixelSize ) * vec4( -2.0_f, 0.0, 0.0, -2.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader doGetEdgeDetectionFP( castor3d::RenderSystem & renderSystem
			, Point2f const & pixelSize
			, float threshold
			, bool predicationEnabled
			, float predicationThreshold
			, float predicationScale
			, float predicationStrength )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem.createGlslWriter();
			writer.enableExtension( cuT( "GL_ARB_texture_gather" ), 400u );

			// Shader inputs
			auto c3d_pixelSize = writer.declConstant( constants::PixelSize
				, vec2( Float( pixelSize[0] ), pixelSize[1] ) );
			auto c3d_threshold = writer.declConstant( constants::Threshold
				, Float( threshold ) );
			auto c3d_predicationThreshold = writer.declConstant( constants::PredicationThreshold
				, Float( predicationThreshold ) );
			auto c3d_predicationScale = writer.declConstant( constants::PredicationScale
				, Float( predicationScale ) );
			auto c3d_predicationStrength = writer.declConstant( constants::PredicationStrength
				, Float( predicationStrength ) );

			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_offset = writer.declInputArray< Vec4 >( cuT( "vtx_offset" ), 1u, 3u );
			auto c3d_colourTex = writer.declSampler< Sampler2D >( cuT( "c3d_colourTex" ), 0u, 0u );
			auto c3d_predicationTex = writer.declSampler< Sampler2D >( cuT( "c3d_predicationTex" ), 1u, 0u, predicationEnabled );

			// Shader outputs
			auto pxl_fragColour = writer.declFragData< Vec4 >( cuT( "pxl_fragColour" ), 0u );

			/**
			* Gathers current pixel, and the top-left neighbors.
			*/
			Function< Vec2 > calculatePredicatedThreshold;

			if ( predicationEnabled )
			{
				auto gatherNeighbours = writer.implementFunction< Vec3 >( cuT( "gatherNeighbours" )
					, [&]()
					{
						writer.returnStmt( textureGather( c3d_predicationTex
							, vtx_texture + c3d_pixelSize * vec2( -0.5_f, -0.5 ) ).grb() );
					} );

				/**
				* Adjusts the threshold by means of predication.
				*/
				calculatePredicatedThreshold = writer.implementFunction< Vec2 >( cuT( "calculatePredicatedThreshold" )
					, [&]()
					{
						auto neighbours = writer.declLocale( cuT( "neighbours" )
							, gatherNeighbours() );
						auto delta = writer.declLocale( cuT( "delta" )
							, abs( neighbours.xx() - neighbours.yz() ) );
						auto edges = writer.declLocale( cuT( "edges" )
							, step( vec2( c3d_predicationThreshold ), delta ) );
						writer.returnStmt( c3d_predicationScale
							* c3d_threshold
							* writer.paren( vec2( 1.0_f ) - c3d_predicationStrength * edges ) );
					} );
			}

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					// Calculate the threshold:
					if ( predicationEnabled )
					{
						auto threshold = writer.declLocale( cuT( "threshold" )
							, calculatePredicatedThreshold() );
					}
					else
					{
						auto threshold = writer.declLocale( cuT( "threshold" )
							, vec2( c3d_threshold ) );
					}

					auto threshold = writer.getBuiltin< Vec2 >( cuT( "threshold" ) );

					// Calculate lumas:
					auto weights = writer.declLocale( cuT( "weights" )
						, vec3( 0.2126_f, 0.7152, 0.0722 ) );
					auto L = writer.declLocale( cuT( "L" )
						, dot( texture( c3d_colourTex, vtx_texture ).rgb(), weights ) );
					auto Lleft = writer.declLocale( cuT( "Lleft" )
						, dot( texture( c3d_colourTex, vtx_offset[0].xy() ).rgb(), weights ) );
					auto Ltop = writer.declLocale( cuT( "Ltop" )
						, dot( texture( c3d_colourTex, vtx_offset[0].zw() ).rgb(), weights ) );

					// We do the usual threshold:
					auto delta = writer.declLocale< Vec4 >( cuT( "delta" ) );
					delta.xy() = abs( vec2( L ) - vec2( Lleft, Ltop ) );
					auto edges = writer.declLocale( cuT( "edges" )
						, step( threshold, delta.xy() ) );

					// Then discard if there is no edge:
					IF( writer, dot( edges, vec2( 1.0_f ) ) == 0.0 )
					{
						writer.discard();
					}
					FI;

					// Calculate right and bottom deltas:
					auto Lright = writer.declLocale( cuT( "Lright" )
						, dot( texture( c3d_colourTex, vtx_offset[1].xy() ).rgb(), weights ) );
					auto Lbottom = writer.declLocale( cuT( "Lbottom" )
						, dot( texture( c3d_colourTex, vtx_offset[1].zw() ).rgb(), weights ) );
					delta.zw() = abs( vec2( L ) - vec2( Lright, Lbottom ) );

					// Calculate the maximum delta in the direct neighborhood:
					auto maxDelta = writer.declLocale( cuT( "maxDelta" )
						, max( delta.xy(), delta.zw() ) );
					maxDelta = max( maxDelta.xx(), maxDelta.yy() );

					// Calculate left-left and top-top deltas:
					auto Lleftleft = writer.declLocale( cuT( "Lleftleft" )
						, dot( texture( c3d_colourTex, vtx_offset[2].xy() ).rgb(), weights ) );
					auto Ltoptop = writer.declLocale( cuT( "Ltoptop" )
						, dot( texture( c3d_colourTex, vtx_offset[2].zw() ).rgb(), weights ) );
					delta.zw() = abs( vec2( Lleft, Ltop ) - vec2( Lleftleft, Ltoptop ) );

					// Calculate the final maximum delta:
					maxDelta = max( maxDelta.xy(), delta.zw() );

					/**
					* Each edge with a delta in luma of less than 50% of the maximum luma
					* surrounding this pixel is discarded. This allows to eliminate spurious
					* crossing edges, and is based on the fact that, if there is too much
					* contrast in a direction, that will hide contrast in the other
					* neighbors.
					* This is done after the discard intentionally as this situation doesn't
					* happen too frequently (but it's important to do as it prevents some
					* edges from going undetected).
					*/
					edges.xy() *= step( 0.5_f * maxDelta, delta.xy() );

					pxl_fragColour = vec4( edges, 0.0_f, 0.0_f );
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	EdgeDetection::EdgeDetection( castor3d::RenderTarget & renderTarget
		, renderer::TextureView const & colourView
		, renderer::TextureView const * predicationView
		, castor3d::SamplerSPtr sampler
		, float threshold
		, float predicationThreshold
		, float predicationScale
		, float predicationStrength )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_surface{ *renderTarget.getEngine() }
		, m_colourView{ colourView }
		, m_predicationView{ predicationView }
	{
		renderer::Extent2D size{ m_colourView.getTexture().getDimensions().width
			, m_colourView.getTexture().getDimensions().height };
		auto & device = *renderTarget.getEngine()->getRenderSystem()->getCurrentDevice();

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = m_colourView.getFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.attachments[1].format = renderer::Format::eS8_UINT;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].depthStencilAttachment = { 1u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };
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
		auto vertex = doGetEdgeDetectionVP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize );
		auto fragment = doGetEdgeDetectionFP( *renderTarget.getEngine()->getRenderSystem()
			, pixelSize
			, threshold
			, m_predicationView != nullptr
			, predicationThreshold
			, predicationScale
			, predicationStrength );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vertex.getSource() );
		stages[1].module->loadShader( fragment.getSource() );

		renderer::DepthStencilState dsstate
		{
			0u,
			false,
			false,
			renderer::CompareOp::eLess,
			false,
			true,
			{
				renderer::StencilOp::eReplace,
				renderer::StencilOp::eKeep,
				renderer::StencilOp::eKeep,
				renderer::CompareOp::eNever,
				0xFFFFFFFFu,
				0xFFFFFFFFu,
				0x00000001u,
			},
		};
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings;
		auto * view = &m_colourView;

		if ( m_predicationView )
		{
			setLayoutBindings.emplace_back( 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
			view = m_predicationView;
		}

		createPipeline( size
			, castor::Position{}
			, stages
			, *view
			, *m_renderPass
			, setLayoutBindings
			, {}
			, dsstate );
		m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, sampler
			, m_colourView.getFormat()
			, renderer::Format::eS8_UINT );
	}

	void EdgeDetection::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		if ( m_predicationView )
		{
			descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
				, m_colourView
				, m_sampler->getSampler() );
		}
	}
}

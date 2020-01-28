#include "SmaaPostEffect/Reproject.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"
#include "SmaaPostEffect/SMAA.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Texture/Sampler.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>

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
		std::unique_ptr< sdw::Shader > doGetReprojectVP( castor3d::RenderSystem const & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.gl_out.gl_Position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > doGetReprojectFP( castor3d::RenderSystem const & renderSystem
			, Point4f const & renderTargetMetrics
			, SmaaConfig const & config
			, bool reprojection )
		{
			using namespace sdw;
			FragmentWriter writer;
			auto c3d_reprojectionWeightScale = writer.declConstant( constants::ReprojectionWeightScale
				, Float( config.data.reprojectionWeightScale ) );

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto c3d_currentColourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_currentColourTex", 0u, 0u );
			auto c3d_previousColourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_previousColourTex", 1u, 0u );
			auto c3d_velocityTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_velocityTex", 2u, 0u, reprojection );

			// Shader outputs
			auto pxl_fragColour = writer.declOutput< Vec4 >( "pxl_fragColour", 0u );

			auto SMAAResolvePS = writer.implementFunction< Vec4 >( "SMAAResolvePS"
				, [&]( Vec2 const & texcoord
					, SampledImage2DRgba32 const & currentColorTex
					, SampledImage2DRgba32 const & previousColorTex )
				{
					if ( config.data.enableReprojection )
					{
						// Velocity is assumed to be calculated for motion blur, so we need to
						// inverse it for reprojection:
						auto velocity = writer.declLocale( "velocity"
							, -texture( c3d_velocityTex, texcoord ).rg() );

						// Fetch current pixel:
						auto current = writer.declLocale( "current"
							, texture( currentColorTex, texcoord ) );

						// Reproject current coordinates and fetch previous pixel:
						auto previous = writer.declLocale( "previous"
							, texture( previousColorTex, texcoord + velocity ) );

						// Attenuate the previous pixel if the velocity is different:
						auto delta = writer.declLocale( "delta"
							, abs( current.a() * current.a() - previous.a() * previous.a() ) / 5.0_f );
						auto weight = writer.declLocale( "weight"
							, 0.5_f * clamp( 1.0_f - sqrt( delta ) * c3d_reprojectionWeightScale, 0.0_f, 1.0_f ) );

						// Blend the pixels according to the calculated weight:
						writer.returnStmt( mix( current, previous, vec4( weight ) ) );
					}
					else
					{
						// Just blend the pixels:
						auto current = writer.declLocale( "current"
							, texture( currentColorTex, texcoord ) );
						auto previous = writer.declLocale( "previous"
							, texture( previousColorTex, texcoord ) );
						writer.returnStmt( mix( current, previous, vec4( 0.5_f ) ) );
					}
				}
				, InVec2{ writer, "texcoord" }
				, InSampledImage2DRgba32{ writer, "currentColorTex" }
				, InSampledImage2DRgba32{ writer, "previousColorTex" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColour = SMAAResolvePS( vtx_texture, c3d_currentColourTex, c3d_previousColourTex );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}
	
	//*********************************************************************************************

	Reproject::Reproject( castor3d::RenderTarget & renderTarget
		, ashes::ImageView const & currentColourView
		, ashes::ImageView const & previousColourView
		, ashes::ImageView const * velocityView
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ *renderTarget.getEngine()->getRenderSystem(), true, false }
		, m_currentColourView{ currentColourView }
		, m_previousColourView{ previousColourView }
		, m_velocityView{ velocityView }
		, m_surface{ *renderTarget.getEngine(), cuT( "SmaaReproject" ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SmaaReproject" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaReproject" }
	{
		VkExtent2D size{ m_currentColourView.image->getDimensions().width, m_currentColourView.image->getDimensions().height };

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				renderTarget.getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				std::nullopt,
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
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		setDebugObjectName( device, *m_renderPass, "Reproject" );

		auto pixelSize = Point4f{ 1.0f / size.width, 1.0f / size.height, float( size.width ), float( size.height ) };
		m_vertexShader.shader = doGetReprojectVP( m_renderSystem
			, pixelSize
			, config );
		m_pixelShader.shader = doGetReprojectFP( m_renderSystem
			, pixelSize
			, config
			, velocityView != nullptr );

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_vertexShader ) );
		stages.push_back( makeShaderState( device, m_pixelShader ) );

		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		auto * view = &m_previousColourView;

		if ( m_velocityView )
		{
			setLayoutBindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( 1u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			view = m_velocityView;
		}

		createPipeline( size
			, castor::Position{}
			, stages
			, *view
			, *m_renderPass
			, std::move( setLayoutBindings )
			, {} );
		m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, renderTarget.getPixelFormat() );
	}

	castor3d::CommandsSemaphore Reproject::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		castor3d::CommandsSemaphore reprojectCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & reprojectCmd = *reprojectCommands.commandBuffer;

		reprojectCmd.begin();
		timer.beginPass( reprojectCmd, passIndex );
		// Put neighbourhood images in shader input layout.
		reprojectCmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_currentColourView.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		reprojectCmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_previousColourView.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

		reprojectCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( reprojectCmd );
		reprojectCmd.endRenderPass();
		timer.endPass( reprojectCmd, passIndex );
		reprojectCmd.end();

		return std::move( reprojectCommands );
	}

	void Reproject::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "Reproject" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_vertexShader.shader );
		visitor.visit( cuT( "Reproject" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pixelShader.shader );
	}

	void Reproject::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_currentColourView
			, m_sampler->getSampler() );

		if ( m_velocityView )
		{
			descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
				, m_previousColourView
				, m_sampler->getSampler() );
		}
	}

	//*********************************************************************************************
}

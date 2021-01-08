#include "SmaaPostEffect/Reproject.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
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
		std::unique_ptr< ast::Shader > doGetReprojectVP( castor3d::RenderSystem const & renderSystem
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
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		enum Idx : uint32_t
		{
			CurColTexIdx,
			PrvColTexIdx,
			VelocityTexIdx,
		};

		std::unique_ptr< ast::Shader > doGetReprojectFP( castor3d::RenderSystem const & renderSystem
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
			auto c3d_currentColourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_currentColourTex", CurColTexIdx, 0u );
			auto c3d_previousColourTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_previousColourTex", PrvColTexIdx, 0u );
			auto c3d_velocityTex = writer.declSampledImage< FImg2DRgba32 >( "c3d_velocityTex", VelocityTexIdx, 0u, reprojection );

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
							, -c3d_velocityTex.sample( texcoord ).rg() );

						// Fetch current pixel:
						auto current = writer.declLocale( "current"
							, currentColorTex.sample( texcoord ) );

						// Reproject current coordinates and fetch previous pixel:
						auto previous = writer.declLocale( "previous"
							, previousColorTex.sample( texcoord + velocity ) );

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
							, currentColorTex.sample( texcoord ) );
						auto previous = writer.declLocale( "previous"
							, previousColorTex.sample( texcoord ) );
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
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::rq::BindingDescriptionArray createBindings()
		{
			return
			{
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
			};
		}
	}
	
	//*********************************************************************************************

	Reproject::Reproject( castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, ashes::ImageView const & currentColourView
		, ashes::ImageView const & previousColourView
		, ashes::ImageView const * velocityView
		, SmaaConfig const & config )
		: castor3d::RenderQuad{ device
			, cuT( "SmaaReproject" )
			, VK_FILTER_NEAREST
			, { createBindings()
				, ashes::nullopt
				, castor3d::rq::Texcoord{} } }
		, m_currentColourView{ currentColourView }
		, m_previousColourView{ previousColourView }
		, m_velocityView{ velocityView }
		, m_surface{ *renderTarget.getEngine(), getName() }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName() }
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
		m_vertexShader.shader = doGetReprojectVP( m_renderSystem
			, pixelSize
			, config );
		m_pixelShader.shader = doGetReprojectFP( m_renderSystem
			, pixelSize
			, config
			, velocityView != nullptr );

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( m_device, m_vertexShader ) );
		stages.push_back( makeShaderState( m_device, m_pixelShader ) );

		ashes::WriteDescriptorSetArray writes
		{
			makeDescriptorWrite( m_currentColourView
				, getSampler().getSampler()
				, CurColTexIdx ),
			makeDescriptorWrite( m_previousColourView
				, getSampler().getSampler()
				, PrvColTexIdx ),
		};

		if ( m_velocityView )
		{
			writes.push_back( makeDescriptorWrite( *m_velocityView
				, m_sampler->getSampler()
				, VelocityTexIdx ) );
		}

		createPipelineAndPass( size
			, castor::Position{}
			, stages
			, *m_renderPass
			, writes );
		m_surface.initialise( m_device
			, *m_renderPass
			, castor::Size{ size.width, size.height }
			, renderTarget.getPixelFormat() );
	}

	castor3d::CommandsSemaphore Reproject::prepareCommands( castor3d::RenderPassTimer const & timer
		, uint32_t passIndex )
	{
		castor3d::CommandsSemaphore reprojectCommands
		{
			m_device.graphicsCommandPool->createCommandBuffer( getName() ),
			m_device->createSemaphore( getName() )
		};
		auto & reprojectCmd = *reprojectCommands.commandBuffer;

		reprojectCmd.begin();
		reprojectCmd.beginDebugBlock(
			{
				"SMAA Reproject",
				castor3d::makeFloatArray( getRenderSystem()->getEngine()->getNextRainbowColour() ),
			} );
		timer.beginPass( reprojectCmd, passIndex );
		reprojectCmd.beginRenderPass( *m_renderPass
			, *m_surface.frameBuffer
			, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerPass( reprojectCmd );
		reprojectCmd.endRenderPass();
		timer.endPass( reprojectCmd, passIndex );
		reprojectCmd.endDebugBlock();
		reprojectCmd.end();

		return std::move( reprojectCommands );
	}

	void Reproject::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	//*********************************************************************************************
}

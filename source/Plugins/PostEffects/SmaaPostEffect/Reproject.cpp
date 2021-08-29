#include "SmaaPostEffect/Reproject.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
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

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace smaa
{
	namespace
	{
		enum Idx : uint32_t
		{
			CurColTexIdx,
			PrvColTexIdx,
			VelocityTexIdx,
		};

		std::unique_ptr< ast::Shader > doGetReprojectVP( VkExtent3D const & size
			, SmaaConfig const & config )
		{
			Point4f renderTargetMetrics{ 1.0f / size.width
				, 1.0f / size.height
				, float( size.width )
				, float( size.height ) };

			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > doGetReprojectFP( VkExtent3D const & size
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
	}
	
	//*********************************************************************************************

	Reproject::Reproject( crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & currentColourViews
		, crg::ImageViewIdArray const & previousColourViews
		, crg::ImageViewId const * velocityView
		, SmaaConfig const & config
		, bool const * enabled )
		: m_device{ device }
		, m_graph{ renderTarget.getGraph() }
		, m_currentColourViews{ currentColourViews }
		, m_previousColourViews{ previousColourViews }
		, m_velocityView{ velocityView }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SmaaReproject", doGetReprojectVP( m_extent, config ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaReproject", doGetReprojectFP( m_extent, config, velocityView != nullptr ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_result{ m_device
			, m_graph.getHandler()
			, "SMRpRes"
			, 0u
			, m_extent
			, 1u
			, 1u
			, renderTarget.getPixelFormat()
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) }
		, m_pass{ m_graph.createPass( "SmaaReproject"
			, [this, &device, &renderTarget, &config, &currentColourViews, enabled]( crg::FramePass const & pass
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
					.build( pass, context, graph, config.maxSubsampleIndices );
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/SMAA"
					, result->getTimer() );
				return result;
			} ) }
	{
		auto & handler = m_graph.getHandler();
		auto & context = m_device.makeContext();
		auto data = m_device.graphicsData();
		auto commandBuffer = data->commandPool->createCommandBuffer( "SmaaReprojectImagesClear" );
		commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

		for ( auto & view : m_currentColourViews )
		{
			ashes::ImagePtr image = std::make_unique< ashes::Image >( *device
				, handler.createImage( context, view.data->image )
				, view.data->image.data->info );
			auto createInfo = view.data->info;
			createInfo.image = *image;
			auto imageView = image->createView( createInfo );
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, imageView.makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			commandBuffer->clear( imageView
				, castor3d::opaqueBlackClearColor.color );
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, imageView.makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		}

		commandBuffer->end();
		data->queue->submit( *commandBuffer, nullptr );
		data->queue->waitIdle();
		commandBuffer.reset();

		crg::SamplerDesc pointSampler{ VK_FILTER_NEAREST
			, VK_FILTER_NEAREST
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addDependency( previousPass );
		m_pass.addOutputColourView( m_result.targetViewId );
		m_pass.addSampledView( m_currentColourViews
			, CurColTexIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, pointSampler );
		m_pass.addSampledView( m_previousColourViews
			, PrvColTexIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, pointSampler );
		m_result.create();

		if ( m_velocityView )
		{
			m_pass.addSampledView( *m_velocityView
				, VelocityTexIdx
				, {}
				, pointSampler );
		}
	}

	void Reproject::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( "SMAA Reprojection Result"
			, m_result
			, m_graph.getFinalLayout( m_result.sampledViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}

	//*********************************************************************************************
}

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

		std::unique_ptr< ast::Shader > doGetReprojectVP( castor::Size const & size
			, SmaaConfig const & config )
		{
			Point4f renderTargetMetrics{ 1.0f / size.getWidth()
				, 1.0f / size.getHeight()
				, float( size.getWidth() )
				, float( size.getHeight() ) };

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

		std::unique_ptr< ast::Shader > doGetReprojectFP( castor::Size const & size
			, SmaaConfig const & config
			, bool reprojection )
		{
			Point4f renderTargetMetrics{ 1.0f / size.getWidth()
				, 1.0f / size.getHeight()
				, float( size.getWidth() )
				, float( size.getHeight() ) };

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
		, SmaaConfig const & config )
		: m_device{ device }
		, m_graph{ renderTarget.getGraph() }
		, m_currentColourViews{ currentColourViews }
		, m_previousColourViews{ previousColourViews }
		, m_velocityView{ velocityView }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "SmaaReproject", doGetReprojectVP( renderTarget.getSize(), config ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "SmaaReproject", doGetReprojectFP( renderTarget.getSize(), config, velocityView != nullptr ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_result{ m_device
			, m_graph.getHandler()
			, "SMRpRes"
			, 0u
			, castor3d::makeExtent3D( renderTarget.getSize() )
			, 1u
			, 1u
			, renderTarget.getPixelFormat()
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) }
		, m_pass{ m_graph.createPass( "SmaaReproject"
			, [this, &config, &device, &currentColourViews]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto commandBuffer = device.graphicsCommandPool->createCommandBuffer();
				commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

				for ( auto & view : m_currentColourViews )
				{
					ashes::ImagePtr image = std::make_unique< ashes::Image >( *device
						, graph.createImage( view.data->image )
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
				device.graphicsQueue->submit( *commandBuffer, nullptr );
				device.graphicsQueue->waitIdle();
				commandBuffer.reset();

				auto size = m_result.imageId.data->info.extent;
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( { size.width, size.height } )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.passIndex( &config.subsampleIndex )
					.build( pass, context, graph, config.maxSubsampleIndices );
			} ) }
	{	
		crg::SamplerDesc pointSampler{ VK_FILTER_NEAREST
			, VK_FILTER_NEAREST
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addDependency( previousPass );
		m_pass.addOutputColourView( m_result.wholeViewId );
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
		visitor.visit( "SMAA Reproject"
			, m_result
			, m_graph.getFinalLayout( m_result.wholeViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}

	//*********************************************************************************************
}

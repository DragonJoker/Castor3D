#include "SmaaPostEffect/Reproject.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace smaa
{
	namespace reproj
	{
		namespace c3d = castor3d::shader;

		enum Idx : uint32_t
		{
			CurColTexIdx = SmaaUboIdx + 1,
			PrvColTexIdx,
			VelocityTexIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device
			, bool reprojection )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Smaa( writer, SmaaUboIdx, 0u );
			auto c3d_currentColourTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_currentColourTex", CurColTexIdx, 0u );
			auto c3d_previousColourTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_previousColourTex", PrvColTexIdx, 0u );
			auto c3d_velocityTex = writer.declCombinedImg< FImg2DRg32 >( "c3d_velocityTex", VelocityTexIdx, 0u, reprojection );

			auto SMAAResolvePS = writer.implementFunction< sdw::Vec4 >( "SMAAResolvePS"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::CombinedImage2DRgba32 const & currentColorTex
					, sdw::CombinedImage2DRgba32 const & previousColorTex )
				{
					IF( writer, c3d_smaaData.enableReprojection != 0 )
					{
						// Velocity is assumed to be calculated for motion blur, so we need to
						// inverse it for reprojection:
						auto velocity = writer.declLocale( "velocity"
							, -c3d_velocityTex.sample( texcoord ) );

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
							, 0.5_f * clamp( 1.0_f - sqrt( delta ) * c3d_smaaData.reprojectionWeightScale, 0.0_f, 1.0_f ) );

						// Blend the pixels according to the calculated weight:
						writer.returnStmt( mix( current, previous, vec4( weight ) ) );
					}
					ELSE
					{
						// Just blend the pixels:
						auto current = writer.declLocale( "current"
							, currentColorTex.sample( texcoord ) );
						auto previous = writer.declLocale( "previous"
							, previousColorTex.sample( texcoord ) );
						writer.returnStmt( mix( current, previous, vec4( 0.5_f ) ) );
					}
					FI;
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InCombinedImage2DRgba32{ writer, "currentColorTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "previousColorTex" } );

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					out.colour() = SMAAResolvePS( in.uv(), c3d_currentColourTex, c3d_previousColourTex );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}
	
	//*********************************************************************************************

	Reproject::Reproject( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, crg::ImageViewIdArray const & currentColourViews
		, crg::ImageViewIdArray const & previousColourViews
		, crg::ImageViewId const * velocityView
		, SmaaConfig const & config
		, bool const * enabled )
		: m_device{ device }
		, m_graph{ graph }
		, m_currentColourViews{ currentColourViews }
		, m_previousColourViews{ previousColourViews }
		, m_velocityView{ velocityView }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_shader{ "SmaaReproject", reproj::getProgram( device, velocityView != nullptr ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_result{ m_device
			, renderTarget.getResources()
			, "SMRpRes"
			, 0u
			, m_extent
			, 1u
			, 1u
			, renderTarget.getPixelFormat()
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) }
		, m_pass{ m_graph.createPass( "Reproject"
			, [this, &device, &config, enabled]( crg::FramePass const & framePass
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
					.build( framePass, context, graph, { config.maxSubsampleIndices } );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} ) }
	{
		auto & context = m_device.makeContext();
		auto data = m_device.graphicsData();
		auto commandBuffer = data->commandPool->createCommandBuffer( "SmaaReprojectImagesClear" );
		commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

		for ( auto & view : m_currentColourViews )
		{
			ashes::ImagePtr image = std::make_unique< ashes::Image >( *device
				, renderTarget.getResources().createImage( context, view.data->image )
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
		ubo.createPassBinding( m_pass
			, SmaaUboIdx );
		m_pass.addSampledView( m_currentColourViews
			, reproj::CurColTexIdx
			, pointSampler );
		m_pass.addSampledView( m_previousColourViews
			, reproj::PrvColTexIdx
			, pointSampler );
		m_result.create();

		if ( m_velocityView )
		{
			m_pass.addSampledView( *m_velocityView
				, reproj::VelocityTexIdx
				, pointSampler );
		}
	}

	Reproject::~Reproject()
	{
		m_result.destroy();
	}

	void Reproject::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
		visitor.visit( "SMAA Reprojection Result"
			, m_result
			, m_graph.getFinalLayoutState( m_result.sampledViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}

	//*********************************************************************************************
}

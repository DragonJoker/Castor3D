#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Queue.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Miscellaneous/RendererFeatures.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <random>

CU_ImplementSmartPtr( castor3d, LineariseDepthPass )

namespace castor3d
{
	namespace passlindpth
	{
		static uint32_t constexpr DepthImgIdx = 0u;
		static uint32_t constexpr ClipInfoUboIdx = 1u;
		static uint32_t constexpr PrevLvlUboIdx = 1u;

		static void getVertexProgram( sdw::TraditionalGraphicsWriter & writer )
		{
			// Shader inputs
			auto position = writer.declInput< sdw::Vec2 >( "position", sdw::EntryPoint::eVertex, 0u );

			writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::VertexIn const & in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
		}

		static ShaderPtr getLineariseProgram( Engine & engine )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };
			getVertexProgram( writer );

			shader::Utils utils{ writer };

			// Shader inputs
			auto clipInfo = writer.declUniformBuffer( "ClipInfo", ClipInfoUboIdx, 0u, ast::type::MemoryLayout::eStd140 );
			auto c3d_clipInfo = clipInfo.declMember< sdw::Vec3 >( "c3d_clipInfo" );
			clipInfo.end();
			auto c3d_mapDepthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepthObj", DepthImgIdx, 0u );

			// Shader outputs
			auto outColour = writer.declOutput< sdw::Float >( "outColour", sdw::EntryPoint::eFragment, 0u );

			writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::FragmentIn const & in
				, sdw::FragmentOut const & )
				{
					auto ssPosition = writer.declLocale( "ssPosition"
						, ivec2( in.fragCoord.xy() ) );

					outColour = utils.reconstructCSZ( c3d_mapDepthObj.fetch( ssPosition, 0_i ).r()
						, c3d_clipInfo );
				} );
			return writer.getBuilder().releaseShader();
		}

		static ShaderPtr getMinifyProgram( Engine & engine )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };
			getVertexProgram( writer );

			// Shader inputs
			auto previousLevel = writer.declUniformBuffer( "PreviousLevel", PrevLvlUboIdx, 0u, ast::type::MemoryLayout::eStd140 );
			auto c3d_textureSize = previousLevel.declMember< sdw::IVec2 >( "c3d_textureSize" );
			previousLevel.end();
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", DepthImgIdx, 0u );

			// Shader outputs
			auto outColour = writer.declOutput< sdw::Float >( "outColour", sdw::EntryPoint::eFragment, 0u );

			writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::FragmentIn const & in
				, sdw::FragmentOut const & )
				{
					auto ssPosition = writer.declLocale( "ssPosition"
						, ivec2( in.fragCoord.xy() ) );

					// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling.
					outColour = c3d_mapDepth.fetch( clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
							, ivec2( 0_i, 0_i )
							, c3d_textureSize - ivec2( 1_i, 1_i ) )
						, 0_i ).r();
				} );
			return writer.getBuilder().releaseShader();
		}

		static Texture doCreateTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, VkExtent2D const & size
			, castor::String const & prefix )
		{
			return Texture{ device
				, resources
				, prefix + cuT( "LinearisedDepth" )
				, 0u
				, { size.width, size.height, 1u }
				, 1u
				, MaxLinearizedDepthMipLevel + 1u
				, VK_FORMAT_R32_SFLOAT
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) };
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( crg::ResourcesCache & resources
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, ProgressBar * progress
		, castor::String const & prefix
		, SsaoConfig const & ssaoConfig
		, VkExtent2D const & size
		, Texture const & depthObj )
		: m_device{ device }
		, m_graph{ graph }
		, m_engine{ *m_device.renderSystem.getEngine() }
		, m_ssaoConfig{ ssaoConfig }
		, m_prefix{ graph.getName() + prefix }
		, m_size{ size }
		, m_result{ passlindpth::doCreateTexture( m_device, resources, m_size, m_prefix ) }
		, m_clipInfo{ m_device.uboPool->getBuffer< castor::Point3f >( 0u ) }
		, m_extractShader{ m_prefix + "ExtractDepth", passlindpth::getLineariseProgram( *device.renderSystem.getEngine() ) }
		, m_extractStages{ makeProgramStates( m_device, m_extractShader ) }
		, m_extractPass{ doInitialiseExtractPass( progress, previousPasses, depthObj ) }
		, m_minifyShader{ m_prefix + "MinifyDepth", passlindpth::getMinifyProgram( *device.renderSystem.getEngine() ) }
		, m_minifyStages{ makeProgramStates( m_device, m_minifyShader ) }
	{
		doInitialiseMinifyPass( progress );
	}

	LineariseDepthPass::~LineariseDepthPass()noexcept
	{
		for ( auto const & level : m_previousLevel )
		{
			m_device.uboPool->putBuffer( level );
		}

		if ( m_clipInfo )
		{
			m_device.uboPool->putBuffer( m_clipInfo );
		}

		m_result.destroy();
	}

	void LineariseDepthPass::update( CpuUpdater & updater )
	{
		auto const & viewport = updater.camera->getViewport();
		auto z_f = viewport.getFar();
		auto z_n = viewport.getNear();
		auto clipInfo = ( std::isinf( z_f )
			? castor::Point3f{ z_n, -1.0f, 1.0f }
			: castor::Point3f{ z_n * z_f, z_f - z_n, z_n } );
		// result = clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] );
		// depth = 0 => result = z_f
		// depth = 1 => result = z_n
		m_clipInfoValue = clipInfo;

		if ( m_clipInfoValue.isDirty() )
		{
			m_clipInfo.getData() = m_clipInfoValue;
			m_clipInfoValue.reset();
		}
	}

	void LineariseDepthPass::accept( ConfigurationVisitorBase & visitor )
	{
		uint32_t index = 0u;

		for ( auto & layer : getResult() )
		{
			visitor.visit( "Linearised Depth " + castor::string::toString( index )
				, layer
				, m_graph.getFinalLayoutState( layer ).layout
				, TextureFactors{}.invert( true ) );
			++index;
		}

		visitor.visit( m_extractShader );
		visitor.visit( m_minifyShader );
	}

	crg::FramePass const & LineariseDepthPass::doInitialiseExtractPass( ProgressBar * progress
		, crg::FramePassArray const & previousPasses
		, Texture const & depthObj )
	{
		stepProgressBarLocal( progress, "Creating linearised depth extraction pass" );
		auto & pass = m_graph.createPass( "ExtractDepth"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBarLocal( progress, "Initialising linearised depth extraction pass" );
				auto result = crg::RenderQuadBuilder{}
					.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_extractStages ) )
					.renderSize( m_size )
					.enabled( &m_ssaoConfig.enabled )
					.build( framePass, context, graph );
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		pass.addSampledView( depthObj.targetViewId, passlindpth::DepthImgIdx );
		m_clipInfo.createPassBinding( pass, "ClipInfoCfg", passlindpth::ClipInfoUboIdx );
		pass.addOutputColourView( m_result.targetViewId );
		m_result.create();
		m_lastPass = &pass;
		return pass;
	}

	void LineariseDepthPass::doInitialiseMinifyPass( ProgressBar * progress )
	{
		auto size = m_size;

		for ( auto index = 0u; index < MaxLinearizedDepthMipLevel; ++index )
		{
			stepProgressBarLocal( progress, "Creating depth minify pass " + std::to_string( index ) );
			m_previousLevel.push_back( m_device.uboPool->getBuffer< castor::Point2i >( 0u ) );
			auto & previousLevel = m_previousLevel.back();
			auto & data = previousLevel.getData();
			data = castor::Point2i{ size.width, size.height };
			size.width >>= 1;
			size.height >>= 1;
			auto source = m_graph.createView( crg::ImageViewData{ m_result.imageId.data->name + std::to_string( index )
				, m_result.imageId
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, m_result.getFormat()
				, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, index, 1u, 0u, 1u } } );
			auto destination = m_graph.createView( crg::ImageViewData{ m_result.imageId.data->name + std::to_string( index + 1u )
				, m_result.imageId
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, m_result.getFormat()
				, VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, index + 1u, 1u, 0u, 1u } } );
			auto & pass = m_graph.createPass( "MinimiseDepth" + std::to_string( index )
				, [this, progress, size]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					stepProgressBarLocal( progress, "Initialising depth minify pass" );
					auto result = crg::RenderQuadBuilder{}
						.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_minifyStages ) )
						.renderSize( size )
						.enabled( &m_ssaoConfig.enabled )
						.build( framePass, context, graph );
					m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			pass.addDependency( *m_lastPass );
			pass.addSampledView( source, passlindpth::DepthImgIdx );
			previousLevel.createPassBinding( pass, "PreviousLvlCfg", passlindpth::PrevLvlUboIdx );
			pass.addOutputColourView( destination );
			m_lastPass = &pass;

			if ( m_mipViews.empty() )
			{
				m_mipViews.push_back( source );
			}

			m_mipViews.push_back( destination );
		}
	}

	uint32_t LineariseDepthPass::countInitialisationSteps()noexcept
	{
		return MaxLinearizedDepthMipLevel + 1u;
	}
}

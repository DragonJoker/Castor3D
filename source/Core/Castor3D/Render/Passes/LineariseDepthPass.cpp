#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <random>

CU_ImplementSmartPtr( c3d, LineariseDepthPass )

namespace c3d
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

			writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::VertexIn const &
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
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DR32 >( "c3d_mapDepth", DepthImgIdx, 0u );

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
						, 0_i );
				} );
			return writer.getBuilder().releaseShader();
		}

		static Texture doCreateTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, Extent2D const & size
			, String const & prefix )
		{
			return Texture{ device
				, resources
				, prefix + cuT( "LinearisedDepth" )
				, { ImageCreateFlags::eNone
					, { size.width, size.height, 1u }, 1u, MaxLinearizedDepthMipLevel + 1u
					, PixelFormat::eR32_SFLOAT
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eSampled
						| ImageUsageFlags::eTransferDst
						| ImageUsageFlags::eTransferSrc ) }
				, {} };
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( crg::ResourcesCache & resources
		, crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, String const & prefix
		, SsaoConfig const & ssaoConfig
		, Extent2D const & size
		, Texture const & depthObj )
		: m_device{ device }
		, m_graph{ graph }
		, m_engine{ c3d::getEngine( m_device ) }
		, m_ssaoConfig{ ssaoConfig }
		, m_prefix{ makeString( graph.getName() ) + prefix }
		, m_size{ size }
		, m_result{ passlindpth::doCreateTexture( m_device, resources, m_size, m_prefix ) }
		, m_clipInfo{ m_device.uboPool->getBuffer< Point3f >( MemoryPropertyFlags::eNone ) }
		, m_extractShader{ m_prefix + cuT( "ExtractDepth" ), passlindpth::getLineariseProgram( c3d::getEngine( device ) ) }
		, m_extractStages{ makeProgramStates( m_device, m_extractShader ) }
		, m_minifyShader{ m_prefix + cuT( "MinifyDepth" ), passlindpth::getMinifyProgram( c3d::getEngine( device ) ) }
		, m_minifyStages{ makeProgramStates( m_device, m_minifyShader ) }
	{
		doInitialiseExtractPass( progress, depthObj );
		doInitialiseMinifyPass( progress );
		m_result.create();
	}

	LineariseDepthPass::~LineariseDepthPass()noexcept
	{
		for ( auto & level : m_previousLevel )
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
			? Point3f{ z_n, -1.0f, 1.0f }
			: Point3f{ z_n * z_f, z_f - z_n, z_n } );
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

		for ( auto & layerViews : getResult() )
		{
			visitor.visit( cuT( "Linearised Depth " ) + string::toString( index )
				, layerViews.sampledViewId
				, m_graph.getFinalLayoutState( layerViews.sampledViewId ).layout
				, TextureFactors{}.invert( true ) );
			++index;
		}

		visitor.visit( m_extractShader );
		visitor.visit( m_minifyShader );
	}

	void LineariseDepthPass::doInitialiseExtractPass( ProgressBar * progress
		, Texture const & depthObj )
	{
		stepProgressBarLocal( progress, cuT( "Creating linearised depth extraction pass" ) );
		auto & pass = m_graph.createPass( "ExtractDepth"
			, [this, progress]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising linearised depth extraction pass" ) );
				auto result = crg::RenderQuadBuilder{}
					.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_extractStages ) )
					.renderSize( m_size )
					.enabled( &m_ssaoConfig.enabled )
					.build( framePass, context, graph );
				c3d::getEngine( m_device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addInputSampled( *depthObj.getSampledLastAttach(), passlindpth::DepthImgIdx );
		m_clipInfo.createPassBinding( pass, passlindpth::ClipInfoUboIdx );
		m_result.setLastAttach( 0u, 0u, pass.addOutputColourTarget( m_result.getTargetViewId( 0u, 0u ) ) );
	}

	void LineariseDepthPass::doInitialiseMinifyPass( ProgressBar * progress )
	{
		auto size = m_size;
		crg::AttachmentArray attachs;
		attachs.push_back( m_result.getLastAttach( 0u, 0u ) );

		for ( auto index = 0u; index < MaxLinearizedDepthMipLevel; ++index )
		{
			stepProgressBarLocal( progress, cuT( "Creating depth minify pass " ) + string::toString( index ) );
			m_previousLevel.push_back( m_device.uboPool->getBuffer< Point2i >( MemoryPropertyFlags::eNone ) );
			auto & previousLevel = m_previousLevel.back();
			auto & data = previousLevel.getData();
			data = Point2i{ size.width, size.height };
			size.width >>= 1;
			size.height >>= 1;
			auto & pass = m_graph.createPass( "MinimiseDepth" + string::toMbString( index )
				, [this, progress, size]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					stepProgressBarLocal( progress, cuT( "Initialising depth minify pass" ) );
					auto runPass = crg::RenderQuadBuilder{}
						.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_minifyStages ) )
						.renderSize( size )
						.enabled( &m_ssaoConfig.enabled )
						.build( framePass, context, graph );
					c3d::getEngine( m_device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			pass.addInputSampled( *m_result.getSampledLastAttach( 0u, index ), passlindpth::DepthImgIdx );
			previousLevel.createPassBinding( pass, passlindpth::PrevLvlUboIdx );
			attachs.push_back( m_result.setLastAttach( 0u, index + 1u
				, pass.addOutputColourTarget( m_result.getTargetViewId( 0u, index + 1u ) ) ) );
		}

		m_result.setLastAttach( m_graph.mergeAttachments( attachs ) );
	}

	uint32_t LineariseDepthPass::countInitialisationSteps()noexcept
	{
		return MaxLinearizedDepthMipLevel + 1u;
	}
}

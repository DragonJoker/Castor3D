#include "BloomPostEffect/HiPass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/ImageBlit.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace Bloom
{
	namespace hi
	{
		namespace c3ds = c3d::shader;

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( [&]( sdw::VertexInT< c3ds::PosUv2FT > in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
				{
					out.colour() = vec4( c3d_mapColor.sample( in.uv(), 0.0_f ).xyz(), 1.0_f );
					auto maxComponent = writer.declLocale( "maxComponent"
						, max( out.colour().r(), out.colour().g() ) );
					maxComponent = max( maxComponent, out.colour().b() );

					sdwIF( writer, maxComponent <= 1.0_f )
					{
						out.colour().xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
					}
					sdwFI
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	HiPass::HiPass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::Texture const & sceneView
		, c3d::Extent2D size
		, uint32_t blurPassesCount
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_graph{ graph }
		, m_shader{ cuT( "BloomHiPass" ), hi::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
#if Bloom_DebugHiPass
		, m_result{ device
			, device.renderSystem.getEngine()->getGraphResourceCache()
			, "BLHi"
			, { c3d::ImageCreateFlags::eNone
				, c3d::Extent3D{ size.width >> 1, size.height >> 1, 1u }, 1u, 1u
				, sceneView.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferSrc
					| c3d::ImageUsageFlags::eTransferDst ) }
			, {} }
#else
		, m_result{ device
			, device.renderSystem.getEngine()->getGraphResourceCache()
			, "BLHi"
			, { c3d::ImageCreateFlags::eNone
				, c3d::Extent3D{ size.width >> 1, size.height >> 1, 1u }, 1u, blurPassesCount
				, sceneView.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferSrc
					| c3d::ImageUsageFlags::eTransferDst ) }
			, {} }
#endif
	{
		crg::AttachmentArray attachs;
		auto extent = m_result.getExtent();
		auto & hiPass = graph.createPass( "HiPass"
			, [this, &device, extent, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto res = c3d::makeRawUnique< crg::RenderQuad >( framePass
					, context
					, runGraph
					, crg::ru::Config{ 2u }
					, crg::rq::Config{}
						.baseConfig( crg::pp::Config{ c3d::Vector< crg::VkPipelineShaderStageCreateInfoArray >{ ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) }, {}, {} } )
						.texcoordConfig( crg::Texcoord{} )
						.enabled( enabled )
						.passIndex( passIndex )
						.renderSize( c3d::makeExtent2D( extent ) ) );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		hiPass.addInputSampled( *sceneView.getSampledLastAttach(), 0u );
		attachs.push_back( m_result.setLastAttach( 0u, 0u, hiPass.addOutputColourTarget( m_result.getTargetViewId( 0u, 0u ) ) ) );

#if !Bloom_DebugHiPass
		for ( uint32_t index = 1u; index < blurPassesCount; ++index )
		{
			auto & blitPass = graph.createPass( "HiPassBlit" + c3d::string::toString( index )
				, [&device, extent, enabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = c3d::makeRawUnique< crg::ImageBlit >( framePass
						, context
						, graph
						, c3d::Rect3D{ {}, extent }
						, c3d::Rect3D{ {}, { extent.width >> 1u, extent.height >> 1u, 1u } }
						, c3d::FilterMode::eLinear
						, crg::ru::Config{}
						, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
						, crg::RunnablePass::IsEnabledCallback( [enabled](){ return *enabled; } ) );
					device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			blitPass.addInputTransfer( *m_result.getLastAttach( 0u, index - 1u ) );
			attachs.push_back( m_result.setLastAttach( 0u, index, blitPass.addOutputTransferImage( m_result.getTargetViewId( 0u, index ) ) ) );
			extent = { extent.width >> 1u, extent.height >> 1u, 1u };
		}
#endif

		m_result.setLastAttach( graph.mergeAttachments( attachs ) );
	}

	void HiPass::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );

		for ( auto & layerViews : m_result )
		{
			for ( auto & mipViews : layerViews.mipViews )
				visitor.visit( cuT( "PostFX: HDRB - Hi " ) + c3d::string::toString( getSubresourceRange( mipViews.sampledViewId ).baseMipLevel )
					, mipViews.sampledViewId
					, m_graph.getFinalLayoutState( mipViews.sampledViewId ).layout
					, c3d::TextureFactors{}.invert( true ) );
		}
	}
}

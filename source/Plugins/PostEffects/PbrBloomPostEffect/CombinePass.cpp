#include "PbrBloomPostEffect/CombinePass.hpp"

#include "PbrBloomPostEffect/PbrBloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace PbrBloom
{
	namespace combine
	{
		namespace c3ds = c3d::shader;

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapPasses = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapScene, 1u, 0u );
			auto constants = writer.declUniformBuffer( "Constants", 2u, 0u );
			auto filterRadius = constants.declMember< sdw::Float >( "filterRadius" );
			auto bloomStrength = constants.declMember< sdw::Float >( "bloomStrength" );
			constants.end();

			writer.implementEntryPointT< c3ds::Position2FT, c3ds::Uv2FT >( [&]( sdw::VertexInT< c3ds::Position2FT > in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
				{
					out.colour() = vec4( mix( c3d_mapScene.sample( in.uv() ).rgb()
							, c3d_mapPasses.sample( in.uv() ).rgb()
							, vec3( bloomStrength ) )
						, 1.0f );
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	c3d::MbString const CombinePass::CombineMapPasses = "c3d_mapPasses";
	c3d::MbString const CombinePass::CombineMapScene = "c3d_mapScene";

	CombinePass::CombinePass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, c3d::RenderDevice const & device
		, crg::ImageViewIdArray const & lhs
		, crg::ImageId const & rhs
		, crg::ImageViewIdArray const & result
		, c3d::UniformBufferOffsetT< c3d::Point2f > const & ubo
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ cuT( "PbrBloomCombine" ), combine::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_pass{ graph.createPass( "Combine"
			, [this, &device, lhs, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto extent = getExtent( lhs.front() );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( c3d::makeExtent2D( extent ) )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( enabled )
					.passIndex( passIndex )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ uint32_t( lhs.size() ) } );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
							, result->getTimer() );
				return result;
			} ) }
	{
		auto intermediateView = graph.createView( crg::ImageViewData{ rhs.data->name + "0"
			, rhs
			, c3d::ImageViewCreateFlags::eNone
			, c3d::ImageViewType::e2D
			, getFormat( rhs )
			, { c3d::ImageAspectFlags::eColor, 0u, 1u, 0u, 1u } } );
		m_pass.addDependency( previousPass );
		m_pass.addSampledView( intermediateView
			, 0u
			, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear } );
		m_pass.addSampledView( lhs
			, 1u );
		ubo.createPassBinding( m_pass
			, "PbrBloomUbo"
			, 2u );
		m_pass.addOutputColourView( result );
	}

	void CombinePass::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}

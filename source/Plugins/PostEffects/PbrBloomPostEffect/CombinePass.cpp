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

		enum class Idx
		{
			SceneMapIdx,
			BlurredMapIdx,
			ConstantsIdx,
		};

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapScene = writer.declCombinedImg< Img2DRgba >( CombinePass::CombineMapScene, uint32_t( Idx::SceneMapIdx ), 0u );
			auto c3d_mapPasses = writer.declCombinedImg< Img2DRgba >( CombinePass::CombineMapPasses, uint32_t( Idx::BlurredMapIdx ), 0u );
			auto constants = writer.declUniformBuffer( "Constants", uint32_t( Idx::ConstantsIdx ), 0u );
			auto filterRadius = constants.declMember< sdw::Float >( "filterRadius" );
			auto bloomStrength = constants.declMember< sdw::Float >( "bloomStrength" );
			constants.end();

			writer.implementEntryPointT< c3ds::Position2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&c3d_mapScene, &c3d_mapPasses, &bloomStrength]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
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
		, c3d::RenderDevice const & device
		, c3d::Texture const & blurredView
		, c3d::Texture const & sceneView
		, c3d::Texture & resultView
		, c3d::Extent2D const & size
		, c3d::UniformBufferOffsetT< c3d::Point2f > const & ubo
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ cuT( "PbrBloomCombine" ), combine::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto & pass = graph.createPass( "Combine"
			, [this, &device, size, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( size )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( enabled )
					.passIndex( passIndex )
					.build( framePass, context, runGraph
						, crg::ru::Config{ 2u } );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
							, result->getTimer() );
				return result;
			} );
		pass.addInputSampled( *sceneView.getSampledLastAttach(), uint32_t( combine::Idx::SceneMapIdx )
			, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear } );
		pass.addInputSampled( *blurredView.getSampledLastAttach(), uint32_t( combine::Idx::BlurredMapIdx ) );
		ubo.createPassBinding( pass, uint32_t( combine::Idx::ConstantsIdx ) );
		resultView.setLastAttach( pass.addOutputColourTarget( { resultView.getTargetViewId(), sceneView.getTargetViewId() } ) );
	}

	void CombinePass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}
}

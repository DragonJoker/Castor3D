#include "LightStreaksPostEffect/CombinePass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

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

namespace light_streaks
{
	namespace combine
	{
		namespace c3ds = c3d::shader;

		enum Idx
		{
			SceneMapIdx,
			KawaseMapIdx,
		};

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapScene, SceneMapIdx, 0u );
			auto c3d_mapKawase = writer.declCombinedImg< FImg2DArrayRgba32 >( CombinePass::CombineMapKawase, KawaseMapIdx, 0u );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( [&]( sdw::VertexInT< c3ds::PosUv2FT > in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
				{
					out.colour() = c3d_mapScene.sample( in.uv() );
					out.colour() += c3d_mapKawase.sample( vec3( in.uv(), 0.0f ) );
					out.colour() += c3d_mapKawase.sample( vec3( in.uv(), 1.0f ) );
					out.colour() += c3d_mapKawase.sample( vec3( in.uv(), 2.0f ) );
					out.colour() += c3d_mapKawase.sample( vec3( in.uv(), 3.0f ) );
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	c3d::MbString const CombinePass::CombineMapScene = "c3d_mapScene";
	c3d::MbString const CombinePass::CombineMapKawase = "c3d_mapKawase";

	CombinePass::CombinePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::Texture const & kawaseViews
		, c3d::Texture const & sceneView
		, c3d::Texture & resultView
		, c3d::Extent2D const & size
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ cuT( "LightStreaksCombine" ), combine::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto & pass = graph.createPass( "Combine"
			, [this, &device, size, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( size )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( enabled )
					.passIndex( passIndex )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ 2u } );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		crg::SamplerDesc linearSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest };
		pass.addInputSampled( *sceneView.getSampledLastAttach(), combine::SceneMapIdx, linearSampler );
		pass.addInputSampled( *kawaseViews.mergeLayerAttachments( graph ), combine::KawaseMapIdx, linearSampler );
		resultView.setLastAttach( pass.addOutputColourTarget( { resultView.getTargetViewId(), sceneView.getTargetViewId() } ) );
	}

	void CombinePass::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}

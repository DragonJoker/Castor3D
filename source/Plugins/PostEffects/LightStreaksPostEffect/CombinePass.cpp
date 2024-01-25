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
		namespace c3d = castor3d::shader;

		enum Idx
		{
			SceneMapIdx,
			KawaseMapIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapScene, SceneMapIdx, 0u );
			auto c3d_mapKawase = writer.declCombinedImg< FImg2DArrayRgba32 >( CombinePass::CombineMapKawase, KawaseMapIdx, 0u );

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
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

	castor::MbString const CombinePass::CombineMapScene = "c3d_mapScene";
	castor::MbString const CombinePass::CombineMapKawase = "c3d_mapKawase";

	CombinePass::CombinePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & sceneView
		, crg::ImageViewIdArray const & kawaseViews
		, crg::ImageViewIdArray const & resultView
		, VkExtent2D const & size
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ cuT( "LightStreaksCombine" ), combine::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_pass{ graph.createPass( "Combine"
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
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} ) }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addDependencies( previousPasses );
		m_pass.addSampledView( sceneView
			, combine::SceneMapIdx
			, linearSampler );
		m_pass.addSampledView( m_pass.mergeViews( kawaseViews )
			, combine::KawaseMapIdx
			, linearSampler );
		m_pass.addOutputColourView( resultView );
	}

	void CombinePass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}

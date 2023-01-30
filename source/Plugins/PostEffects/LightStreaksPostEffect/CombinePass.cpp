#include "LightStreaksPostEffect/CombinePass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace light_streaks
{
	namespace combine
	{
		enum Idx
		{
			SceneMapIdx,
			KawaseMapIdx,
		};

		static std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;
			// Shader inputs
			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapScene, SceneMapIdx, 0u );
			auto c3d_mapKawase = writer.declCombinedImg< FImg2DArrayRgba32 >( CombinePass::CombineMapKawase, KawaseMapIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					outColour = c3d_mapScene.sample( vtx_texture );
					outColour += c3d_mapKawase.sample( vec3( vtx_texture, 0.0f ) );
					outColour += c3d_mapKawase.sample( vec3( vtx_texture, 1.0f ) );
					outColour += c3d_mapKawase.sample( vec3( vtx_texture, 2.0f ) );
					outColour += c3d_mapKawase.sample( vec3( vtx_texture, 3.0f ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	castor::String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );
	castor::String const CombinePass::CombineMapKawase = cuT( "c3d_mapKawase" );

	CombinePass::CombinePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & sceneView
		, crg::ImageViewIdArray const & kawaseViews
		, crg::ImageViewIdArray const & resultView
		, VkExtent2D const & size
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksCombine", combine::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "LightStreaksCombine", combine::getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
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
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
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

	void CombinePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

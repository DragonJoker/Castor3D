#include "LightStreaksPostEffect/CombinePass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace light_streaks
{
	namespace
	{
		enum Idx
		{
			SceneMapIdx,
			KawaseMapIdx,
		};

		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;
			// Shader inputs
			auto c3d_mapScene = writer.declSampledImage< FImg2DRgba32 >( CombinePass::CombineMapScene, SceneMapIdx, 0u );
			auto c3d_mapKawase = writer.declSampledImage< FImg2DArrayRgba32 >( CombinePass::CombineMapKawase, KawaseMapIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );
			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();
			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = c3d_mapScene.sample( vtx_texture );
					pxl_fragColor += c3d_mapKawase.sample( vec3( vtx_texture, 0.0f ) );
					pxl_fragColor += c3d_mapKawase.sample( vec3( vtx_texture, 1.0f ) );
					pxl_fragColor += c3d_mapKawase.sample( vec3( vtx_texture, 2.0f ) );
					pxl_fragColor += c3d_mapKawase.sample( vec3( vtx_texture, 3.0f ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );
	String const CombinePass::CombineMapKawase = cuT( "c3d_mapKawase" );

	CombinePass::CombinePass( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & sceneView
		, crg::ImageViewIdArray const & kawaseViews
		, VkExtent2D const & size )
		: m_device{ device }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksCombine", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "LightStreaksCombine", getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_resultImg{ graph.createImage( crg::ImageData{ "LSComb"
			, 0u
			, VK_IMAGE_TYPE_2D
			, sceneView.data->info.format
			, VkExtent3D{ size.width, size.height, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
		, m_resultView{ graph.createView( crg::ImageViewData{ "LSComb"
			, m_resultImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_resultImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_pass{ graph.createPass( "LightStreaksCombinePass"
			, [this, size]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( size )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( pass, context, graph );
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
			, SceneMapIdx
			, {}
			, linearSampler );
		m_pass.addSampledView( m_pass.mergeViews( kawaseViews )
			, KawaseMapIdx
			, {}
			, linearSampler );
		m_pass.addOutputColourView( m_resultView );
	}

	void CombinePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

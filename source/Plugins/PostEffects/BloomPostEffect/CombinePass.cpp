#include "BloomPostEffect/CombinePass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace Bloom
{
	namespace
	{
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

		std::unique_ptr< ast::Shader > getPixelProgram( uint32_t blurPassesCount )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapPasses = writer.declSampledImage< FImg2DRgba32 >( CombinePass::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declSampledImage< FImg2DRgba32 >( CombinePass::CombineMapScene, 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = c3d_mapScene.sample( vtx_texture );

					for ( uint32_t i = 0; i < blurPassesCount; ++i )
					{
						pxl_fragColor += c3d_mapPasses.sample( vtx_texture, Float( float( i ) ) );
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	String const CombinePass::CombineMapPasses = cuT( "c3d_mapPasses" );
	String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );

	CombinePass::CombinePass( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & sceneView
		, crg::ImageViewIdArray const & blurViews
		, VkExtent2D const & size
		, uint32_t blurPassesCount )
		: m_device{ device }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "BloomCombine", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "BloomCombine", getPixelProgram( blurPassesCount ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_blurPassesCount{ blurPassesCount }
		, m_resultImg{ graph.createImage( crg::ImageData{ "BLComb"
			, 0u
			, VK_IMAGE_TYPE_2D
			, sceneView.data->info.format
			, VkExtent3D{ size.width, size.height, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
		, m_resultView{ graph.createView( crg::ImageViewData{ "BLComb"
			, m_resultImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_resultImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_pass{ graph.createPass( "BloomCombinePass"
			, [this, size]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( size )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( pass, context, graph );
			} ) }
	{
		m_pass.addDependencies( previousPasses );
		m_pass.addSampledView( m_pass.mergeViews( blurViews )
			, 0u
			, {}
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_NEAREST
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, 0.0f
				, 0.0f
				, float( blurPassesCount ) } );
		m_pass.addSampledView( sceneView, 1u );
		m_pass.addOutputColourView( m_resultView );
	}

	void CombinePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

#include "PbrBloomPostEffect/CombinePass.hpp"

#include "PbrBloomPostEffect/PbrBloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace PbrBloom
{
	namespace combine
	{
		static std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapPasses = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapScene, 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			auto constants = writer.declUniformBuffer( "Constants", 2u, 0u );
			auto filterRadius = constants.declMember< sdw::Float >( "filterRadius" );
			auto bloomStrength = constants.declMember< sdw::Float >( "bloomStrength" );
			constants.end();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					pxl_fragColor = vec4( mix( c3d_mapScene.sample( vtx_texture ).rgb()
							, c3d_mapPasses.sample( vtx_texture ).rgb()
							, vec3( bloomStrength ) )
						, 1.0f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	castor::String const CombinePass::CombineMapPasses = cuT( "c3d_mapPasses" );
	castor::String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );

	CombinePass::CombinePass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & sceneView
		, crg::ImageId const & intermediate
		, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
		, bool const * enabled )
		: m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "PbrBloomCombine", combine::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "PbrBloomCombine", combine::getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_resultImg{ graph.createImage( crg::ImageData{ "PBLComb"
			, 0u
			, VK_IMAGE_TYPE_2D
			, sceneView.data->info.format
			, getExtent( sceneView )
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) } ) }
		, m_resultView{ graph.createView( crg::ImageViewData{ "PBLComb"
			, m_resultImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_resultImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_pass{ graph.createPass( "Combine"
			, [this, &device, &sceneView, enabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto extent = getExtent( sceneView );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( extent ) )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( enabled )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{}
							.implicitAction( m_resultView
								, crg::RecordContext::copyImage( sceneView
									, m_resultView
									, { extent.width, extent.height } ) ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, result->getTimer() );
				return result;
			} ) }
	{
		auto intermediateView = graph.createView( crg::ImageViewData{ intermediate.data->name + "0"
			, intermediate
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, intermediate.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		m_pass.addDependency( previousPass );
		m_pass.addSampledView( intermediateView
			, 0u
			, {}
			, crg::SamplerDesc{ VK_FILTER_LINEAR, VK_FILTER_LINEAR } );
		m_pass.addSampledView( sceneView
			, 1u
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		ubo.createPassBinding( m_pass
			, std::string{ "PbrBloomUbo" }
			, 2u );
		m_pass.addOutputColourView( m_resultView );
	}

	void CombinePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

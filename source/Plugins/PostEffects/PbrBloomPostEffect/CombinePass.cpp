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
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					outColour = vec4( mix( c3d_mapScene.sample( vtx_texture ).rgb()
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
		, crg::ImageViewIdArray const & lhs
		, crg::ImageId const & rhs
		, crg::ImageViewIdArray const & result
		, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "PbrBloomCombine", combine::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "PbrBloomCombine", combine::getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_pass{ graph.createPass( "Combine"
			, [this, &device, lhs, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto extent = getExtent( lhs.front() );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( extent ) )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( enabled )
					.passIndex( passIndex )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ uint32_t( lhs.size() ) } );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, result->getTimer() );
				return result;
			} ) }
	{
		auto intermediateView = graph.createView( crg::ImageViewData{ rhs.data->name + "0"
			, rhs
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, rhs.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		m_pass.addDependency( previousPass );
		m_pass.addSampledView( intermediateView
			, 0u
			, crg::SamplerDesc{ VK_FILTER_LINEAR, VK_FILTER_LINEAR } );
		m_pass.addSampledView( lhs
			, 1u );
		ubo.createPassBinding( m_pass
			, std::string{ "PbrBloomUbo" }
			, 2u );
		m_pass.addOutputColourView( result );
	}

	void CombinePass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

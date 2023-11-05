#include "BloomPostEffect/CombinePass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace Bloom
{
	namespace combine
	{
		namespace c3d = castor3d::shader;

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device
			, uint32_t blurPassesCount )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapPasses = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapScene, 1u, 0u );

			writer.implementEntryPointT< c3d::Position2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::Position2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					out.colour() = c3d_mapScene.sample( in.uv() );

					for ( uint32_t i = 0; i < blurPassesCount; ++i )
					{
						out.colour() += c3d_mapPasses.sample( in.uv(), sdw::Float( float( i ) ) );
					}
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	castor::String const CombinePass::CombineMapPasses = cuT( "c3d_mapPasses" );
	castor::String const CombinePass::CombineMapScene = cuT( "c3d_mapScene" );

	CombinePass::CombinePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & sceneView
		, crg::ImageViewIdArray const & blurViews
		, crg::ImageViewIdArray const & result
		, VkExtent2D const & size
		, uint32_t blurPassesCount
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ "BloomCombine", combine::getProgram( device, blurPassesCount ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_pass{ graph.createPass( "Combine"
			, [this, &device, size, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( size )
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
		m_pass.addDependencies( previousPasses );
		m_pass.addImplicitColourView( blurViews.front()
			, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		m_pass.addSampledView( m_pass.mergeViews( blurViews )
			, 0u
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_NEAREST
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, 0.0f
				, 0.0f
				, float( blurPassesCount ) } );
		m_pass.addSampledView( sceneView
			, 1u );
		m_pass.addOutputColourView( result );
	}

	void CombinePass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}

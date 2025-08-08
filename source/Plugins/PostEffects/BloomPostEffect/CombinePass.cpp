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
		namespace c3ds = c3d::shader;

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device
			, uint32_t blurPassesCount )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapPasses = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declCombinedImg< FImg2DRgba32 >( CombinePass::CombineMapScene, 1u, 0u );

			writer.implementEntryPointT< c3ds::Position2FT, c3ds::Uv2FT >( [&]( sdw::VertexInT< c3ds::Position2FT > in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
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

	c3d::MbString const CombinePass::CombineMapPasses = "c3d_mapPasses";
	c3d::MbString const CombinePass::CombineMapScene = "c3d_mapScene";

	CombinePass::CombinePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::Texture const & sceneView
		, c3d::Texture const & blurView
		, c3d::Texture & result
		, c3d::Extent2D const & size
		, uint32_t blurPassesCount
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ cuT( "BloomCombine" ), combine::getProgram( device, blurPassesCount ) }
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
		pass.addInputSampled( *blurView.getSampledLastAttach(), 0u
			, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest
				, c3d::WrapMode::eClampToEdge, c3d::WrapMode::eClampToEdge, c3d::WrapMode::eClampToEdge
				, 0.0f, 0.0f, float( blurPassesCount ) } );
		pass.addInputSampled( *sceneView.getSampledLastAttach(), 1u );
		result.setLastAttach( pass.addOutputColourTarget( { result.getTargetViewId(), sceneView.getTargetViewId() } ) );
	}

	void CombinePass::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}

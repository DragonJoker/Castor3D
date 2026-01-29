#include "PbrBloomPostEffect/DownsamplePass.hpp"

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
	namespace down
	{
		namespace c3ds = c3d::shader;

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto constants = writer.declPushConstantsBuffer<>( "constants" );
			auto srcTexelSize = constants.declMember< sdw::Vec2 >( "srcTexelSize" );
			constants.end();
			auto c3d_mapColor = writer.declCombinedImg< Img2DRgba >( "c3d_mapColor", 0u, 0u );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::PosUv2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour3FT >( [&writer, &srcTexelSize, &c3d_mapColor]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour3FT > const & out )
				{
					auto x = writer.declLocale( "x"
						, srcTexelSize.x() );
					auto y = writer.declLocale( "y"
						, srcTexelSize.y() );
					auto v2 = vec2( 2.0_f );

					// Take 13 samples around current texel:
					// a - b - c
					// - j - k -
					// d - e - f
					// - l - m -
					// g - h - i
					// === ('e' is the current texel) ===
					auto a = writer.declLocale( "a"
						, c3d_mapColor.sample( vec2( in.uv().x() - 2.0f * x, in.uv().y() + 2.0f * y ) ).rgb() );
					auto b = writer.declLocale( "b"
						, c3d_mapColor.sample( vec2( in.uv().x(), in.uv().y() + 2.0f * y ) ).rgb() );
					auto c = writer.declLocale( "c"
						, c3d_mapColor.sample( vec2( in.uv().x() + 2.0f * x, in.uv().y() + 2.0f * y ) ).rgb() );

					auto d = writer.declLocale( "d"
						, c3d_mapColor.sample( vec2( in.uv().x() - 2.0f * x, in.uv().y() ) ).rgb() );
					auto e = writer.declLocale( "e"
						, c3d_mapColor.sample( vec2( in.uv().x(), in.uv().y() ) ).rgb() );
					auto f = writer.declLocale( "f"
						, c3d_mapColor.sample( vec2( in.uv().x() + 2.0f * x, in.uv().y() ) ).rgb() );

					auto g = writer.declLocale( "g"
						, c3d_mapColor.sample( vec2( in.uv().x() - 2.0f * x, in.uv().y() - 2.0f * y ) ).rgb() );
					auto h = writer.declLocale( "h"
						, c3d_mapColor.sample( vec2( in.uv().x(), in.uv().y() - 2.0f * y ) ).rgb() );
					auto i = writer.declLocale( "i"
						, c3d_mapColor.sample( vec2( in.uv().x() + 2.0f * x, in.uv().y() - 2.0f * y ) ).rgb() );

					auto j = writer.declLocale( "j"
						, c3d_mapColor.sample( vec2( in.uv().x() - x, in.uv().y() + y ) ).rgb() );
					auto k = writer.declLocale( "k"
						, c3d_mapColor.sample( vec2( in.uv().x() + x, in.uv().y() + y ) ).rgb() );
					auto l = writer.declLocale( "l"
						, c3d_mapColor.sample( vec2( in.uv().x() - x, in.uv().y() - y ) ).rgb() );
					auto m = writer.declLocale( "m"
						, c3d_mapColor.sample( vec2( in.uv().x() + x, in.uv().y() - y ) ).rgb() );

					// Apply weighted distribution:
					// 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
					// a,b,d,e * 0.125
					// b,c,e,f * 0.125
					// d,e,g,h * 0.125
					// e,f,h,i * 0.125
					// j,k,l,m * 0.5
					// This shows 5 square areas that are being sampled. But some of them overlap,
					// so to have an energy preserving downsample we need to make some adjustments.
					// The weights are the distributed, so that the sum of j,k,l,m (e.g.)
					// contribute 0.5 to the final color output. The code below is written
					// to effectively yield this sum. We get:
					// 0.125*5 + 0.03125*4 + 0.0625*4 = 1
					out.colour() = e * 0.125f;
					out.colour() += ( a + c + g + i ) * 0.03125f;
					out.colour() += ( b + d + f + h ) * 0.0625f;
					out.colour() += ( j + k + l + m ) * 0.125f;
					out.colour() = max( out.colour(), vec3( 0.0001_f ) );
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	DownsamplePass::DownsamplePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::Texture const & sceneView
		, c3d::Vector< c3d::Texture > & resultImg
		, uint32_t passesCount
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ cuT( "PbrBloomDownsample" ), down::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		c3d::Vector< crg::FramePass * > result;
		auto src = sceneView.getLastAttach();

		for ( uint32_t i = 0u; i < passesCount; ++i )
		{
			auto srcExtent = c3d::makeExtent2D( getMipExtent( src->view() ) );
			auto dstExtent = c3d::makeExtent2D( getMipExtent( resultImg[i].getSampledViewId() ) );
			auto count = src->getViewCount();
			auto & pass = graph.createPass( "Downsample" + c3d::string::toMbString( i )
				, [this, &device, passIndex, enabled, count, srcExtent, dstExtent, i]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto builder = crg::RenderQuadBuilder{}
						.enabled( enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
						.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_FRAGMENT_BIT, 0u, sizeof( c3d::Point2f ) } )
						.renderSize( dstExtent )
						.texcoordConfig( {} )
						.recordInto( [this, srcExtent, i]( crg::RecordContext const & ctx
							, VkCommandBuffer cb
							, [[maybe_unused]] uint32_t idx )
							{
								c3d::Point2f invSize{ 1.0f / float( srcExtent.width )
									, 1.0f / float( srcExtent.height ) };
								ctx.getContext().vkCmdPushConstants( cb
									, m_quads[i]->getPipelineLayout()
									, VK_SHADER_STAGE_FRAGMENT_BIT
									, 0u
									, sizeof( c3d::Point2f )
									, invSize.constPtr() );
							} );

					if ( count > 1u )
					{
						builder.passIndex( passIndex );
					}

					auto result = builder.build( framePass, context, runGraph
						, crg::ru::Config{ count } );
					m_quads.push_back( result.get() );
					c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			pass.addInputSampled( *src, 0u
				, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest } );
			src = resultImg[i].setLastAttach( pass.addOutputColourTarget( resultImg[i].getTargetViewId() ) );
		}
	}


	void DownsamplePass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}
}

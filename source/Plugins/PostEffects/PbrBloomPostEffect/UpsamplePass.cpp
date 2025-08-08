#include "PbrBloomPostEffect/UpsamplePass.hpp"

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
	namespace up
	{
		namespace c3ds = c3d::shader;

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColor = writer.declCombinedImg< Img2DRgba >( "c3d_mapColor", 0u, 0u );
			auto constants = writer.declUniformBuffer( "Constants", 1u, 0u );
			auto filterRadius = constants.declMember< sdw::Float >( "filterRadius" );
			auto bloomStrength = constants.declMember< sdw::Float >( "bloomStrength" );
			constants.end();

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( [&]( sdw::VertexInT< c3ds::PosUv2FT > in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour3FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > in
				, sdw::FragmentOutT< c3ds::Colour3FT > out )
				{
					// The filter kernel is applied with a radius, specified in texture
					// coordinates, so that the radius will vary across mip resolutions.
					auto x = writer.declLocale( "x"
						, filterRadius );
					auto y = writer.declLocale( "x"
						, filterRadius );

					// Take 9 samples around current texel:
					// a - b - c
					// d - e - f
					// g - h - i
					// === ('e' is the current texel) ===
					auto a = writer.declLocale( "a"
						, c3d_mapColor.sample( vec2( in.uv().x() - x, in.uv().y() + y ) ).rgb() );
					auto b = writer.declLocale( "b"
						, c3d_mapColor.sample( vec2( in.uv().x(), in.uv().y() + y ) ).rgb() );
					auto c = writer.declLocale( "c"
						, c3d_mapColor.sample( vec2( in.uv().x() + x, in.uv().y() + y ) ).rgb() );

					auto d = writer.declLocale( "d"
						, c3d_mapColor.sample( vec2( in.uv().x() - x, in.uv().y() ) ).rgb() );
					auto e = writer.declLocale( "e"
						, c3d_mapColor.sample( vec2( in.uv().x(), in.uv().y() ) ).rgb() );
					auto f = writer.declLocale( "f"
						, c3d_mapColor.sample( vec2( in.uv().x() + x, in.uv().y() ) ).rgb() );

					auto g = writer.declLocale( "g"
						, c3d_mapColor.sample( vec2( in.uv().x() - x, in.uv().y() - y ) ).rgb() );
					auto h = writer.declLocale( "h"
						, c3d_mapColor.sample( vec2( in.uv().x(), in.uv().y() - y ) ).rgb() );
					auto i = writer.declLocale( "i"
						, c3d_mapColor.sample( vec2( in.uv().x() + x, in.uv().y() - y ) ).rgb() );

					// Apply weighted distribution, by using a 3x3 tent filter:
					//  1   | 1 2 1 |
					// -- * | 2 4 2 |
					// 16   | 1 2 1 |
					out.colour() = e * 4.0f;
					out.colour() += ( b + d + f + h ) * 2.0f;
					out.colour() += ( a + c + g + i );
					out.colour() *= 1.0_f / 16.0f;
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	UpsamplePass::UpsamplePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::Texture const & sourceImg
		, c3d::Vector< c3d::Texture > & resultImg
		, c3d::UniformBufferOffsetT< c3d::Point2f > const & ubo
		, uint32_t passesCount
		, bool const * enabled )
		: m_shader{ cuT( "PbrBloomUpsample" ), up::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto src = sourceImg.getLastAttach();

		for ( auto i = int32_t( passesCount - 2u ); i >= 0; --i )
		{
			auto index = uint32_t( i );
			auto dstExtent = c3d::makeExtent2D( getMipExtent( resultImg[index].getSampledViewId() ) );
			auto & pass = graph.createPass( "Upsample" + c3d::string::toMbString( index )
				, [this, &device, enabled, dstExtent]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = crg::RenderQuadBuilder{}
						.enabled( enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
						.renderSize( dstExtent )
						.texcoordConfig( {} )
						.build( framePass, context, graph, crg::ru::Config{} );
					device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			pass.addInputSampled( *src, 0u
				, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest } );
			ubo.createPassBinding( pass, 1u );
			src = resultImg[index].setLastAttach( pass.addOutputColourTarget( resultImg[index].getTargetViewId() ) );
		}
	}

	void UpsamplePass::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}

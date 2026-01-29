#include "LightStreaksPostEffect/KawasePass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace light_streaks
{
	namespace kawase
	{
		namespace c3ds = c3d::shader;

		enum class Bindings
		{
			KawaseUboIdx,
			DifImgIdx,
		};

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			C3D_Kawase( writer, Bindings::KawaseUboIdx, 0u );
			auto c3d_mapHiPass = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHiPass", Bindings::DifImgIdx, 0u );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::PosUv2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_kawaseData, &c3d_mapHiPass]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					auto colour = writer.declLocale( "colour"
						, vec3( 0.0_f ) );
					auto b = writer.declLocale( "b"
						, pow( writer.cast< sdw::Float >( c3d_kawaseData.samples ), writer.cast< sdw::Float >( c3d_kawaseData.pass ) ) );
					auto texcoords = writer.declLocale( "texcoords"
						, in.uv() );

					sdwFOR( writer, sdw::Int, s, 0, s < c3d_kawaseData.samples, ++s )
					{
						// Weight = a^(b*s)
						auto weight = writer.declLocale( "weight"
							, pow( c3d_kawaseData.attenuation, b * writer.cast< sdw::Float >( s ) ) );
						// Streak direction is a 2D vector in image space
						auto sampleCoord = writer.declLocale( "sampleCoord"
							, texcoords + ( c3d_kawaseData.direction * b * vec2( s, s ) * c3d_kawaseData.pixelSize ) );
						// Scale and accumulate
						colour += c3d_mapHiPass.sample( sampleCoord ).rgb() * clamp( weight, 0.0_f, 1.0_f );
					}
					sdwROF

					out.colour() = vec4( clamp( colour, vec3( 0.0_f ), vec3( 1.0_f ) ), 1.0_f );
				} );
			return writer.getBuilder().releaseShader();
		}

		static void createSubpass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture const & srcView, uint32_t srcIndex
			, c3d::Texture & dstView, uint32_t dstIndex
			, c3d::Extent2D dimensions
			, ashes::PipelineShaderStageCreateInfoArray const & stages
			, KawaseUbo const & kawaseUbo
			, uint32_t index
			, bool const * enabled )
		{
			auto & pass = graph.createPass( "Kawase" + c3d::string::toMbString( index )
				, [&device, &stages, dimensions, enabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = crg::RenderQuadBuilder{}
						.renderPosition( {} )
						.renderSize( dimensions )
						.texcoordConfig( crg::Texcoord{} )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) )
						.enabled( enabled )
						.build( framePass, context, graph );
					c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			kawaseUbo.createPassBinding( pass, kawase::Bindings::KawaseUboIdx, index );
			pass.addInputSampledT( *srcView.getSampledLastAttach( srcIndex ), kawase::Bindings::DifImgIdx
				, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest } );
			dstView.setLastAttach( dstIndex, pass.addOutputColourTarget( dstView.getTargetViewId( dstIndex ) ) );
		}
	}

	//*********************************************************************************************

	KawasePass::KawasePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::Texture & hiViews
		, c3d::Texture & kawaseViews
		, KawaseUbo const & kawaseUbo
		, c3d::Extent2D dimensions
		, bool const * enabled )
		: m_shader{ cuT( "LightStreaksKawasePass" ), kawase::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		assert( hiViews.size() == kawaseViews.size() + 1u
			&& kawaseViews.size() == PostEffect::Count );
		uint32_t index = 0u;

		for ( auto targetIndex = 0u; targetIndex < PostEffect::Count; ++targetIndex )
		{
			auto sourceIndex = targetIndex + 1u;
			auto destinationIndex = targetIndex;
			auto * source = &hiViews;
			auto * destination = &kawaseViews;
			kawase::createSubpass( graph, device
				, *source, sourceIndex
				, *destination, destinationIndex
				, dimensions, m_stages, kawaseUbo
				, index, enabled );
			++index;

			for ( auto j = 1u; j < 3u; ++j )
			{
				c3d::swap( source, destination );
				c3d::swap( sourceIndex, destinationIndex );
				kawase::createSubpass( graph, device
					, *source, sourceIndex
					, *destination, destinationIndex
					, dimensions, m_stages, kawaseUbo
					, index, enabled );
				++index;
			}
		}
	}

	void KawasePass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}
}

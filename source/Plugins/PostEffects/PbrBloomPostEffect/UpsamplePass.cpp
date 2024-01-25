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
		namespace c3d = castor3d::shader;

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );
			auto constants = writer.declUniformBuffer( "Constants", 1u, 0u );
			auto filterRadius = constants.declMember< sdw::Float >( "filterRadius" );
			auto bloomStrength = constants.declMember< sdw::Float >( "bloomStrength" );
			constants.end();

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour3FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour3FT > out )
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
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageId const & image
		, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
		, uint32_t passesCount
		, bool const * enabled )
		: m_graph{ graph }
		, m_shader{ cuT( "PbrBloomUpsample" ), up::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_resultViews{ doCreateResultViews( graph, image, passesCount ) }
		, m_passes{ doCreatePasses( graph, previousPass, device, ubo, passesCount, enabled ) }
	{
	}


	void UpsamplePass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );

		for ( auto & view : m_resultViews )
		{
			visitor.visit( cuT( "PostFX: PBRB - Up " ) + castor::string::toString( view.data->info.subresourceRange.baseMipLevel )
				, view
				, m_graph.getFinalLayoutState( view ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}
	}

	crg::ImageViewIdArray UpsamplePass::doCreateResultViews( crg::FramePassGroup & graph
		, crg::ImageId const & resultImg
		, uint32_t passesCount )
	{
		crg::ImageViewIdArray result;

		for ( uint32_t i = 0u; i < passesCount; ++i )
		{
			result.push_back( graph.createView( crg::ImageViewData{ resultImg.data->name + castor::string::toMbString( i )
				, resultImg
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, resultImg.data->info.format
				, { VK_IMAGE_ASPECT_COLOR_BIT, i, 1u, 0u, 1u } } ) );
		}

		return result;
	}

	castor::Vector< crg::FramePass * > UpsamplePass::doCreatePasses( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
		, uint32_t passesCount
		, bool const * enabled )
	{
		castor::Vector< crg::FramePass * > result;
		auto prev = &previousPass;
		auto src = &m_resultViews.back();

		for ( auto i = int32_t( passesCount - 2u ); i >= 0; --i )
		{
			auto dstExtent = castor3d::makeExtent2D( getMipExtent( m_resultViews[uint32_t( i )] ) );
			auto & pass = graph.createPass( "Upsample" + castor::string::toMbString( i )
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
					device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );

			pass.addDependency( *prev );
			pass.addSampledView( *src
				, 0u
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_NEAREST
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
					, 0.0f
					, float( i )
					, float( i + 1u ) } );
			ubo.createPassBinding( pass
				, "PbrBloomUbo"
				, 1u );
			pass.addOutputColourView( m_resultViews[uint32_t( i )] );
			result.push_back( &pass );
			prev = &pass;
			src = &m_resultViews[uint32_t( i )];
		}

		return result;
	}
}

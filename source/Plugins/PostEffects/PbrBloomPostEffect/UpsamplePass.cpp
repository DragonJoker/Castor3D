#include "PbrBloomPostEffect/UpsamplePass.hpp"

#include "PbrBloomPostEffect/PbrBloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace PbrBloom
{
	namespace up
	{
		static std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			auto constants = writer.declUniformBuffer( "Constants", 1u, 0u );
			auto filterRadius = constants.declMember< sdw::Float >( "filterRadius" );
			auto bloomStrength = constants.declMember< sdw::Float >( "bloomStrength" );
			constants.end();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec3 >( "pxl_fragColor", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
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
						, c3d_mapColor.sample( vec2( vtx_texture.x() - x, vtx_texture.y() + y ) ).rgb() );
					auto b = writer.declLocale( "b"
						, c3d_mapColor.sample( vec2( vtx_texture.x(), vtx_texture.y() + y ) ).rgb() );
					auto c = writer.declLocale( "c"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + x, vtx_texture.y() + y ) ).rgb() );

					auto d = writer.declLocale( "d"
						, c3d_mapColor.sample( vec2( vtx_texture.x() - x, vtx_texture.y() ) ).rgb() );
					auto e = writer.declLocale( "e"
						, c3d_mapColor.sample( vec2( vtx_texture.x(), vtx_texture.y() ) ).rgb() );
					auto f = writer.declLocale( "f"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + x, vtx_texture.y() ) ).rgb() );

					auto g = writer.declLocale( "g"
						, c3d_mapColor.sample( vec2( vtx_texture.x() - x, vtx_texture.y() - y ) ).rgb() );
					auto h = writer.declLocale( "h"
						, c3d_mapColor.sample( vec2( vtx_texture.x(), vtx_texture.y() - y ) ).rgb() );
					auto i = writer.declLocale( "i"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + x, vtx_texture.y() - y ) ).rgb() );

					// Apply weighted distribution, by using a 3x3 tent filter:
					//  1   | 1 2 1 |
					// -- * | 2 4 2 |
					// 16   | 1 2 1 |
					pxl_fragColor = e * 4.0f;
					pxl_fragColor += ( b + d + f + h ) * 2.0f;
					pxl_fragColor += ( a + c + g + i );
					pxl_fragColor *= 1.0_f / 16.0f;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "PbrBloomUpsample", up::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "PbrBloomUpsample", up::getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_resultViews{ doCreateResultViews( graph, image, passesCount ) }
		, m_passes{ doCreatePasses( graph, previousPass, device, ubo, passesCount, enabled ) }
	{
	}


	void UpsamplePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );

		for ( auto & view : m_resultViews )
		{
			visitor.visit( "PostFX: PBRB - Up " + std::to_string( view.data->info.subresourceRange.baseMipLevel )
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
			result.push_back( graph.createView( crg::ImageViewData{ resultImg.data->name + castor::string::toString( i )
				, resultImg
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, resultImg.data->info.format
				, { VK_IMAGE_ASPECT_COLOR_BIT, i, 1u, 0u, 1u } } ) );
		}

		return result;
	}

	std::vector< crg::FramePass * > UpsamplePass::doCreatePasses( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
		, uint32_t passesCount
		, bool const * enabled )
	{
		std::vector< crg::FramePass * > result;
		auto prev = &previousPass;
		auto src = &m_resultViews.back();

		for ( auto i = int32_t( passesCount - 2u ); i >= 0; --i )
		{
			auto dstExtent = castor3d::makeExtent2D( getMipExtent( m_resultViews[uint32_t( i )] ) );
			auto & pass = graph.createPass( "Upsample" + std::to_string( i )
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
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );

			pass.addDependency( *prev );
			pass.addSampledView( *src
				, 0u
				, VK_IMAGE_LAYOUT_UNDEFINED
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
				, std::string{ "PbrBloomUbo" }
				, 1u );
			pass.addOutputColourView( m_resultViews[uint32_t( i )] );
			result.push_back( &pass );
			prev = &pass;
			src = &m_resultViews[uint32_t( i )];
		}

		return result;
	}
}

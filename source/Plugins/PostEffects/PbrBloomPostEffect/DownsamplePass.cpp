#include "PbrBloomPostEffect/DownsamplePass.hpp"

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
	namespace down
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

			auto constants = writer.declPushConstantsBuffer<>( "constants" );
			auto srcTexelSize = constants.declMember< sdw::Vec2 >( "srcTexelSize" );
			constants.end();

			// Shader inputs
			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto outColour = writer.declOutput< Vec3 >( "outColour", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
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
						, c3d_mapColor.sample( vec2( vtx_texture.x() - 2.0f * x, vtx_texture.y() + 2.0f * y ) ).rgb() );
					auto b = writer.declLocale( "b"
						, c3d_mapColor.sample( vec2( vtx_texture.x(), vtx_texture.y() + 2.0f * y ) ).rgb() );
					auto c = writer.declLocale( "c"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + 2.0f * x, vtx_texture.y() + 2.0f * y ) ).rgb() );

					auto d = writer.declLocale( "d"
						, c3d_mapColor.sample( vec2( vtx_texture.x() - 2.0f * x, vtx_texture.y() ) ).rgb() );
					auto e = writer.declLocale( "e"
						, c3d_mapColor.sample( vec2( vtx_texture.x(), vtx_texture.y() ) ).rgb() );
					auto f = writer.declLocale( "f"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + 2.0f * x, vtx_texture.y() ) ).rgb() );

					auto g = writer.declLocale( "g"
						, c3d_mapColor.sample( vec2( vtx_texture.x() - 2.0f * x, vtx_texture.y() - 2.0f * y ) ).rgb() );
					auto h = writer.declLocale( "h"
						, c3d_mapColor.sample( vec2( vtx_texture.x(), vtx_texture.y() - 2.0f * y ) ).rgb() );
					auto i = writer.declLocale( "i"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + 2.0f * x, vtx_texture.y() - 2.0f * y ) ).rgb() );

					auto j = writer.declLocale( "j"
						, c3d_mapColor.sample( vec2( vtx_texture.x() - x, vtx_texture.y() + y ) ).rgb() );
					auto k = writer.declLocale( "k"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + x, vtx_texture.y() + y ) ).rgb() );
					auto l = writer.declLocale( "l"
						, c3d_mapColor.sample( vec2( vtx_texture.x() - x, vtx_texture.y() - y ) ).rgb() );
					auto m = writer.declLocale( "m"
						, c3d_mapColor.sample( vec2( vtx_texture.x() + x, vtx_texture.y() - y ) ).rgb() );

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
					outColour = e * 0.125f;
					outColour += ( a + c + g + i ) * 0.03125f;
					outColour += ( b + d + f + h ) * 0.0625f;
					outColour += ( j + k + l + m ) * 0.125f;
					outColour = max( outColour, vec3( 0.0001_f ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	DownsamplePass::DownsamplePass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & sceneView
		, crg::ImageId const & resultImg
		, uint32_t passesCount
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_graph{ graph }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "PbrBloomDownsample", down::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "PbrBloomDownsample", down::getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_resultViews{ doCreateResultViews( graph, resultImg, passesCount ) }
		, m_passes{ doCreatePasses( graph, previousPass, device, sceneView, passesCount, enabled, passIndex ) }
	{
	}


	void DownsamplePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );

		for ( auto & view : m_resultViews )
		{
			visitor.visit( "PostFX: PBRB - Down " + std::to_string( view.data->info.subresourceRange.baseMipLevel )
				, view
				, m_graph.getFinalLayoutState( view ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}
	}

	crg::ImageViewIdArray DownsamplePass::doCreateResultViews( crg::FramePassGroup & graph
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

	std::vector< crg::FramePass * > DownsamplePass::doCreatePasses( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & sceneView
		, uint32_t passesCount
		, bool const * enabled
		, uint32_t const * passIndex )
	{
		std::vector< crg::FramePass * > result;
		auto prev = &previousPass;
		auto src = sceneView;

		for ( uint32_t i = 0u; i < passesCount; ++i )
		{
			auto srcExtent = castor3d::makeExtent2D( getMipExtent( src.front() ) );
			auto dstExtent = castor3d::makeExtent2D( getMipExtent( m_resultViews[i] ) );
			auto count = uint32_t( src.size() );
			auto & pass = graph.createPass( "Downsample" + std::to_string( i )
				, [this, &device, passIndex, enabled, count, srcExtent, dstExtent, i]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto builder = crg::RenderQuadBuilder{}
						.enabled( enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
						.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_FRAGMENT_BIT, 0u, sizeof( castor::Point2f ) } )
						.renderSize( dstExtent )
						.texcoordConfig( {} )
						.recordInto( [this, srcExtent, i]( crg::RecordContext & ctx
							, VkCommandBuffer cb
							, uint32_t idx )
							{
								castor::Point2f invSize{ 1.0f / float( srcExtent.width )
									, 1.0f / float( srcExtent.height ) };
								ctx.getContext().vkCmdPushConstants( cb
									, m_quads[i]->getPipelineLayout()
									, VK_SHADER_STAGE_FRAGMENT_BIT
									, 0u
									, sizeof( castor::Point2f )
									, invSize.constPtr() );
							} );

					if ( count > 1u )
					{
						builder.passIndex( passIndex );
					}

					auto result = builder.build( framePass
						, context
						, graph
						, crg::ru::Config{ count } );
					m_quads.push_back( result.get() );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );

			pass.addDependency( *prev );
			pass.addSampledView( src
				, 0u
				, VK_IMAGE_LAYOUT_UNDEFINED
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_NEAREST
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
					, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
					, 0.0f
					, float( i == 0u ? i : i - 1u )
					, float( i == 0u ? i + 1u : i ) } );
			pass.addOutputColourView( m_resultViews[i] );
			result.push_back( &pass );
			prev = &pass;
			src = { m_resultViews[i] };
		}

		return result;
	}
}

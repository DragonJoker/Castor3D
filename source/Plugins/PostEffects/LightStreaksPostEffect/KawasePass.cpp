#include "LightStreaksPostEffect/KawasePass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace light_streaks
{
	namespace
	{
		enum Idx
		{
			KawaseUboIdx,
			DifImgIdx,
		};

		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_KAWASE( writer, KawaseUboIdx, 0u );
			auto c3d_mapHiPass = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapHiPass", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto colour = writer.declLocale( "colour"
						, vec3( 0.0_f ) );
					auto b = writer.declLocale( "b"
						, pow( writer.cast< Float >( c3d_samples ), writer.cast< Float >( c3d_pass ) ) );
					auto texcoords = writer.declLocale( "texcoords"
						, vtx_texture );

					FOR( writer, Int, s, 0, s < c3d_samples, ++s )
					{
						// Weight = a^(b*s)
						auto weight = writer.declLocale( "weight"
							, pow( c3d_attenuation, b * writer.cast< Float >( s ) ) );
						// Streak direction is a 2D vector in image space
						auto sampleCoord = writer.declLocale( "sampleCoord"
							, texcoords + ( c3d_direction * b * vec2( s, s ) * c3d_pixelSize ) );
						// Scale and accumulate
						colour += c3d_mapHiPass.sample( sampleCoord ).rgb() * clamp( weight, 0.0_f, 1.0_f );
					}
					ROF;

					pxl_fragColor = vec4( clamp( colour, vec3( 0.0_f ), vec3( 1.0_f ) ), 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	KawasePass::Subpass::Subpass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & srcView
		, crg::ImageViewId const & dstView
		, VkExtent2D dimensions
		, ashes::PipelineShaderStageCreateInfoArray const & stages
		, KawaseUbo const & kawaseUbo
		, uint32_t index
		, bool const * enabled )
		: pass{ graph.createPass( "LightStreaksKawasePass" + std::to_string( index )
			, [this, &device, &stages, &srcView, dimensions, index, enabled]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( dimensions )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) )
					.enabled( enabled )
					.build( pass, context, graph );
				device.renderSystem.getEngine()->registerTimer( "LightStreaks"
					, result->getTimer() );
				return result;
			} ) }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		pass.addDependency( previousPass );
		kawaseUbo.createPassBinding( pass
			, KawaseUboIdx
			, index );
		pass.addSampledView( srcView
			, DifImgIdx
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addOutputColourView( dstView );
	}

	std::vector< KawasePass::Subpass > doCreateSubpasses( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & srcImages
		, crg::ImageViewIdArray const & dstImages
		, VkExtent2D dimensions
		, ashes::PipelineShaderStageCreateInfoArray const & stages
		, KawaseUbo const & kawaseUbo
		, bool const * enabled )
	{
		std::vector< KawasePass::Subpass > result;
		assert( srcImages.size() == dstImages.size() + 1u
			&& dstImages.size() == PostEffect::Count );
		uint32_t index = 0u;

		for ( auto i = 0u; i < PostEffect::Count; ++i )
		{
			auto * source = &srcImages[0u];
			auto * destination = &dstImages[i];
			result.emplace_back( graph
				, *previousPass
				, device
				, *source
				, *destination
				, dimensions
				, stages
				, kawaseUbo
				, index
				, enabled );
			previousPass = &result.back().pass;
			++index;
			source = &srcImages[1u];

			for ( auto j = 1u; j < 3u; ++j )
			{
				std::swap( source, destination );
				result.emplace_back( graph
					, *previousPass
					, device
					, *source
					, *destination
					, dimensions
					, stages
					, kawaseUbo
					, index
					, enabled );
				previousPass = &result.back().pass;
				++index;
			}
		}

		return result;
	}

	//*********************************************************************************************

	KawasePass::KawasePass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & hiViews
		, crg::ImageViewIdArray const & kawaseViews
		, KawaseUbo & kawaseUbo
		, VkExtent2D dimensions
		, bool const * enabled )
		: m_device{ device }
		, m_kawaseUbo{ kawaseUbo }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksKawasePass", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "LightStreaksKawasePass", getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_lastPass{ &previousPass }
		, m_subpasses{ doCreateSubpasses( graph
			, m_lastPass
			, m_device
			, hiViews
			, kawaseViews
			, dimensions
			, m_stages
			, m_kawaseUbo
			, enabled ) }
	{
	}

	void KawasePass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );

		visitor.visit( m_pixelShader.name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Kawase" )
			, cuT( "Attenuation" )
			, m_kawaseUbo.getUbo( 0u ).getData().attenuation );
		visitor.visit( m_pixelShader.name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Kawase" )
			, cuT( "Samples" )
			, m_kawaseUbo.getUbo( 0u ).getData().samples );
	}
}

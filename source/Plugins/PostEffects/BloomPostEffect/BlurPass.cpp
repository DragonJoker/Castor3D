#include "BloomPostEffect/BlurPass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace Bloom
{
	namespace
	{
		enum Idx
		{
			GaussCfgUboIdx,
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
			Ubo config{ writer, castor3d::GaussianBlur::Config, GaussCfgUboIdx, 0u };
			auto c3d_pixelSize = config.declMember< Vec2 >( castor3d::GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( castor3d::GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( castor3d::GaussianBlur::Coefficients, castor3d::GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto offset = writer.declLocale( "offset"
						, vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = c3d_mapSource.sample( vtx_texture ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1u, i < c3d_coefficientsCount, ++i )
					{
						offset += c3d_pixelSize;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture + offset );
					}
					ROF;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::vector< float > getHalfPascal( uint32_t height )
		{
			std::vector< float > result;
			result.resize( height );
			auto x = 1.0f;
			auto max = 1 + height;

			for ( uint32_t i = 0u; i <= max; ++i )
			{
				auto index = max - i;

				if ( index < height )
				{
					result[index] = x;
				}

				x = x * ( ( height + 1 ) * 2 - i ) / ( i + 1 );
			}

			// Normalize kernel coefficients
			float sum = result[0];

			for ( uint32_t i = 1u; i < height; ++i )
			{
				sum += result[i] * 2;
			}

			std::transform( std::begin( result )
				, std::end( result )
				, std::begin( result )
				, [&sum]( float & value )
				{
					return value /= sum;
				} );

			return result;
		}

		std::array< castor::Point4f, 15u > doCreateKernel( uint32_t count )
		{
			std::array< castor::Point4f, 15u > result;
			auto kernel = getHalfPascal( count );
			std::memcpy( result.data()->ptr()
				, kernel.data()
				, sizeof( float ) * std::min( size_t( 60u ), kernel.size() ) );
			return result;
		}

		UboOffsetArray doCreateUbo( castor3d::RenderDevice const & device
			, VkExtent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical )
		{
			UboOffsetArray result;
			auto coefficientsCount = blurKernelSize;
			auto kernel = doCreateKernel( coefficientsCount );

			for ( auto i = 0u; i < blurPassesCount; ++i )
			{
				auto ubo = device.uboPools->getBuffer< castor3d::GaussianBlur::Configuration >( 0u );
				auto & data = ubo.getData();
				data.textureSize = castor::Point2f
				{
					isVertical ? 0.0f : 1.0f / float( dimensions.width >> ( i + 1 ) ),
					isVertical ? 1.0f / float( dimensions.height >> ( i + 1 ) ) : 0.0f
				};
				data.blurCoeffsCount = coefficientsCount;
				data.blurCoeffs = kernel;
				result.emplace_back( std::move( ubo ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	BlurPass::Subpass::Subpass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & srcView
		, crg::ImageViewId const & dstView
		, VkExtent2D dimensions
		, ashes::PipelineShaderStageCreateInfoArray const & stages
		, castor3d::UniformBufferOffsetT< castor3d::GaussianBlur::Configuration > const & blurUbo
		, uint32_t index
		, bool isVertical
		, bool const * enabled )
		: pass{ graph.createPass( "BloomBlurPass" + std::to_string( index ) + ( isVertical ? "Y" : "X" )
			, [this, &device, &stages, dimensions, index, enabled]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( { dimensions.width >> ( index + 1 )
						, dimensions.height >> ( index + 1 ) } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) )
					.enabled( enabled )
					.build( pass, context, graph );
				device.renderSystem.getEngine()->registerTimer( "Bloom"
					, result->getTimer() );
				return result;
			} ) }
	{
		pass.addDependency( previousPass );
		blurUbo.createPassBinding( pass, std::string{ "BlurCfg" } + ( isVertical ? "Y" : "X" ), GaussCfgUboIdx );
		pass.addSampledView( srcView
			, DifImgIdx
			, {}
			, crg::SamplerDesc{ VK_FILTER_NEAREST
				, VK_FILTER_NEAREST
				, VK_SAMPLER_MIPMAP_MODE_NEAREST
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, 0.0f
				, float( index )
				, float( index + 1u ) } );
		pass.addOutputColourView( dstView );
	}

	std::vector< BlurPass::Subpass > doCreateSubpasses( crg::FrameGraph & graph
		, crg::FramePassArray & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & srcImages
		, crg::ImageViewIdArray const & dstImages
		, VkExtent2D dimensions
		, ashes::PipelineShaderStageCreateInfoArray const & stages
		, UboOffsetArray const & blurUbo
		, uint32_t blurPassesCount
		, bool isVertical
		, bool const * enabled )
	{
		std::vector< BlurPass::Subpass > result;
		assert( srcImages.size() == dstImages.size()
			&& srcImages.size() == blurPassesCount );

		for ( auto i = 0u; i < blurPassesCount; ++i )
		{
			result.emplace_back( graph
				, *previousPasses[i]
				, device
				, srcImages[i]
				, dstImages[i]
				, dimensions
				, stages
				, blurUbo[i]
				, i
				, isVertical
				, enabled );
			previousPasses[i] = &result.back().pass;
		}

		return result;
	}

	//*********************************************************************************************

	BlurPass::BlurPass( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & srcImages
		, crg::ImageViewIdArray const & dstImages
		, VkExtent2D dimensions
		, uint32_t blurKernelSize
		, uint32_t blurPassesCount
		, bool isVertical
		, bool const * enabled )
		: m_device{ device }
		, m_blurKernelSize{ blurKernelSize }
		, m_blurPassesCount{ blurPassesCount }
		, m_blurUbo{ doCreateUbo( m_device, dimensions, m_blurKernelSize, m_blurPassesCount, isVertical ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "BloomBlurPass", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "BloomBlurPass", getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_passes{ previousPasses }
		, m_subpasses{ doCreateSubpasses( graph
			, m_passes
			, m_device
			, srcImages
			, dstImages
			, dimensions
			, m_stages
			, m_blurUbo
			, m_blurPassesCount
			, isVertical
			, enabled ) }
		, m_isVertical{ isVertical }
	{
	}

	BlurPass::BlurPass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & srcImages
		, crg::ImageViewIdArray const & dstImages
		, VkExtent2D dimensions
		, uint32_t blurKernelSize
		, uint32_t blurPassesCount
		, bool isVertical
		, bool const * enabled )
		: BlurPass{ graph
			, { blurPassesCount, &previousPass }
			, device
			, srcImages
			, dstImages
			, dimensions
			, blurKernelSize
			, blurPassesCount
			, isVertical
			, enabled }
	{
	}

	void BlurPass::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}
}

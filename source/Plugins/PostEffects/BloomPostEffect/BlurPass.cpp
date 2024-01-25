#include "BloomPostEffect/BlurPass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace Bloom
{
	namespace blur
	{
		namespace c3d = castor3d::shader;

		enum Idx
		{
			GaussCfgUboIdx,
			DifImgIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto config = writer.declUniformBuffer( castor3d::GaussianBlur::Config, GaussCfgUboIdx, 0u );
			auto c3d_pixelSize = config.declMember< sdw::Vec2 >( castor3d::GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< sdw::UInt >( castor3d::GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< sdw::UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< sdw::Vec4 >( castor3d::GaussianBlur::Coefficients, castor3d::GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );

			writer.implementEntryPointT< c3d::Position2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::Position2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto offset = writer.declLocale( "offset"
						, vec2( 0.0_f, 0.0_f ) );
					out.colour() = c3d_mapSource.sample( in.uv() ) * c3d_coefficients[0_u][0_u];

					FOR( writer, sdw::UInt, i, 1u, i < c3d_coefficientsCount, ++i )
					{
						offset += c3d_pixelSize;
						out.colour() += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.uv() - offset );
						out.colour() += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.uv() + offset );
					}
					ROF;
				} );
			return writer.getBuilder().releaseShader();
		}

		static castor::Vector< float > getHalfPascal( uint32_t height )
		{
			castor::Vector< float > result;
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

				x = x * float( ( height + 1 ) * 2 - i ) / float( i + 1 );
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

		static castor::Array< castor::Point4f, 15u > doCreateKernel( uint32_t count )
		{
			castor::Array< castor::Point4f, 15u > result;
			auto kernel = getHalfPascal( count );
			std::memcpy( result.data()->ptr()
				, kernel.data()
				, sizeof( float ) * std::min( size_t( 60u ), kernel.size() ) );
			return result;
		}

		static UboOffsetArray doCreateUbo( castor3d::RenderDevice const & device
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
				auto ubo = device.uboPool->getBuffer< castor3d::GaussianBlur::Configuration >( 0u );
				auto & data = ubo.getData();
				data.textureSize = castor::Point2f
				{
					isVertical ? 0.0f : 1.0f / float( dimensions.width >> ( i + 1 ) ),
					isVertical ? 1.0f / float( dimensions.height >> ( i + 1 ) ) : 0.0f
				};
				data.blurCoeffsCount = coefficientsCount;
				data.blurCoeffs = kernel;
				result.emplace_back( castor::move( ubo ) );
			}

			return result;
		}

		static castor::Vector< BlurPass::Subpass > doCreateSubpasses( crg::FramePassGroup & graph
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
			castor::Vector< BlurPass::Subpass > result;
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
	}

	//*********************************************************************************************

	BlurPass::Subpass::Subpass( crg::FramePassGroup & graph
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
		: pass{ graph.createPass( "Blur" + castor::string::toMbString( index ) + ( isVertical ? "Y" : "X" )
			, [&device, &stages, dimensions, index, enabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( { dimensions.width >> ( index + 1 )
						, dimensions.height >> ( index + 1 ) } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) )
					.enabled( enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} ) }
	{
		pass.addDependency( previousPass );
		blurUbo.createPassBinding( pass, castor::MbString{ "BlurCfg" } + ( isVertical ? "Y" : "X" ), blur::GaussCfgUboIdx );
		pass.addSampledView( srcView
			, blur::DifImgIdx
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

	//*********************************************************************************************

	BlurPass::BlurPass( crg::FramePassGroup & graph
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
		, m_blurPassesCount{ blurPassesCount }
		, m_blurUbo{ blur::doCreateUbo( m_device, dimensions, blurKernelSize, m_blurPassesCount, isVertical ) }
		, m_shader{ cuT( "BloomBlurPass" ), blur::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_passes{ previousPasses }
		, m_subpasses{ blur::doCreateSubpasses( graph
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
	{
	}

	BlurPass::BlurPass( crg::FramePassGroup & graph
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

	void BlurPass::update( uint32_t kernelSize )
	{
		auto kernel = blur::doCreateKernel( kernelSize );

		for ( auto & ubo : m_blurUbo )
		{
			auto & data = ubo.getData();
			data.blurCoeffsCount = kernelSize;
			data.blurCoeffs = kernel;
		}
	}

	void BlurPass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}

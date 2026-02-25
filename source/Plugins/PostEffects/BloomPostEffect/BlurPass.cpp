#include "BloomPostEffect/BlurPass.hpp"

#include "BloomPostEffect/BloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
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
		namespace c3ds = c3d::shader;

		enum class Bindings
		{
			GaussCfgUboIdx,
			DifImgIdx,
		};

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto config = writer.declUniformBuffer( c3d::GaussianBlur::Config, Bindings::GaussCfgUboIdx, 0u );
			auto c3d_pixelSize = config.declMember< sdw::Vec2 >( c3d::GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< sdw::UInt >( c3d::GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< sdw::UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< sdw::Vec4 >( c3d::GaussianBlur::Coefficients, c3d::GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapSource", Bindings::DifImgIdx, 0u );

			writer.implementEntryPointT< c3ds::Position2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_mapSource, c3d_coefficients, &c3d_coefficientsCount, &c3d_pixelSize]
				( sdw::FragmentInT< c3ds::Uv2FT > const & in
					, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					auto offset = writer.declLocale( "offset"
						, vec2( 0.0_f, 0.0_f ) );
					out.colour() = c3d_mapSource.sample( in.uv() ) * c3d_coefficients[0_u][0_u];

					sdwFOR( writer, sdw::UInt, i, 1u, i < c3d_coefficientsCount, ++i )
					{
						offset += c3d_pixelSize;
						out.colour() += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.uv() - offset );
						out.colour() += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.uv() + offset );
					}
					sdwROF
				} );
			return writer.getBuilder().releaseShader();
		}

		static c3d::Vector< float > getHalfPascal( uint32_t height )
		{
			c3d::Vector< float > result;
			result.resize( height );
			auto x = 1.0f;
			auto max = 1 + height;

			for ( uint32_t i = 0u; i <= max; ++i )
			{
				if ( auto index = max - i;
					index < height )
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

		static c3d::Array< c3d::Point4f, 15u > doCreateKernel( uint32_t count )
		{
			c3d::Array< c3d::Point4f, 15u > result;
			auto kernel = getHalfPascal( count );
			std::memcpy( result.data()->ptr()
				, kernel.data()
				, sizeof( float ) * std::min( size_t( 60u ), kernel.size() ) );
			return result;
		}

		static UboArray doCreateUbos( c3d::RenderDevice const & device
			, c3d::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical )
		{
			UboArray result;
			auto coefficientsCount = blurKernelSize;
			auto kernel = doCreateKernel( coefficientsCount );

			for ( auto i = 0u; i < blurPassesCount; ++i )
			{
				auto & ubo = result.emplace_back( device );
				c3d::GaussianBlur::Configuration data{};
				data.textureSize = c3d::Point2f
				{
					isVertical ? 0.0f : 1.0f / float( dimensions.width >> ( i + 1 ) ),
					isVertical ? 1.0f / float( dimensions.height >> ( i + 1 ) ) : 0.0f
				};
				data.blurCoeffsCount = coefficientsCount;
				data.blurCoeffs = kernel;
				ubo.setData( c3d::move( data ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	BlurPass::BlurPass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::Texture const & srcImage
		, c3d::Texture & dstImage
		, c3d::Extent2D dimensions
		, uint32_t blurKernelSize
		, uint32_t blurPassesCount
		, bool isVertical
		, bool const * enabled )
		: m_device{ device }
		, m_blurUbos{ blur::doCreateUbos( m_device, dimensions, blurKernelSize, blurPassesCount, isVertical ) }
		, m_shader{ cuT( "BloomBlurPass" ), blur::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_result{ dstImage }
	{
		assert( srcImage.getMipLevels() == m_result.getMipLevels()
			&& srcImage.getMipLevels() == blurPassesCount );
		crg::AttachmentArray attachs;

		for ( auto index = 0u; index < blurPassesCount; ++index )
		{
			auto & pass = graph.createPass( "Blur" + c3d::string::toMbString( index ) + ( isVertical ? "Y" : "X" )
				, [this, &device, dimensions, index, enabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto result = crg::RenderQuadBuilder{}
							.renderPosition( {} )
							.renderSize( { dimensions.width >> ( index + 1 )
								, dimensions.height >> ( index + 1 ) } )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
							.enabled( enabled )
							.build( framePass, context, graph );
						c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
							, result->getTimer() );
						return result;
				} );
			m_blurUbos[index].createPassBinding( pass, blur::Bindings::GaussCfgUboIdx );
			pass.addInputSampledT( *srcImage.getSampledLastAttach( 0u, index ), blur::Bindings::DifImgIdx
				, crg::SamplerDesc{ c3d::FilterMode::eNearest, c3d::FilterMode::eNearest, c3d::MipmapMode::eNearest
					, c3d::WrapMode::eClampToEdge, c3d::WrapMode::eClampToEdge, c3d::WrapMode::eClampToEdge
					, 0.0f, float( index ), float( index + 1u ) } );
			attachs.push_back( m_result.setLastAttach( 0u, index, pass.addOutputColourTarget( m_result.getTargetViewId( 0u, index ) ) ) );
		}

		m_result.setLastAttach( graph.mergeAttachments( attachs ) );
	}

	void BlurPass::update( uint32_t kernelSize )
	{
		auto kernel = blur::doCreateKernel( kernelSize );

		for ( auto & ubo : m_blurUbos )
		{
			auto data = ubo.getData();
			data.blurCoeffsCount = kernelSize;
			data.blurCoeffs = kernel;
			ubo.setData( c3d::move( data ) );
		}
	}

	void BlurPass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}
}

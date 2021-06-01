#include "Castor3D/Render/Passes/GaussianBlur.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

CU_ImplementCUSmartPtr( castor3d, GaussianBlur )

using namespace castor;

namespace castor3d
{
	namespace
	{
		enum Idx
		{
			GaussCfgIdx,
			DifImgIdx,
		};

		ShaderPtr getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurXProgram( bool isDepth )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, GaussCfgIdx, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto base = writer.declLocale( "base", vec2( 1.0_f, 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = c3d_mapSource.sample( vtx_texture ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture + offset );
					}
					ROF;

					if ( isDepth )
					{
						out.fragDepth = pxl_fragColor.r();
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurXProgramLayer( bool isDepth
			, uint32_t layer )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, GaussCfgIdx, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DArrayRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto base = writer.declLocale( "base", vec2( 1.0_f, 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = c3d_mapSource.sample( vec3( vtx_texture, Float( float( layer ) ) ) ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vec3( vtx_texture - offset, Float( float( layer ) ) ) );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vec3( vtx_texture + offset, Float( float( layer ) ) ) );
					}
					ROF;

					if ( isDepth )
					{
						out.fragDepth = pxl_fragColor.r();
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurYProgram( bool isDepth )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, GaussCfgIdx, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto base = writer.declLocale( "base", vec2( 0.0_f, 1.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = c3d_mapSource.sample( vtx_texture ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture + offset );
					}
					ROF;

					if ( isDepth )
					{
						out.fragDepth = pxl_fragColor.r();
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram( RenderSystem & renderSystem
			, uint32_t baseArrayLayer
			, uint32_t layerCount
			, bool isHorizontal
			, bool isDepth )
		{
			if ( isHorizontal )
			{
				if ( layerCount > 1u
					&& !renderSystem.getDescription().features.hasImageTexture )
				{
					return getBlurXProgramLayer( isDepth
						, baseArrayLayer );
				}

				return getBlurXProgram( isDepth );
			}

			return getBlurYProgram( isDepth );
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

		VkImageViewType getNonArrayType( VkImageViewType in )
		{
			switch ( in )
			{
			case VK_IMAGE_VIEW_TYPE_CUBE:
				return VK_IMAGE_VIEW_TYPE_2D;
			case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
				return VK_IMAGE_VIEW_TYPE_1D;
			case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
				return VK_IMAGE_VIEW_TYPE_2D;
			case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
				return VK_IMAGE_VIEW_TYPE_2D;
			default:
				return in;
			}
		}

		crg::ImageViewData createImageView( castor::String const & name
			, crg::ImageViewId const & view
			, VkImageSubresourceRange const & range
			, VkImageAspectFlags aspectMask )
		{
			auto result = *view.data;
			result.info.subresourceRange.aspectMask = aspectMask;
			result.info.subresourceRange.baseMipLevel = range.baseMipLevel;
			result.info.subresourceRange.levelCount = range.levelCount;
			return result;
		}

		crg::ImageViewData createImageView( castor::String const & name
			, crg::ImageViewId const & view
			, VkImageSubresourceRange const & range )
		{
			return createImageView( name
				, view
				, range
				, view.data->info.subresourceRange.aspectMask );
		}

		crg::ImageViewData createSampledView( castor::String const & prefix
			, crg::ImageViewId const & view )
		{
			return createImageView( prefix + "Sampled"
				, view
				, view.data->info.subresourceRange
				, VkImageAspectFlags( ( ashes::isDepthFormat( getFormat( view ) ) || ashes::isDepthOrStencilFormat( getFormat( view ) ) )
					? VK_IMAGE_ASPECT_DEPTH_BIT
					: ( ashes::isStencilFormat( getFormat( view ) )
						? VK_IMAGE_ASPECT_STENCIL_BIT
						: VK_IMAGE_ASPECT_COLOR_BIT ) ) );
		}

		crg::ImageViewData createTargetView( castor::String const & prefix
			, crg::ImageViewId const & view )
		{
			return createImageView( prefix + "Target"
				, view
				, view.data->info.subresourceRange
				, VkImageAspectFlags( ( ashes::isDepthFormat( getFormat( view ) ) || ashes::isDepthOrStencilFormat( getFormat( view ) ) )
					? VK_IMAGE_ASPECT_DEPTH_BIT
					: ( ashes::isStencilFormat( getFormat( view ) )
						? VK_IMAGE_ASPECT_STENCIL_BIT
						: VK_IMAGE_ASPECT_COLOR_BIT ) ) );
		}

		crg::ImageViewData createMipView( castor::String const & name
			, crg::ImageViewId const & source
			, uint32_t layer
			, uint32_t level )
		{
			auto result = *source.data;
			result.info.subresourceRange.baseArrayLayer = layer;
			result.info.subresourceRange.baseMipLevel = level;
			result.info.subresourceRange.layerCount = 1u;
			result.info.subresourceRange.levelCount = 1u;

			if ( result.info.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY )
			{
				result.info.viewType = VK_IMAGE_VIEW_TYPE_1D;
			}
			else if ( result.info.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
				|| result.info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
				|| result.info.viewType == VK_IMAGE_VIEW_TYPE_3D )
			{
				result.info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			}

			result.name = name;
			result.name += "L" + string::toString( layer );
			result.name += "M" + string::toString( level );
			return result;
		}

		uint32_t getDescriptorSetIndex( uint32_t descriptorBaseIndex
			, uint32_t level
			, uint32_t levelCount )
		{
			return ( descriptorBaseIndex * levelCount ) + level;
		}

		rq::BindingDescriptionArray createBindings()
		{
			return
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, std::nullopt },
			};
		}
	}

	//*********************************************************************************************

	GaussianBlur::BlurPass::BlurPass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, castor::String const & name
		, crg::ImageViewId const & input
		, crg::ImageViewId const & output
		, UniformBufferOffsetT< GaussianBlur::Configuration > const & blurUbo
		, VkFormat format
		, VkExtent2D const & textureSize
		, bool isHorizontal )
		: vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, name, getVertexProgram() }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, name
			, getPixelProgram( device.renderSystem
				, input.data->info.subresourceRange.baseArrayLayer
				, input.data->info.subresourceRange.layerCount
				, isHorizontal
				, ashes::isDepthFormat( format ) ),
		}
		, stages{ makeShaderState( device, vertexShader )
			, makeShaderState( device, pixelShader ) }
		, isHorizontal{ isHorizontal }
	{
		auto subresourceRange = input.data->info.subresourceRange;

		for ( auto layer = subresourceRange.baseArrayLayer; layer < subresourceRange.layerCount; ++layer )
		{
			for ( auto level = subresourceRange.baseMipLevel; level < subresourceRange.levelCount; ++level )
			{
				auto & pass = graph.createPass( name + "L" + std::to_string( layer ) + "M" + std::to_string( level )
					, [this, &input]( crg::FramePass const & pass
						, crg::GraphContext const & context
						, crg::RunnableGraph & graph )
					{
						auto extent = getExtent( input );
						return crg::RenderQuadBuilder{}
							.renderPosition( {} )
							.renderSize( { extent.width, extent.height } )
							.texcoordConfig( {} )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) )
							.build( pass, context, graph );
					} );
				pass.addDependency( *previousPass );
				previousPass = &pass;
				blurUbo.createPassBinding( pass, GaussCfgIdx );
				pass.addSampledView( graph.createView( createMipView( name, input, layer, level ) )
					, DifImgIdx );
				pass.addOutputColourView( output );
			}
		}
	}

	//*********************************************************************************************
	
	String const GaussianBlur::Config = cuT( "Config" );
	String const GaussianBlur::Coefficients = cuT( "c3d_coefficients" );
	String const GaussianBlur::CoefficientsCount = cuT( "c3d_coefficientsCount" );
	String const GaussianBlur::TextureSize = cuT( "c3d_textureSize" );

	GaussianBlur::GaussianBlur( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & prefix
		, crg::ImageViewId const & view
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_previousPass{ &previousPass }
		, m_prefix{ prefix }
		, m_source{ view }
		, m_size{ getExtent( m_source ).width, getExtent( m_source ).height }
		, m_format{ getFormat( m_source ) }
		, m_intermediate{ graph.createImage( crg::ImageData{ prefix + "GB"
			, 0u
			, VK_IMAGE_TYPE_2D
			, m_format
			, { m_size.width, m_size.height, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| ( getMipLevels( m_source ) > 1u
					? ( VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
					: 0u ) )
			, getMipLevels( m_source ) } ) }
		, m_intermediateView{ graph.createView( crg::ImageViewData{ prefix + "GB"
			, m_intermediate
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_format
			, { ashes::getAspectMask( m_format ), 0u, getMipLevels( m_source ), 0u, 1u } } ) }
		, m_blurUbo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
		, m_blurX{ graph
			, m_previousPass
			, m_device
			, m_prefix + cuT( "GBX" )
			, m_source
			, m_intermediateView
			, m_blurUbo
			, m_format
			, m_size
			, true }
		, m_blurY{ graph
			, m_previousPass
			, m_device
			, m_prefix + cuT( "GBY" )
			, m_intermediateView
			, m_source
			, m_blurUbo
			, m_format
			, m_size
			, false }
		, m_kernel{ getHalfPascal( kernelSize ) }
	{
		CU_Require( kernelSize < MaxCoefficients );
		auto & data = m_blurUbo.getData();
		data.blurCoeffsCount = uint32_t( m_kernel.size() );
		data.dump = 0u;
		std::memcpy( data.blurCoeffs.data()->ptr()
			, m_kernel.data()
			, sizeof( float ) * std::min( size_t( MaxCoefficients ), m_kernel.size() ) );
		data.textureSize[0] = float( m_size.width );
		data.textureSize[1] = float( m_size.height );
	}

	GaussianBlur::~GaussianBlur()
	{
	}

	void GaussianBlur::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_prefix + " GaussianBlur Intermediate"
			, m_intermediateView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		visitor.visit( m_blurX.vertexShader );
		visitor.visit( m_blurX.pixelShader );

		visitor.visit( m_blurY.vertexShader );
		visitor.visit( m_blurY.pixelShader );
	}
}

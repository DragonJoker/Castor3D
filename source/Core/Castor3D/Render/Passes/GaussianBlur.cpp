#include "Castor3D/Render/Passes/GaussianBlur.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
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

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
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
			auto c3d_mapSource = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
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
			auto c3d_mapSource = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
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

		crg::ImageViewData createMipView( crg::ImageViewId const & source
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

			result.name = source.data->name;
			result.name += "L" + string::toString( layer );
			result.name += "M" + string::toString( level );
			return result;
		}

		crg::ImageViewId createIntermediate( crg::FramePassGroup & graph
			, castor::String const & prefix
			, VkFormat format
			, VkExtent3D const & size
			, uint32_t mipLevels )
		{
			auto intermediate = graph.createImage( crg::ImageData{ prefix + "GB"
				, 0u
				, VK_IMAGE_TYPE_2D
				, format
				, { size.width, size.height, 1u }
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| ( mipLevels > 1u
						? ( VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
						: 0u ) )
				, mipLevels } );
			return graph.createView( crg::ImageViewData{ prefix + "GB"
				, intermediate
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, format
				, { ashes::getAspectMask( format ), 0u, mipLevels, 0u, 1u } } );
		}

		crg::ImageViewIdArray createViews( crg::FramePassGroup & graph
			, crg::ImageViewId input )
		{
			crg::ImageViewIdArray result;

			auto subresourceRange = input.data->info.subresourceRange;

			for ( auto layerIdx = 0u; layerIdx < subresourceRange.layerCount; ++layerIdx )
			{
				auto layer = subresourceRange.baseArrayLayer + layerIdx;

				for ( auto levelIdx = subresourceRange.baseMipLevel; levelIdx < subresourceRange.levelCount; ++levelIdx )
				{
					auto level = subresourceRange.baseMipLevel + levelIdx;
					result.push_back( graph.createView( createMipView( input, layer, level ) ) );
				}
			}

			return result;
		}
	}

	//*********************************************************************************************
	
	String const GaussianBlur::Config = cuT( "Config" );
	String const GaussianBlur::Coefficients = cuT( "c3d_coefficients" );
	String const GaussianBlur::CoefficientsCount = cuT( "c3d_coefficientsCount" );
	String const GaussianBlur::TextureSize = cuT( "c3d_textureSize" );

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & prefix
		, crg::ImageViewIdArray const & views
		, crg::ImageViewId const & intermediateView
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_sources{ views }
		, m_device{ device }
		, m_lastPass{ &previousPass }
		, m_prefix{ prefix }
		, m_size{ makeExtent2D( getExtent( m_sources[0] ) ) }
		, m_format{ getFormat( m_sources[0] ) }
		, m_intermediateView{ intermediateView }
		, m_blurUbo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
		, m_kernel{ getHalfPascal( kernelSize ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, m_prefix + cuT( "GB" ), getVertexProgram() }
		, m_pixelShaderX{ VK_SHADER_STAGE_FRAGMENT_BIT, m_prefix + cuT( "GBX" ), getBlurXProgram( ashes::isDepthFormat( m_format ) ) }
		, m_pixelShaderY{ VK_SHADER_STAGE_FRAGMENT_BIT, m_prefix + cuT( "GBY" ), getBlurYProgram( ashes::isDepthFormat( m_format ) ) }
		, m_stagesX{ makeShaderState( device, m_vertexShader ), makeShaderState( device, m_pixelShaderX ) }
		, m_stagesY{ makeShaderState( device, m_vertexShader ), makeShaderState( device, m_pixelShaderY ) }
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

		for ( auto & input : m_sources )
		{
			{
				auto name = input.data->name + "BlurX";
				auto & passX = graph.createPass( name
					, [this, &input]( crg::FramePass const & framePass
						, crg::GraphContext & context
						, crg::RunnableGraph & graph )
					{
						auto extent = getExtent( input );
						auto result = crg::RenderQuadBuilder{}
							.renderPosition( {} )
							.renderSize( { extent.width, extent.height } )
							.texcoordConfig( {} )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stagesX ) )
							.build( framePass, context, graph );
						m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, result->getTimer() );
						return result;
					} );
				passX.addDependency( *m_lastPass );
				m_lastPass = &passX;
				m_blurUbo.createPassBinding( passX, "BlurCfgX", GaussCfgIdx );
				passX.addSampledView( input, DifImgIdx, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
				passX.addOutputColourView( m_intermediateView );
			}
			{
				auto name = input.data->name + "BlurY";
				auto & passY = graph.createPass( name
					, [this, &input]( crg::FramePass const & framePass
						, crg::GraphContext & context
						, crg::RunnableGraph & graph )
					{
						auto extent = getExtent( input );
						auto result = crg::RenderQuadBuilder{}
							.renderPosition( {} )
							.renderSize( { extent.width, extent.height } )
							.texcoordConfig( {} )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stagesY ) )
							.build( framePass, context, graph );
						m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, result->getTimer() );
						return result;
					} );
				passY.addDependency( *m_lastPass );
				m_lastPass = &passY;
				m_blurUbo.createPassBinding( passY, "BlurCfgY", GaussCfgIdx );
				passY.addSampledView( m_intermediateView, DifImgIdx, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
				passY.addOutputColourView( input
					, {}
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			}
		}
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & prefix
		, crg::ImageViewIdArray const & views
		, uint32_t kernelSize )
		: GaussianBlur{ graph
			, previousPass
			, device
			, category
			, prefix
			, views
			, createIntermediate( graph, prefix, getFormat( views[0] ), getExtent( views[0] ), getMipLevels( views[0] ) )
			, kernelSize }
	{
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & prefix
		, crg::ImageViewId const & view
		, uint32_t kernelSize )
		: GaussianBlur{ graph
			, previousPass
			, device
			, category
			, prefix
			, createViews( graph, view )
			, createIntermediate( graph, prefix, getFormat( view ), getExtent( view ), getMipLevels( view ) )
			, kernelSize }
	{
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & prefix
		, crg::ImageViewId const & view
		, crg::ImageViewId const & intermediateView
		, uint32_t kernelSize )
		: GaussianBlur{ graph
			, previousPass
			, device
			, category
			, prefix
			, createViews( graph, view )
			, intermediateView
			, kernelSize }
	{
	}

	void GaussianBlur::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_prefix + " GaussianBlur Intermediate"
			, m_intermediateView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShaderX );

		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShaderY );
	}
}

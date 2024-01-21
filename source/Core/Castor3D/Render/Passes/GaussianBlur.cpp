#include "Castor3D/Render/Passes/GaussianBlur.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

CU_ImplementSmartPtr( castor3d, GaussianBlur )

#pragma GCC diagnostic ignored "-Wrestrict"

namespace castor3d
{
	namespace passgauss
	{
		enum Idx
		{
			GaussCfgIdx,
			DifImgIdx,
		};

		template< sdw::var::Flag FlagT >
		using TexcoordStructT = sdw::IOStructInstanceHelperT< FlagT
			, "Texcoord"
			, sdw::IOStructFieldT< sdw::Vec2, "texcoord", 0u > >;

		template< sdw::var::Flag FlagT >
		struct TexcoordT
			: public TexcoordStructT< FlagT >
		{
			TexcoordT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: TexcoordStructT< FlagT >{ writer, std::move( expr ), enabled }
			{
			}

			auto texcoord()const{ return this->template getMember< "texcoord" >(); }
		};

		static ShaderPtr getProgram( Engine & engine
			, bool isDepth
			, bool isVertical )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			auto config = writer.declUniformBuffer( GaussianBlur::Config, GaussCfgIdx, 0u );
			auto c3d_textureSize = config.declMember< sdw::Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< sdw::UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< sdw::UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< sdw::Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );

			// Shader inputs
			auto position = writer.declInput< sdw::Vec2 >( "position", sdw::EntryPoint::eVertex, 0u );
			auto uv = writer.declInput< sdw::Vec2 >( "uv", sdw::EntryPoint::eVertex, 1u );

			// Shader outputs
			auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", sdw::EntryPoint::eFragment, 0u );

			writer.implementEntryPointT< sdw::VoidT, TexcoordT >( [&]( sdw::VertexIn const & in
				, sdw::VertexOutT< TexcoordT > out )
				{
					out.texcoord() = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< TexcoordT, sdw::VoidT >( [&]( sdw::FragmentInT< TexcoordT > const & in
				, sdw::FragmentOut out )
				{
					auto base = writer.declLocale( "base", vec2( isVertical ? 0.0_f : 1.0_f, isVertical ? 1.0_f : 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					outColour = c3d_mapSource.sample( in.texcoord() ) * c3d_coefficients[0_u][0_u];

					FOR( writer, sdw::UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						outColour += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.texcoord() - offset );
						outColour += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.texcoord() + offset );
					}
					ROF

					if ( isDepth )
					{
						out.fragDepth = outColour.r();
					}
				} );
			return writer.getBuilder().releaseShader();
		}

		static std::vector< float > getHalfPascal( uint32_t height )
		{
			std::vector< float > result;
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

		static crg::ImageViewData createMipView( crg::ImageViewId const & source
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
			result.name += "L" + castor::string::toString( layer );
			result.name += "M" + castor::string::toString( level );
			return result;
		}

		static crg::ImageViewId createIntermediate( crg::FramePassGroup const & graph
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

		static crg::ImageViewIdArray createViews( crg::FramePassGroup const & graph
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
	
	castor::String const GaussianBlur::Config = cuT( "Config" );
	castor::String const GaussianBlur::Coefficients = cuT( "c3d_coefficients" );
	castor::String const GaussianBlur::CoefficientsCount = cuT( "c3d_coefficientsCount" );
	castor::String const GaussianBlur::TextureSize = cuT( "c3d_textureSize" );

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & prefix
		, crg::ImageViewIdArray const & views
		, crg::ImageViewId const & intermediateView
		, uint32_t kernelSize
		, crg::RunnablePass::IsEnabledCallback const & isEnabled )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_sources{ views }
		, m_device{ device }
		, m_lastPass{ &previousPass }
		, m_prefix{ prefix }
		, m_size{ makeExtent2D( getExtent( m_sources[0] ) ) }
		, m_format{ getFormat( m_sources[0] ) }
		, m_intermediateView{ intermediateView }
		, m_blurUbo{ m_device.uboPool->getBuffer< Configuration >( 0u ) }
		, m_kernel{ passgauss::getHalfPascal( kernelSize ) }
		, m_shaderX{ m_prefix + cuT( "GBX" ), passgauss::getProgram( *device.renderSystem.getEngine(), ashes::isDepthFormat( m_format ), false ) }
		, m_shaderY{ m_prefix + cuT( "GBY" ), passgauss::getProgram( *device.renderSystem.getEngine(), ashes::isDepthFormat( m_format ), true ) }
		, m_stagesX{ makeProgramStates( device, m_shaderX ) }
		, m_stagesY{ makeProgramStates( device, m_shaderY ) }
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

		for ( auto const & input : m_sources )
		{
			{
				auto name = input.data->name + "BlurX";
				auto & passX = graph.createPass( name
					, [this, &input, isEnabled]( crg::FramePass const & framePass
						, crg::GraphContext & context
						, crg::RunnableGraph & graph )
					{
						auto extent = getExtent( input );
						auto result = crg::RenderQuadBuilder{}
							.renderPosition( {} )
							.renderSize( { extent.width, extent.height } )
							.texcoordConfig( {} )
							.isEnabled( isEnabled )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stagesX ) )
							.build( framePass, context, graph );
						m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, result->getTimer() );
						return result;
					} );
				passX.addDependency( *m_lastPass );
				m_lastPass = &passX;
				m_blurUbo.createPassBinding( passX, "BlurCfgX", passgauss::GaussCfgIdx );
				passX.addSampledView( input, passgauss::DifImgIdx );
				passX.addOutputColourView( m_intermediateView );
			}
			{
				auto name = input.data->name + "BlurY";
				auto & passY = graph.createPass( name
					, [this, &input, isEnabled]( crg::FramePass const & framePass
						, crg::GraphContext & context
						, crg::RunnableGraph & graph )
					{
						auto extent = getExtent( input );
						auto result = crg::RenderQuadBuilder{}
							.renderPosition( {} )
							.renderSize( { extent.width, extent.height } )
							.texcoordConfig( {} )
							.isEnabled( isEnabled )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stagesY ) )
							.build( framePass, context, graph );
						m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, result->getTimer() );
						return result;
					} );
				passY.addDependency( *m_lastPass );
				m_lastPass = &passY;
				m_blurUbo.createPassBinding( passY, "BlurCfgY", passgauss::GaussCfgIdx );
				passY.addSampledView( m_intermediateView, passgauss::DifImgIdx );
				passY.addOutputColourView( input );
			}
		}
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & prefix
		, crg::ImageViewIdArray const & views
		, uint32_t kernelSize
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: GaussianBlur{ graph
			, previousPass
			, device
			, prefix
			, views
			, passgauss::createIntermediate( graph, prefix, getFormat( views[0] ), getExtent( views[0] ), getMipLevels( views[0] ) )
			, kernelSize
			, isEnabled }
	{
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & prefix
		, crg::ImageViewId const & view
		, uint32_t kernelSize
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: GaussianBlur{ graph
			, previousPass
			, device
			, prefix
			, passgauss::createViews( graph, view )
			, passgauss::createIntermediate( graph, prefix, getFormat( view ), getExtent( view ), getMipLevels( view ) )
			, kernelSize
			, isEnabled }
	{
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & prefix
		, crg::ImageViewId const & view
		, crg::ImageViewId const & intermediateView
		, uint32_t kernelSize
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: GaussianBlur{ graph
			, previousPass
			, device
			, prefix
			, passgauss::createViews( graph, view )
			, intermediateView
			, kernelSize
			, isEnabled }
	{
	}

	void GaussianBlur::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_prefix + " GaussianBlur Intermediate"
			, m_intermediateView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

		visitor.visit( m_shaderX );
		visitor.visit( m_shaderY );
	}
}

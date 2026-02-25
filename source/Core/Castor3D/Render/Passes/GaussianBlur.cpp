#include "Castor3D/Render/Passes/GaussianBlur.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"

#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

CU_ImplementSmartPtr( c3d, GaussianBlur )

#pragma GCC diagnostic ignored "-Wrestrict"

namespace c3d
{
	namespace passgauss
	{
		enum class Bindings
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
				: TexcoordStructT< FlagT >{ writer, c3d::move( expr ), enabled }
			{
			}

			auto texcoord()const{ return this->template getMember< "texcoord" >(); }
		};

		static ShaderPtr getProgram( Engine & engine
			, bool isDepth
			, bool isVertical )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			auto config = writer.declUniformBuffer( GaussianBlur::Config, Bindings::GaussCfgIdx, 0u );
			auto c3d_textureSize = config.declMember< sdw::Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< sdw::UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< sdw::UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< sdw::Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapSource", Bindings::DifImgIdx, 0u );

			writer.implementEntryPointT< shader::PosUv2FT, TexcoordT >( []( sdw::VertexInT< shader::PosUv2FT > const & in
				, sdw::VertexOutT< TexcoordT > out )
				{
					out.texcoord() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< TexcoordT, shader::Colour4FT >( [&writer, &c3d_textureSize, &c3d_coefficients, &c3d_coefficientsCount, &c3d_mapSource, isVertical, isDepth]( sdw::FragmentInT< TexcoordT > const & in
				, sdw::FragmentOutT< shader::Colour4FT > out )
				{
					auto base = writer.declLocale( "base", vec2( isVertical ? 0.0_f : 1.0_f, isVertical ? 1.0_f : 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					out.colour() = c3d_mapSource.sample( in.texcoord() ) * c3d_coefficients[0_u][0_u];

					sdwFOR( writer, sdw::UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						out.colour() += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.texcoord() - offset );
						out.colour() += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( in.texcoord() + offset );
					}
					sdwROF

					if ( isDepth )
					{
						out.fragDepth = out.colour().r();
					}
				} );
			return writer.getBuilder().releaseShader();
		}

		static Vector< float > getHalfPascal( uint32_t height )
		{
			Vector< float > result;
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

		static crg::ImageViewId createIntermediate( crg::FramePassGroup const & graph
			, String const & prefix
			, PixelFormat format
			, Extent3D const & size
			, uint32_t mipLevels )
		{
			auto mbPrefix = toUtf8( prefix );
			auto intermediate = graph.createImage( crg::ImageData{ mbPrefix + "GB"
				, ImageCreateFlags::eNone
				, ImageType::e2D
				, format
				, { size.width, size.height, 1u }
				, ( ImageUsageFlags::eColorAttachment
					| ImageUsageFlags::eSampled
					| ( mipLevels > 1u
						? ( ImageUsageFlags::eTransferDst | ImageUsageFlags::eTransferSrc )
						: ImageUsageFlags::eNone ) )
				, mipLevels } );
			return graph.createView( crg::ImageViewData{ mbPrefix + "GB"
				, intermediate
				, ImageViewCreateFlags::eNone
				, ImageViewType::e2D
				, format
				, { getAspectMask( format ), 0u, mipLevels, 0u, 1u } } );
		}
	}

	//*********************************************************************************************
	
	MbString const GaussianBlur::Config = "Config";
	MbString const GaussianBlur::Coefficients = "c3d_coefficients";
	MbString const GaussianBlur::CoefficientsCount = "c3d_coefficientsCount";
	MbString const GaussianBlur::TextureSize = "c3d_textureSize";

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & prefix
		, crg::Attachment const & attach
		, crg::ImageViewId const & intermediateView
		, uint32_t kernelSize
		, crg::RunnablePass::IsEnabledCallback const & isEnabled )
		: OwnedBy< Engine >{ c3d::getEngine( device ) }
		, m_source{ attach }
		, m_device{ device }
		, m_prefix{ prefix }
		, m_size{ makeExtent2D( getExtent( m_source.view() ) ) }
		, m_format{ getFormat( m_source.view() ) }
		, m_intermediateView{ intermediateView }
		, m_blurUbo{ m_device }
		, m_kernel{ passgauss::getHalfPascal( kernelSize ) }
		, m_shaderX{ m_prefix + cuT( "GBX" ), passgauss::getProgram( c3d::getEngine( device ), isDepthFormat( m_format ), false ) }
		, m_shaderY{ m_prefix + cuT( "GBY" ), passgauss::getProgram( c3d::getEngine( device ), isDepthFormat( m_format ), true ) }
		, m_stagesX{ makeProgramStates( device, m_shaderX ) }
		, m_stagesY{ makeProgramStates( device, m_shaderY ) }
	{
		CU_Require( kernelSize < MaxCoefficients );
		Configuration data{};
		data.blurCoeffsCount = uint32_t( m_kernel.size() );
		data.dump = 0u;
		std::memcpy( data.blurCoeffs.data()->ptr()
			, m_kernel.data()
			, sizeof( float ) * std::min( size_t( MaxCoefficients ), m_kernel.size() ) );
		data.textureSize[0] = float( m_size.width );
		data.textureSize[1] = float( m_size.height );
		m_blurUbo.setData( c3d::move( data ) );
		{
			auto name = m_source.view( 0 ).data->name + "BlurX";
			auto & passX = graph.createPass( name
				, [this, isEnabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnable )
				{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( { m_size.width, m_size.height } )
					.texcoordConfig( {} )
					.isEnabled( isEnabled )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stagesX ) )
					.build( framePass, context, runnable );
				c3d::getEngine( m_device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
				} );
			m_blurUbo.createPassBinding( passX, passgauss::Bindings::GaussCfgIdx );
			passX.addInputSampledT( m_source, passgauss::Bindings::DifImgIdx );
			m_lastAttach = passX.addOutputColourTarget( m_intermediateView );
		}
		{
			auto name = m_source.view( 0 ).data->name + "BlurY";
			auto & passY = graph.createPass( name
				, [this, isEnabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnable )
				{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( { m_size.width, m_size.height } )
					.texcoordConfig( {} )
					.isEnabled( isEnabled )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stagesY ) )
					.build( framePass, context, runnable );
				c3d::getEngine( m_device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
				} );
			m_blurUbo.createPassBinding( passY, passgauss::Bindings::GaussCfgIdx );
			passY.addInputSampledT( *m_lastAttach, passgauss::Bindings::DifImgIdx );
			m_lastAttach = passY.addOutputColourTarget( m_source.view() );
		}
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & prefix
		, crg::Attachment const & attach
		, uint32_t kernelSize
		, crg::RunnablePass::IsEnabledCallback const & isEnabled )
		: GaussianBlur{ graph
			, device
			, prefix
			, attach
			, passgauss::createIntermediate( graph, prefix, getFormat( attach.view() ), getExtent( attach.view() ), getMipLevels( attach.view() ) )
			, kernelSize
			, isEnabled }
	{
	}

	GaussianBlur::GaussianBlur( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & prefix
		, c3d::Texture & texture
		, uint32_t kernelSize
		, crg::RunnablePass::IsEnabledCallback const & isEnabled )
		: GaussianBlur{ graph
			, device
			, prefix
			, *texture.getLastAttach()
			, passgauss::createIntermediate( graph, prefix, texture.getFormat(), texture.getExtent(), texture.getMipLevels() )
			, kernelSize
			, isEnabled }
	{
		texture.setLastAttach( &getResultAttach() );
	}

	void GaussianBlur::accept( ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_prefix + cuT( " GaussianBlur Intermediate" )
			, m_intermediateView
			, ImageLayout::eShaderReadOnly
			, TextureFactors{}.invert( true ) );

		visitor.visit( m_shaderX );
		visitor.visit( m_shaderY );
	}
}

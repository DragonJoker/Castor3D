#include "FilmGrainPostEffect/FilmGrainPostEffect.hpp"

#include "NoiseLayer1.xpm"
#include "NoiseLayer2.xpm"
#include "NoiseLayer3.xpm"
#include "NoiseLayer4.xpm"
#include "NoiseLayer5.xpm"
#include "NoiseLayer6.xpm"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace film_grain
{
	namespace postfx
	{
		namespace c3ds = c3d::shader;

		static c3d::MbString const FilmGrainUbo = "FilmGrainUbo";
		static c3d::MbString const PixelSize = "c3d_pixelSize";
		static c3d::MbString const NoiseIntensity = "c3d_noiseIntensity";
		static c3d::MbString const Exposure = "c3d_exposure";
		static c3d::MbString const Time = "c3d_time";
		static c3d::MbString const SrcTex = "c3d_srcTex";
		static c3d::MbString const NoiseTex = "c3d_noiseTex";

		enum Idx : uint32_t
		{
			FilmCfgUboIdx,
			NoiseTexIdx,
			SourceTexIdx,
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			auto filmGrain = writer.declUniformBuffer( FilmGrainUbo, FilmCfgUboIdx, 0u );
			auto c3d_pixelSize = filmGrain.declMember< sdw::Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< sdw::Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< sdw::Float >( Exposure );
			auto c3d_time = filmGrain.declMember< sdw::Float >( Time );
			filmGrain.end();

			auto c3d_noiseTex = writer.declCombinedImg< FImg3DR32 >( NoiseTex, NoiseTexIdx, 0u );
			auto c3d_srcTex = writer.declCombinedImg< FImg2DRgba32 >( SrcTex, SourceTexIdx, 0u );
			
			auto overlay = writer.implementFunction< sdw::Vec3 >( "overlay"
				, [&writer]( sdw::Vec3 const & a
					, sdw::Vec3 const & b )
				{
					auto comp = writer.declLocale( "comp"
						, pow( abs( b ), vec3( 2.2_f ) ) );

					sdwIF( writer, comp.x() < 0.5_f && comp.y() < 0.5_f && comp.z() < 0.5_f )
					{
						writer.returnStmt( 2.0_f * a * b );
					}
					sdwFI

					writer.returnStmt( vec3( 1.0_f ) - 2.0_f * ( 1.0_f - a ) * ( 1.0_f - b ) );
				}
				, sdw::InVec3{ writer, "a" }
				, sdw::InVec3{ writer, "b" } );

			auto addNoise = writer.implementFunction< sdw::Vec3 >( "addNoise"
				, [&writer, &c3d_pixelSize, &c3d_time, &c3d_noiseTex, &c3d_exposure, &c3d_noiseIntensity, &overlay]( sdw::Vec3 const & color
					, sdw::Vec2 const & texcoord )
				{
					auto coord = writer.declLocale( "coord"
						, texcoord * 2.0_f );
					coord.x() *= c3d_pixelSize.y() / c3d_pixelSize.x();
					auto noise = writer.declLocale( "noise"
						, c3d_noiseTex.sample( vec3( coord, c3d_time ) ) );
					auto exposureFactor = writer.declLocale( "exposureFactor"
						, c3d_exposure / 2.0_f );
					exposureFactor = sqrt( exposureFactor );
					auto t = writer.declLocale( "t"
						, mix( 3.5_f * c3d_noiseIntensity
							, 1.13_f * c3d_noiseIntensity
							, exposureFactor ) );
					writer.returnStmt( overlay( color
						, vec3( mix( 0.5_f, noise, t ) ) ) );
				}
				, sdw::InVec3{ writer, "color" }
				, sdw::InVec2{ writer, "texcoord" } );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::PosUv2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position().xy(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_srcTex, &addNoise]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					auto colour = writer.declLocale( "colour"
						, c3d_srcTex.sample( in.uv() ).xyz() );
					colour = addNoise( colour, in.uv() );
					out.colour() = vec4( colour, 1.0 );
				} );
			return writer.getBuilder().releaseShader();
		}

		static c3d::Array< c3d::Image, PostEffect::NoiseMapCount > loadImages( c3d::Engine const & engine )
		{
			auto & loader = engine.getImageLoader();
			return { loader.load( cuT( "FilmGrainNoise0" ), cuT( "xpm" ), c3d::ByteCPtr( NoiseLayer1_xpm ), uint32_t( c3d::getCountOf( NoiseLayer1_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise1" ), cuT( "xpm" ), c3d::ByteCPtr( NoiseLayer2_xpm ), uint32_t( c3d::getCountOf( NoiseLayer2_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise2" ), cuT( "xpm" ), c3d::ByteCPtr( NoiseLayer3_xpm ), uint32_t( c3d::getCountOf( NoiseLayer3_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise3" ), cuT( "xpm" ), c3d::ByteCPtr( NoiseLayer4_xpm ), uint32_t( c3d::getCountOf( NoiseLayer4_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise4" ), cuT( "xpm" ), c3d::ByteCPtr( NoiseLayer5_xpm ), uint32_t( c3d::getCountOf( NoiseLayer5_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise5" ), cuT( "xpm" ), c3d::ByteCPtr( NoiseLayer6_xpm ), uint32_t( c3d::getCountOf( NoiseLayer6_xpm ) ), {} ) };
		}
	}

	//*********************************************************************************************

	c3d::String PostEffect::Type = cuT( "film_grain" );
	c3d::MbString PostEffect::Name = "FilmGrain PostEffect";

	PostEffect::PostEffect( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
		: c3d::PostEffect{ PostEffect::Type
			, cuT( "FilmGrain" )
			, c3d::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, params }
		, m_shader{ cuT( "FilmGrain" ), postfx::getProgram( *renderTarget.getEngine() ) }
		, m_stages{ makeProgramStates( renderSystem.getRenderDevice(), m_shader ) }
		, m_configUbo{ renderSystem.getRenderDevice().uboPool->getBuffer< Configuration >( 0u ) }
		, m_noiseImages{ postfx::loadImages( *renderTarget.getEngine() ) }
	{
		m_config.pixelSize = c3d::Point2f{ m_renderTarget.getSize().getWidth()
			, m_renderTarget.getSize().getHeight() };
		m_config.noiseIntensity = 1.0f;
		m_config.exposure = 1.0f;
		m_config.time = 0.0f;
		PostEffect::setParameters( params );

		for ( auto & image : m_noiseImages )
		{
			auto format = image.getPixelFormat();

			if ( format == c3d::PixelFormat::eR8G8B8_UNORM )
			{
				auto buffer = c3d::PxBufferBase::create( image.getDimensions()
					, c3d::PixelFormat::eR8G8B8A8_UNORM
					, image.getPxBuffer().getConstPtr()
					, image.getPxBuffer().getFormat()
					, image.getPxBuffer().getAlign() );
				image = c3d::Image{ image.getName()
					, image.getPath()
					, *buffer };
			}
		}

		auto & data = m_configUbo.getData();
		data = m_config;
	}

	PostEffect::~PostEffect()
	{
		getRenderSystem()->getRenderDevice().uboPool->putBuffer( m_configUbo );
	}

	c3d::PostEffectUPtr PostEffect::create( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
	{
		return c3d::makeUniqueDerived< c3d::PostEffect, PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
		visitor.visit( cuT( "Exposure" )
			, m_config.exposure );
		visitor.visit( cuT( "NoiseIntensity" )
			, m_config.noiseIntensity );
	}

	void PostEffect::setParameters( c3d::Parameters parameters )
	{
		c3d::String param;

		if ( parameters.get( cuT( "exposure" ), param ) )
		{
			m_config.exposure = c3d::string::toFloat( param );
		}

		if ( parameters.get( cuT( "noiseIntensity" ), param ) )
		{
			m_config.noiseIntensity = c3d::string::toFloat( param );
		}
	}

	bool PostEffect::doInitialise( c3d::RenderDevice const & device
		, c3d::Texture const & source
		, c3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		auto dim = m_noiseImages[0].getDimensions();
		auto format = m_noiseImages[0].getPixelFormat();
		m_noiseImg = m_graph.createImage( crg::ImageData{ "FGNoise"
			, c3d::ImageCreateFlags::eNone
			, c3d::ImageType::e3D
			, format
			, { dim.getWidth(), dim.getHeight(), NoiseMapCount }
			, ( c3d::ImageUsageFlags::eSampled
				| c3d::ImageUsageFlags::eTransferDst ) } );
		m_noiseView = m_graph.createView( crg::ImageViewData{ "FGNoise"
			, m_noiseImg
			, c3d::ImageViewCreateFlags::eNone
			, c3d::ImageViewType::e3D
			, getFormat( m_noiseImg )
			, { c3d::ImageAspectFlags::eColor, 0u, 1u, 0u, 1u } } );
		auto extent = c3d::makeExtent2D( target.getExtent() );
		m_pass = &m_graph.createPass( "FilmGrain"
			, [this, extent]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto dim = m_noiseImages[0].getDimensions();
				auto format = convert( m_noiseImages[0].getPixelFormat() );
				auto & device = getRenderSystem()->getRenderDevice();
				auto staging = device->createStagingTexture( format
					, VkExtent2D{ dim.getWidth(), dim.getHeight() } );
				auto noiseImg = c3d::makeRawUnique< ashes::Image >( *device
					, graph.createImage( m_noiseImg )
					, ashes::ImageCreateInfo{ convert( m_noiseImg.data->info ) } );
				ashes::ImageView noiseView{ ashes::ImageViewCreateInfo{ convert( m_noiseView.data->info ) }
					, graph.createImageView( m_noiseView )
					, noiseImg.get() };
				auto data = device.graphicsData();

				for ( uint32_t i = 0u; i < NoiseMapCount; ++i )
				{
					staging->uploadTextureData( *data->queue
						, *data->commandPool
						, { getImageAspectFlags( m_noiseView.data->info.subresourceRange.aspectMask )
							, m_noiseView.data->info.subresourceRange.baseMipLevel
							, m_noiseView.data->info.subresourceRange.baseArrayLayer
							, m_noiseView.data->info.subresourceRange.layerCount }
						, format
						, { 0, 0, int32_t( i ) }
						, c3d::makeVkExtent3D( dim )
						, m_noiseImages[i].getBuffer().data()
						, noiseView );
				}

				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( extent )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &isEnabled() )
					.passIndex( &m_passIndex )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ 2u } );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		m_pass->addDependency( previousPass );
		m_configUbo.createPassBinding( *m_pass
			, "FilmCfg"
			, postfx::FilmCfgUboIdx );
		m_pass->addSampledView( m_noiseView
			, postfx::NoiseTexIdx
			, crg::SamplerDesc{ c3d::FilterMode::eLinear
				, c3d::FilterMode::eLinear
				, c3d::MipmapMode::eLinear
				, c3d::WrapMode::eClampToEdge
				, c3d::WrapMode::eClampToEdge
				, c3d::WrapMode::eClampToEdge } );
		m_pass->addSampledView( crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, postfx::SourceTexIdx );
		m_pass->addOutputColourView( crg::ImageViewIdArray{ target.targetViewId, source.targetViewId } );
		return true;
	}

	void PostEffect::doCleanup( c3d::RenderDevice const & device )
	{
	}

	void PostEffect::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		static auto const defaultTime = 25_ms;
		auto time = std::chrono::duration_cast< c3d::Milliseconds >( m_timer.getElapsed() );

		if ( m_firstUpdate )
		{
			time = 0_ms;
			m_firstUpdate = false;
		}

		auto & data = m_configUbo.getData();
		data.exposure = m_config.exposure;
		data.noiseIntensity = m_config.noiseIntensity;
		time = updater.tslf > 0_ms
			? updater.tslf
			: time;

		if ( time > 0_ms )
		{
			m_time += time;

			while ( m_time >= defaultTime )
			{
				m_time -= defaultTime;
				++m_timeIndex;
			}

			if ( m_timeIndex >= NoiseMapCount )
			{
				m_timeIndex -= NoiseMapCount;
			}

			data.time = float( m_timeIndex ) / float( NoiseMapCount );
		}
	}

	bool PostEffect::doWriteInto( c3d::StringStream & file
		, c3d::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"\n" ) );
		return true;
	}
}

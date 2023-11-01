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
		namespace c3d = castor3d::shader;

		static std::string const FilmGrainUbo = "FilmGrainUbo";
		static std::string const PixelSize = "c3d_pixelSize";
		static std::string const NoiseIntensity = "c3d_noiseIntensity";
		static std::string const Exposure = "c3d_exposure";
		static std::string const Time = "c3d_time";
		static std::string const SrcTex = "c3d_srcTex";
		static std::string const NoiseTex = "c3d_noiseTex";

		enum Idx : uint32_t
		{
			FilmCfgUboIdx,
			NoiseTexIdx,
			SourceTexIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::Engine & engine )
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
				, [&]( sdw::Vec3 const & a
					, sdw::Vec3 const & b )
				{
					auto comp = writer.declLocale( "comp"
						, pow( abs( b ), vec3( 2.2_f ) ) );

					IF( writer, comp.x() < 0.5_f && comp.y() < 0.5_f && comp.z() < 0.5_f )
					{
						writer.returnStmt( 2.0_f * a * b );
					}
					FI;

					writer.returnStmt( vec3( 1.0_f ) - 2.0_f * ( 1.0_f - a ) * ( 1.0_f - b ) );
				}
				, sdw::InVec3{ writer, "a" }
				, sdw::InVec3{ writer, "b" } );

			auto addNoise = writer.implementFunction< sdw::Vec3 >( "addNoise"
				, [&]( sdw::Vec3 const & color
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

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position().xy(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto colour = writer.declLocale( "colour"
						, c3d_srcTex.sample( in.uv() ).xyz() );
					colour = addNoise( colour, in.uv() );
					out.colour() = vec4( colour, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::array< castor::Image, PostEffect::NoiseMapCount > loadImages( castor3d::Engine const & engine )
		{
			auto & loader = engine.getImageLoader();
			return { loader.load( cuT( "FilmGrainNoise0" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer1_xpm ), uint32_t( castor::getCountOf( NoiseLayer1_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise1" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer2_xpm ), uint32_t( castor::getCountOf( NoiseLayer2_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise2" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer3_xpm ), uint32_t( castor::getCountOf( NoiseLayer3_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise3" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer4_xpm ), uint32_t( castor::getCountOf( NoiseLayer4_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise4" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer5_xpm ), uint32_t( castor::getCountOf( NoiseLayer5_xpm ) ), {} )
				, loader.load( cuT( "FilmGrainNoise5" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer6_xpm ), uint32_t( castor::getCountOf( NoiseLayer6_xpm ) ), {} ) };
		}
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "film_grain" );
	castor::String PostEffect::Name = cuT( "FilmGrain PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, "FilmGrain"
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params }
		, m_shader{ "FilmGrain", postfx::getProgram( *renderTarget.getEngine() ) }
		, m_stages{ makeProgramStates( renderSystem.getRenderDevice(), m_shader ) }
		, m_configUbo{ renderSystem.getRenderDevice().uboPool->getBuffer< Configuration >( 0u ) }
		, m_noiseImages{ postfx::loadImages( *renderTarget.getEngine() ) }
	{
		m_config.pixelSize = castor::Point2f{ m_renderTarget.getSize().getWidth()
			, m_renderTarget.getSize().getHeight() };
		m_config.noiseIntensity = 1.0f;
		m_config.exposure = 1.0f;
		m_config.time = 0.0f;
		setParameters( params );

		for ( auto & image : m_noiseImages )
		{
			auto format = image.getPixelFormat();

			if ( format == castor::PixelFormat::eR8G8B8_UNORM )
			{
				auto buffer = castor::PxBufferBase::create( image.getDimensions()
					, castor::PixelFormat::eR8G8B8A8_UNORM
					, image.getPxBuffer().getConstPtr()
					, image.getPxBuffer().getFormat()
					, image.getPxBuffer().getAlign() );
				image = castor::Image{ image.getName()
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

	castor3d::PostEffectUPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return castor::makeUniqueDerived< castor3d::PostEffect, PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
		visitor.visit( cuT( "Exposure" )
			, m_config.exposure );
		visitor.visit( cuT( "NoiseIntensity" )
			, m_config.noiseIntensity );
	}

	void PostEffect::setParameters( castor3d::Parameters parameters )
	{
		castor::String param;

		if ( parameters.get( "exposure", param ) )
		{
			m_config.exposure = castor::string::toFloat( param );
		}

		if ( parameters.get( "noiseIntensity", param ) )
		{
			m_config.noiseIntensity = castor::string::toFloat( param );
		}
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::Texture const & source
		, castor3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		auto dim = m_noiseImages[0].getDimensions();
		auto format = castor3d::convert( m_noiseImages[0].getPixelFormat() );
		m_noiseImg = m_graph.createImage( crg::ImageData{ "FGNoise"
			, 0u
			, VK_IMAGE_TYPE_3D
			, format
			, { dim.getWidth(), dim.getHeight(), NoiseMapCount }
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) } );
		m_noiseView = m_graph.createView( crg::ImageViewData{ "FGNoise"
			, m_noiseImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_3D
			, m_noiseImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		auto extent = castor3d::makeExtent2D( target.getExtent() );
		m_pass = &m_graph.createPass( "FilmGrain"
			, [this, extent]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto dim = m_noiseImages[0].getDimensions();
				auto format = castor3d::convert( m_noiseImages[0].getPixelFormat() );
				auto & device = getRenderSystem()->getRenderDevice();
				auto staging = device->createStagingTexture( format
					, VkExtent2D{ dim.getWidth(), dim.getHeight() } );
				ashes::ImagePtr noiseImg = std::make_unique< ashes::Image >( *device
					, graph.createImage( m_noiseImg )
					, m_noiseImg.data->info );
				ashes::ImageView noiseView{ ashes::ImageViewCreateInfo{ *noiseImg, m_noiseView.data->info }
					, graph.createImageView( m_noiseView )
					, noiseImg.get() };
				auto data = device.graphicsData();

				for ( uint32_t i = 0u; i < NoiseMapCount; ++i )
				{
					staging->uploadTextureData( *data->queue
						, *data->commandPool
						, { m_noiseView.data->info.subresourceRange.aspectMask
							, m_noiseView.data->info.subresourceRange.baseMipLevel
							, m_noiseView.data->info.subresourceRange.baseArrayLayer
							, m_noiseView.data->info.subresourceRange.layerCount }
						, format
						, { 0, 0, int32_t( i ) }
						, castor3d::makeExtent3D( dim )
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
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		m_pass->addDependency( previousPass );
		m_configUbo.createPassBinding( *m_pass
			, "FilmCfg"
			, postfx::FilmCfgUboIdx );
		m_pass->addSampledView( m_noiseView
			, postfx::NoiseTexIdx
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR
				, VK_SAMPLER_ADDRESS_MODE_REPEAT
				, VK_SAMPLER_ADDRESS_MODE_REPEAT
				, VK_SAMPLER_ADDRESS_MODE_REPEAT } );
		m_pass->addSampledView( crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, postfx::SourceTexIdx );
		m_pass->addOutputColourView( crg::ImageViewIdArray{ target.targetViewId, source.targetViewId } );
		return true;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		device.uboPool->putBuffer( m_configUbo );
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
	{
		static auto const defaultTime = 25_ms;
		auto time = std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );

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

	bool PostEffect::doWriteInto( castor::StringStream & file
		, castor::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"\n" ) );
		return true;
	}
}

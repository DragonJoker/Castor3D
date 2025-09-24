#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Miscellaneous/ProgressBar.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Background/Visitor.hpp>
#include <Castor3D/Shader/Ubos/RenderUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

#include <RenderGraph/FramePassGroup.hpp>

//*************************************************************************************************

CU_ImplementSmartPtr( atmosphere_scattering, AtmosphereBackground )

namespace c3d
{
	using namespace atmosphere_scattering;

	template<>
	class TextWriter< AtmosphereBackground >
		: public TextWriterT< AtmosphereBackground >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder )
			: TextWriterT< AtmosphereBackground >{ tabs }
			, m_folder{ folder }
		{
		}

		bool operator()( AtmosphereBackground const & background
			, StringStream & file )override
		{
			log::info << tabs() << cuT( "Writing AtmosphereBackground" ) << std::endl;
			auto result = false;
			file << ( cuT( "\n" ) + tabs() + cuT( "//Skybox\n" ) );

			if ( auto block{ beginBlock( file, cuT( "atmospheric_scattering" ) ) } )
			{
				auto transmittance = background.getTransmittance().getExtent();
				auto multiScatter = background.getMultiScatter().getExtent().width;
				auto atmosphereVolume = background.getVolumeResolution();
				result = ( background.getSunNode()
					? writeName( file, cuT( "sunNode" ), background.getSunNode()->getName() )
					: true );
				result = result && ( background.getPlanetNode()
					? writeName( file, cuT( "planetNode" ), background.getPlanetNode()->getName() )
					: true );
				result = result && write( file, cuT( "transmittanceResolution" ), transmittance.width, transmittance.height );
				result = result && write( file, cuT( "multiScatterResolution" ), multiScatter );
				result = result && write( file, cuT( "atmosphereVolumeResolution" ), atmosphereVolume );

				auto & config = background.getAtmosphereCfg();
				result = result && write( file, cuT( "sunIlluminance" ), config.sunIlluminance );
				result = result && write( file, cuT( "sunIlluminanceScale" ), config.sunIlluminanceScale );
				result = result && write( file, cuT( "rayMarchMinSPP" ), uint32_t( config.rayMarchMinMaxSPP->x ) );
				result = result && write( file, cuT( "rayMarchMaxSPP" ), uint32_t( config.rayMarchMinMaxSPP->y ) );
				result = result && write( file, cuT( "multipleScatteringFactor" ), config.multipleScatteringFactor );
				result = result && write( file, cuT( "solarIrradiance" ), config.solarIrradiance );
				result = result && write( file, cuT( "sunAngularRadius" ), config.sunAngularRadius );
				result = result && write( file, cuT( "absorptionExtinction" ), config.absorptionExtinction );
				result = result && write( file, cuT( "maxSunZenithAngle" ), acos( config.muSMin ) );
				result = result && write( file, cuT( "rayleighScattering" ), config.rayleighScattering );
				result = result && write( file, cuT( "mieScattering" ), config.mieScattering );
				result = result && write( file, cuT( "miePhaseFunctionG" ), config.miePhaseFunctionG );
				result = result && write( file, cuT( "mieExtinction" ), config.mieExtinction );
				result = result && write( file, cuT( "bottomRadius" ), config.bottomRadius );
				result = result && write( file, cuT( "topRadius" ), config.topRadius );
				result = result && write( file, cuT( "groundAlbedo" ), config.groundAlbedo );
				result = result && writeDensity( file, cuT( "minRayleighDensity" ), config.rayleighDensity[0] );
				result = result && writeDensity( file, cuT( "maxRayleighDensity" ), config.rayleighDensity[1] );
				result = result && writeDensity( file, cuT( "minMieDensity" ), config.mieDensity[0] );
				result = result && writeDensity( file, cuT( "maxMieDensity" ), config.mieDensity[1] );
				result = result && writeDensity( file, cuT( "minAbsorptionDensity" ), config.absorptionDensity[0] );
				result = result && writeDensity( file, cuT( "maxAbsorptionDensity" ), config.absorptionDensity[1] );

				if ( auto wblock{ beginBlock( file, cuT( "weather" ) ) } )
				{
					auto & weather = background.getWeatherCfg();
					result = result && write( file, cuT( "worleyResolution" ), background.getWorleyResolution() );
					result = result && write( file, cuT( "perlinWorleyResolution" ), background.getPerlinWorleyResolution() );
					result = result && write( file, cuT( "curlResolution" ), background.getCurlResolution() );
					result = result && write( file, cuT( "weatherResolution" ), background.getWeatherResolution() );
					result = result && write( file, cuT( "amplitude" ), weather.perlinAmplitude );
					result = result && write( file, cuT( "frequency" ), weather.perlinFrequency );
					result = result && write( file, cuT( "scale" ), weather.perlinScale );
					result = result && write( file, cuT( "octaves" ), weather.perlinOctaves );
				}

				if ( auto cblock{ beginBlock( file, cuT( "clouds" ) ) } )
				{
					auto & clouds = background.getCloudsCfg();
					result = result && write( file, cuT( "windDirection" ), clouds.windDirection );
					result = result && write( file, cuT( "speed" ), clouds.speed );
					result = result && write( file, cuT( "coverage" ), clouds.coverage );
					result = result && write( file, cuT( "crispiness" ), clouds.crispiness );
					result = result && write( file, cuT( "curliness" ), clouds.curliness );
					result = result && write( file, cuT( "density" ), clouds.density );
					result = result && write( file, cuT( "absorption" ), clouds.absorption );
					result = result && write( file, cuT( "innerRadius" ), clouds.innerRadius );
					result = result && write( file, cuT( "outerRadius" ), clouds.outerRadius );
					result = result && write( file, cuT( "topColour" ), clouds.colorTop );
					result = result && write( file, cuT( "bottomColour" ), clouds.colorBottom );
					result = result && write( file, cuT( "enablePowder" ), clouds.enablePowder != 0 );
					result = result && write( file, cuT( "topOffset" ), clouds.topOffset );
				}
			}

			return result;
		}

	private:
		bool writeDensity( c3d::StringStream & file
			, c3d::String const & name
			, DensityProfileLayer const & config )
		{
			auto result = false;

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = write( file, cuT( "layerWidth" ), config.layerWidth );
				result = result && write( file, cuT( "expTerm" ), config.expTerm );
				result = result && write( file, cuT( "expScale" ), config.expScale );
				result = result && write( file, cuT( "linearTerm" ), config.linearTerm );
				result = result && write( file, cuT( "constantTerm" ), config.constantTerm );
			}

			return result;
		}

	private:
		Path m_folder;
	};
}

//*************************************************************************************************

namespace atmosphere_scattering
{
	static constexpr bool disablePassOptimisations = false;

	AtmosphereBackground::CameraPasses::CameraPasses( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, AtmosphereBackground & background
		, c3d::Texture const & transmittance
		, c3d::Texture const & multiscatter
		, c3d::Texture const & worley
		, c3d::Texture const & perlinWorley
		, c3d::Texture const & curl
		, c3d::Texture const & weather
		, c3d::Texture const * depthObj
		, c3d::RenderUbo const & renderUbo
		, c3d::SceneUbo const & sceneUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CloudsUbo const & cloudsUbo
		, c3d::Extent2D const & size
		, c3d::Point2ui const & skyViewResolution
		, uint32_t volumeResolution
		, uint32_t index
		, bool forceVisible
		, c3d::BackgroundPassBase *& backgroundPass )
		: skyView{ device
			, background.getScene().getResources()
			, cuT( "AtmosphereSkyView" ) + c3d::string::toString( index )
			, {c3d::ImageCreateFlags::eNone
				, { skyViewResolution->x, skyViewResolution->y, 1u }, 1u, 1u
				, c3d::PixelFormat::eB10G11R11_UFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
			, { { .mipFilter = c3d::MipmapMode::eNearest } } }
		, volume{ device
			, background.getScene().getResources()
			, cuT( "AtmosphereVolume" ) + c3d::string::toString( index )
			, { c3d::ImageCreateFlags::eNone
				, { volumeResolution, volumeResolution, volumeResolution }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
			, { { .mipFilter = c3d::MipmapMode::eNearest } } }
		, skyColour{ device
			, background.getScene().getResources()
			, cuT( "SkyColour" ) + c3d::string::toString( index )
			, { c3d::ImageCreateFlags::eNone
				, { size.width, size.height, 1u }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eStorage | c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eColorAttachment }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } }
		, sunColour{ device
			, background.getScene().getResources()
			, cuT( "SunColour" ) + c3d::string::toString( index )
			, { c3d::ImageCreateFlags::eNone
				, { size.width, size.height, 1u }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eStorage | c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eColorAttachment }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } }
		, cloudsColour{ device
			, background.getScene().getResources()
			, cuT( "CloudsColour" ) + c3d::string::toString( index )
			, { c3d::ImageCreateFlags::eNone
				, { size.width, size.height, 1u }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eStorage | c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eColorAttachment }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } }
		, cloudsResult{ device
			, background.getScene().getResources()
			, cuT( "CloudsResult" ) + c3d::string::toString( index )
			, { c3d::ImageCreateFlags::eNone
				, { size.width, size.height, 1u }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } }
		, cameraUbo{ device, camAtmoChanged }
		, skyViewPass{ c3d::makeRawUnique< AtmosphereSkyViewPass >( graph
			, device
			, cameraUbo
			, atmosphereUbo
			, transmittance
			, skyView
			, index
			, camAtmoChanged ) }
		, volumePass{ c3d::makeRawUnique< AtmosphereVolumePass >( graph
			, device
			, cameraUbo
			, atmosphereUbo
			, transmittance
			, volume
			, index
			, camAtmoChanged ) }
		, volumetricCloudsPass{ c3d::makeRawUnique< CloudsVolumePass >( graph
			, device
			, atmosphereUbo
			, cameraUbo
			, cloudsUbo
			, transmittance
			, multiscatter
			, skyView
			, volume
			, perlinWorley
			, worley
			, curl
			, weather
			, depthObj
			, skyColour
			, sunColour
			, cloudsColour
			, index ) }
		, cloudsResolvePass{ c3d::makeRawUnique< CloudsResolvePass >( graph
			, device
			, cameraUbo
			, atmosphereUbo
			, cloudsUbo
			, skyColour
			, sunColour
			, cloudsColour
			, cloudsResult
			, index ) }
	{
		skyView.create();
		volume.create();
		skyColour.create();
		sunColour.create();
		cloudsColour.create();
		cloudsResult.create();
		auto & pass = graph.createPass( "Background"
			, [&background, &backgroundPass, &device, size, forceVisible]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = c3d::makeRawUnique< AtmosphereBackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, background
					, size
					, forceVisible );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		crg::SamplerDesc linearSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear };
		renderUbo.createPassBinding( pass, AtmosphereBackgroundBindings::eRenderConfig );
		sceneUbo.createPassBinding( pass, AtmosphereBackgroundBindings::eScene );
		pass.addInputSampledT( *cloudsResult.getSampledLastAttach(), AtmosphereBackgroundBindings::eClouds
			, linearSampler );
		lastPass = &pass;
	}

	AtmosphereBackground::CameraPasses::~CameraPasses()
	{
		skyView.destroy();
		volume.destroy();
		cloudsColour.destroy();
		sunColour.destroy();
		skyColour.destroy();
		cloudsResult.destroy();
	}

	void AtmosphereBackground::CameraPasses::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( cuT( "Atmosphere SkyView" )
			, skyView.getSampledViewId()
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Sky Colour" )
			, skyColour
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Sun Colour" )
			, sunColour
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Clouds Colour" )
			, cloudsColour
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Clouds Result" )
			, cloudsResult
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
	}

	void AtmosphereBackground::CameraPasses::update( c3d::CpuUpdater const & updater
		, c3d::Point3f const & sunDirection
		, c3d::Vector3f const & planetPosition )const
	{
		update( updater.renderSize
			, *updater.camera
			, updater.isSafeBanded
			, sunDirection
			, planetPosition );
	}

	void AtmosphereBackground::CameraPasses::update( c3d::Size const & renderSize
		, c3d::Camera const & camera
		, bool safeBanded
		, c3d::Point3f const & sunDirection
		, c3d::Vector3f const & planetPosition )const
	{
		cameraUbo.cpuUpdate( renderSize
			, camera
			, safeBanded
			, sunDirection
			, planetPosition );
	}

	//*********************************************************************************************

	static uint32_t constexpr SkyTexSize = 16u;

	AtmosphereBackground::AtmosphereBackground( c3d::Engine & engine
		, c3d::Scene & scene )
		: SceneBackground{ engine, scene, cuT( "Atmosphere" ), cuT( "atmosphere" ), false }
		, m_weatherUbo{ c3d::makeRawUnique< WeatherUbo >( engine.getRenderSystem()->getRenderDevice()
			, m_weatherChanged ) }
		, m_cloudsUbo{ c3d::makeRawUnique< CloudsUbo >( engine.getRenderSystem()->getRenderDevice()
			, m_cloudsChanged ) }
		, m_atmosphereUbo{ c3d::makeRawUnique< AtmosphereScatteringUbo >( engine.getRenderSystem()->getRenderDevice()
			, m_atmosphereChanged ) }
	{
	}

	AtmosphereBackground::~AtmosphereBackground()
	{
		m_worley.destroy();
		m_perlinWorley.destroy();
		m_curl.destroy();
		m_weather.destroy();
		m_transmittance.destroy();
		m_multiScatter.destroy();
	}

	void AtmosphereBackground::accept( c3d::BackgroundVisitor & visitor )
	{
		accept( static_cast< c3d::ConfigurationVisitorBase & >( visitor ) );
	}

	void AtmosphereBackground::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "Atmosphere Configuration" ) );
		visitor.visit( cuT( "Solar Irradiance" )
			, m_atmosphereCfg.solarIrradiance
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Sun Angular Radius" )
			, m_atmosphereCfg.sunAngularRadius
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Sun Illuminance" )
			, m_atmosphereCfg.sunIlluminance
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Sun Illuminance Scale" )
			, m_atmosphereCfg.sunIlluminanceScale
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Raymarch Min Max SPP " )
			, m_atmosphereCfg.rayMarchMinMaxSPP
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Absorption Extinction" )
			, m_atmosphereCfg.absorptionExtinction
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Mu S Min" )
			, m_atmosphereCfg.muSMin
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Rayleigh Scattering" )
			, m_atmosphereCfg.rayleighScattering
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Mie Phase Function G" )
			, m_atmosphereCfg.miePhaseFunctionG
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Mie Scattering" )
			, m_atmosphereCfg.mieScattering
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Mie Extinction" )
			, m_atmosphereCfg.mieExtinction
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Atmosphere Bottom Radius" )
			, m_atmosphereCfg.bottomRadius
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Atmosphere Top Radius" )
			, m_atmosphereCfg.topRadius
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Multiple Scattering Factor" )
			, m_atmosphereCfg.multipleScatteringFactor
			, &m_atmosphereChanged );
		visitor.visit( cuT( "Ground Albedo" )
			, m_atmosphereCfg.groundAlbedo
			, &m_atmosphereChanged );

		visitor.visit( cuT( "Clouds Configuration" ) );
		visitor.visit( cuT( "Clouds Speed" )
			, m_cloudsCfg.speed
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Coverage" )
			, m_cloudsCfg.coverage
			, c3d::makeRange( 0.0f, 1.0f )
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Crispiness" )
			, m_cloudsCfg.crispiness
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Curliness" )
			, m_cloudsCfg.curliness
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Density" )
			, m_cloudsCfg.density
			, c3d::makeRange( 0.0f, 1.0f )
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Absorption" )
			, m_cloudsCfg.absorption
			, c3d::makeRange( 0.0f, 1.0f )
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Top Offset" )
			, m_cloudsCfg.topOffset
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Top Colour" )
			, m_cloudsCfg.colorTop
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Bottom Colour" )
			, m_cloudsCfg.colorBottom
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Dome Bottom" )
			, m_cloudsCfg.innerRadius
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Dome Top" )
			, m_cloudsCfg.outerRadius
			, &m_cloudsChanged );

		visitor.visit( cuT( "Weather Configuration" ) );
		visitor.visit( cuT( "Clouds Perlin Amplitude" )
			, m_weatherCfg.perlinAmplitude
			, &m_weatherChanged );
		visitor.visit( cuT( "Clouds Perlin Frequency" )
			, m_weatherCfg.perlinFrequency
			, &m_weatherChanged );
		visitor.visit( cuT( "Clouds Perlin Scale" )
			, m_weatherCfg.perlinScale
			, &m_weatherChanged );
		visitor.visit( cuT( "Clouds Perlin Octaves" )
			, m_weatherCfg.perlinOctaves
			, &m_weatherChanged );

		visitor.visit( cuT( "Atmosphere Transmittance" )
			, m_transmittance
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Atmosphere Multiscatter" )
			, m_multiScatter
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );

		for ( auto const & [_, pass] : m_cameraPasses )
		{
			pass->accept( visitor );
		}

		visitor.visit( cuT( "Weather Result" )
			, m_weather
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Curl Noise" )
			, m_curl
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );

		uint32_t index{};
		for ( auto & layerViews : m_worley )
		{
			visitor.visit( cuT( "Worley Noise Slice " ) + c3d::string::toString( index )
				, layerViews.sampledViewId
				, c3d::ImageLayout::eShaderReadOnly
				, c3d::TextureFactors::tex3DSlice( index ).invert( true ) );
			++index;
		}

		index = {};
		for ( auto & layerViews : m_perlinWorley )
		{
			visitor.visit( cuT( "Perlin Worley Noise Slice " )+ c3d::string::toString( index )
				, layerViews.sampledViewId
				, c3d::ImageLayout::eShaderReadOnly
				, c3d::TextureFactors::tex3DSlice( index ).invert( true ) );
			++index;
		}
	}

	void AtmosphereBackground::createBackgroundPass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::ProgressBar * progress
		, c3d::Extent2D const & size
		, c3d::Texture & colour
		, c3d::Texture * depth
		, c3d::Texture const * depthObj
		, c3d::UniformBufferOffsetT< c3d::ModelBufferConfiguration > const & modelUbo
		, c3d::CameraUbo const & cameraUbo
		, c3d::RenderUbo const & renderUbo
		, c3d::SceneUbo const & sceneUbo
		, bool clearColour
		, bool clearDepth
		, bool forceVisible
		, c3d::BackgroundPassBase *& backgroundPass )
	{
		if ( !m_transmittancePass )
		{
			m_worleyPass = c3d::makeRawUnique< CloudsWorleyPass >( graph
				, device
				, m_worley
				, m_generateWorley );
			m_perlinWorleyPass = c3d::makeRawUnique< CloudsPerlinPass >( graph
				, device
				, m_perlinWorley
				, m_generatePerlinWorley );
			m_curlPass = c3d::makeRawUnique< CloudsCurlPass >( graph
				, device
				, m_curl
				, m_generateCurl );
			m_weatherPass = c3d::makeRawUnique< CloudsWeatherPass >( graph
				, device
				, *m_weatherUbo
				, m_weather
				, m_weatherChanged );
			m_transmittancePass = c3d::makeRawUnique< AtmosphereTransmittancePass >( graph
				, device
				, *m_atmosphereUbo
				, m_transmittance
				, m_atmosphereChanged );
			m_multiScatteringPass = c3d::makeRawUnique< AtmosphereMultiScatteringPass >( graph
				, device
				, *m_atmosphereUbo
				, m_transmittance
				, m_multiScatter
				, m_atmosphereChanged );
		}

		auto it = findCameraPass( &colour );

		if ( it == m_cameraPasses.end() )
		{
			it = m_cameraPasses.emplace( &colour
				, c3d::makeRawUnique< CameraPasses >( graph
					, device
					, *this
					, m_transmittance
					, m_multiScatter
					, m_worley
					, m_perlinWorley
					, m_curl
					, m_weather
					, depthObj
					, renderUbo
					, sceneUbo
					, *m_atmosphereUbo
					, *m_cloudsUbo
					, size
					, m_skyViewResolution
					, m_volumeResolution
					, uint32_t( m_cameraPasses.size() )
					, forceVisible
					, backgroundPass ) ).first;
			auto & pass = *it->second->lastPass;

			if ( depth )
			{
				if ( clearDepth )
					depth->setLastAttach( pass.addOutputDepthStencilTarget( depth->getTargetViewId()
						, c3d::defaultClearDepthStencil ) );
				else
					depth->setLastAttach( pass.addInOutDepthStencilTarget( *depth->getLastAttach() ) );
			}

			if ( clearColour )
				colour.setLastAttach( pass.addOutputColourTarget( colour.getTargetViewId()
					, c3d::transparentBlackClearColor ) );
			else
				colour.setLastAttach( pass.addInOutColourTarget( *colour.getLastAttach() ) );
		}
	}

	bool AtmosphereBackground::write( c3d::String const & tabs
		, c3d::Path const & folder
		, c3d::StringStream & stream )const
	{
		return c3d::TextWriter< AtmosphereBackground >{ tabs, folder }( *this, stream );
	}

	c3d::String const & AtmosphereBackground::getModelName()const
	{
		return AtmosphereBackgroundModel::Name;
	}

	void AtmosphereBackground::loadWorley( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto const & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_worleyResolution = dimension;
		m_worley = c3d::Texture{ device
			, resources
			, cuT( "WorleyNoise" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, dimension }, 1u
				, c3d::getMipLevels( c3d::Extent3D{ dimension, dimension, dimension }, c3d::PixelFormat::eR8G8B8A8_UNORM )
				, c3d::PixelFormat::eR8G8B8A8_UNORM
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eTransferSrc | c3d::ImageUsageFlags::eTransferDst | c3d::ImageUsageFlags::eStorage }
			, { { .addressMode = c3d::WrapMode::eRepeat } } };
		notifyChanged();
	}

	void AtmosphereBackground::loadPerlinWorley( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto const & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_perlinWorleyResolution = dimension;
		m_perlinWorley = c3d::Texture{ device
			, resources
			, cuT( "PerlinWorleyNoise" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, dimension }, 1u
				, c3d::getMipLevels( c3d::Extent3D{ dimension, dimension , dimension }, c3d::PixelFormat::eR8G8B8A8_UNORM )
				, c3d::PixelFormat::eR8G8B8A8_UNORM
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eTransferSrc | c3d::ImageUsageFlags::eTransferDst | c3d::ImageUsageFlags::eStorage }
			, { { .addressMode = c3d::WrapMode::eRepeat } } };
		notifyChanged();
	}

	void AtmosphereBackground::loadCurl( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto const & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_curlResolution = dimension;
		m_curl = c3d::Texture{ device
			, resources
			, cuT( "CurlNoise" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, 1u }, 1u, 1u
				, c3d::PixelFormat::eR8G8_UNORM
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eTransferSrc | c3d::ImageUsageFlags::eTransferDst | c3d::ImageUsageFlags::eStorage }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } };
		notifyChanged();
	}

	void AtmosphereBackground::loadWeather( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto const & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_weatherResolution = dimension;
		m_weather = c3d::Texture{ device
			, resources
			, cuT( "Weather" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, 1u }, 1u, 1u
				, c3d::PixelFormat::eR32G32_SFLOAT
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eStorage | c3d::ImageUsageFlags::eColorAttachment }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } };
		notifyChanged();
	}

	void AtmosphereBackground::loadTransmittance( c3d::Point2ui const & dimensions )
	{
		auto & resources = getScene().getResources();
		auto const & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_transmittance = c3d::Texture{ device
			, resources
			, cuT( "Transmittance" )
			, { c3d::ImageCreateFlags::eNone
			, { dimensions->x, dimensions->y, 1u }, 1u, 1u
			, c3d::PixelFormat::eR16G16B16A16_SFLOAT
			, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
			, { { .mipFilter = c3d::MipmapMode::eNearest } } };
		notifyChanged();
	}

	void AtmosphereBackground::loadMultiScatter( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto const & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_multiScatter = c3d::Texture{ device
			, resources
			, cuT( "MultiScatter" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, 1u }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eStorage }
			, { { .mipFilter = c3d::MipmapMode::eNearest } } };
		notifyChanged();
	}

	void AtmosphereBackground::loadAtmosphereVolume( uint32_t dimension )
	{
		m_volumeResolution = dimension;
		notifyChanged();
	}

	void AtmosphereBackground::loadSkyView( c3d::Point2ui const & dimensions )
	{
		m_skyViewResolution = dimensions;
		notifyChanged();
	}

	bool AtmosphereBackground::doInitialise( c3d::RenderDevice const & device )
	{
		auto data = device.graphicsData();
		auto & resources = getScene().getResources();
		m_textureId = c3d::Texture{ device
			, resources
			, cuT( "Dummy" )
			, { c3d::ImageCreateFlags::eNone
				, { SkyTexSize, SkyTexSize, 1u }, 1u, 1u
				, c3d::PixelFormat::eB10G11R11_UFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
			, { { .mipFilter = c3d::MipmapMode::eNearest } } };
		m_transmittance.create();
		m_multiScatter.create();
		m_textureId.create();
		m_texture = c3d::makeUnique< c3d::TextureLayout >( device.renderSystem
			, cuT( "AtmosphereBackground/Dummy" )
			, *m_textureId.image
			, m_textureId.getWholeViewId() );
		m_hdr = true;
		m_srgb = false;
		m_timer.getElapsed();
		auto result = m_texture->initialise( device );

		if ( result )
		{
			m_time = 0.0f;
			m_atmosphereUbo->cpuUpdate( m_atmosphereCfg, *m_sunNode, *m_planetNode );
			m_weatherUbo->cpuUpdate( m_weatherCfg );
			m_cloudsUbo->cpuUpdate( m_cloudsCfg, m_time );
		}

		return result;
	}

	void AtmosphereBackground::doCleanup()
	{
	}

	void AtmosphereBackground::doCpuUpdate( c3d::CpuUpdater & updater )const
	{
		m_generateWorley = m_generateWorley && m_first;
		m_generatePerlinWorley = m_generatePerlinWorley && m_first;
		m_generateCurl = m_generateCurl && m_first;

		if constexpr ( disablePassOptimisations )
			m_first = true;

		m_atmosphereChanged = m_first;
		m_weatherChanged = m_first;
		m_cloudsChanged = m_first;
		m_first = false;

		CU_Require( m_sunNode );
		CU_Require( m_planetNode );
		if ( m_planetNode && m_sunNode )
		{
			auto [sunDirection, planetPosition] = m_atmosphereUbo->cpuUpdate( m_atmosphereCfg, *m_sunNode, *m_planetNode );
			auto time = updater.tslf > 0_ms
				? updater.tslf
				: std::chrono::duration_cast< c3d::Milliseconds >( m_timer.getElapsed() );
			m_time += float( time.count() ) / 1000.0f;
			m_weatherUbo->cpuUpdate( m_weatherCfg );
			m_cloudsUbo->cpuUpdate( m_cloudsCfg, m_time );
			auto it = findCameraPass( updater.targetImage );

			if ( it != m_cameraPasses.end() )
			{
				it->second->camAtmoChanged = m_atmosphereChanged;
				it->second->update( updater, sunDirection, planetPosition );
			}
		}
	}

	void AtmosphereBackground::doGpuUpdate( c3d::GpuUpdater & updater )const
	{
	}

	void AtmosphereBackground::doUpload( c3d::UploadData & uploader )
	{
	}

	void AtmosphereBackground::doAddPassBindings( crg::FramePass & pass
		, c3d::Texture * targetImage
		, uint32_t & index )const
	{
		auto it = findCameraPass( targetImage );

		if ( it != m_cameraPasses.end() )
		{
			it->second->cameraUbo.createPassBinding( pass, index );
			++index;
			m_atmosphereUbo->createPassBinding( pass, index );
			++index;
			m_cloudsUbo->createPassBinding( pass, index );
			++index;
			crg::SamplerDesc linearClampSampler{ c3d::FilterMode::eLinear
				, c3d::FilterMode::eLinear };
			pass.addInputSampled( *m_transmittance.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *m_multiScatter.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *it->second->skyView.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *it->second->volume.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *it->second->cloudsResult.getSampledLastAttach(), index, linearClampSampler );
			++index;
		}
	}

	void AtmosphereBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, shaderStages );	// CameraBuffer
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, shaderStages );	// AtmosphereBuffer
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, shaderStages );	// CloudsBuffer
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages );	// c3d_mapTransmittance
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages );	// c3d_mapMultiScatter
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages );	// c3d_mapSkyView
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages );	// c3d_mapVolume
		c3d::addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages );	// c3d_mapClouds
	}

	void AtmosphereBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, c3d::Texture * targetImage
		, uint32_t & index )const
	{
		auto it = findCameraPass( targetImage );

		if ( it != m_cameraPasses.end() )
		{
			descriptorWrites.push_back( it->second->cameraUbo.getDescriptorWrite( index ) );
			++index;
			descriptorWrites.push_back( m_atmosphereUbo->getDescriptorWrite( index ) );
			++index;
			descriptorWrites.push_back( m_cloudsUbo->getDescriptorWrite( index ) );
			++index;
			c3d::bindTexture( m_transmittance.getSampledView()
				, *m_transmittance.sampler
				, descriptorWrites
				, index );
			c3d::bindTexture( m_multiScatter.getSampledView()
				, *m_multiScatter.sampler
				, descriptorWrites
				, index );
			c3d::bindTexture( it->second->skyView.getSampledView()
				, *it->second->skyView.sampler
				, descriptorWrites
				, index );
			c3d::bindTexture( it->second->volume.getSampledView()
				, *it->second->volume.sampler
				, descriptorWrites
				, index );
			c3d::bindTexture( it->second->cloudsResult.getSampledView()
				, *it->second->volume.sampler
				, descriptorWrites
				, index );
		}
	}

	//************************************************************************************************
}

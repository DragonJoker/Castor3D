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

#include <ashespp/Image/StagingTexture.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

#include <RenderGraph/FramePassGroup.hpp>

//*************************************************************************************************

namespace castor
{
	using namespace castor3d;
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

			if ( auto block{ beginBlock( file, "atmospheric_scattering" ) } )
			{
				auto transmittance = background.getTransmittance().getExtent();
				auto multiScatter = background.getMultiScatter().getExtent().width;
				auto atmosphereVolume = background.getVolumeResolution();
				result = ( background.getSunNode()
					? writeName( file, "sunNode", background.getSunNode()->getName() )
					: true );
				result = result && ( background.getPlanetNode()
					? write( file, "planetNode", background.getPlanetNode()->getName() )
					: true );
				result = result && write( file, "transmittanceResolution", transmittance.width, transmittance.height );
				result = result && write( file, "multiScatterResolution", multiScatter );
				result = result && write( file, "atmosphereVolumeResolution", atmosphereVolume );

				auto & config = background.getAtmosphereCfg();
				result = result && write( file, "sunIlluminance", config.sunIlluminance );
				result = result && write( file, "sunIlluminanceScale", config.sunIlluminanceScale );
				result = result && write( file, "rayMarchMinSPP", uint32_t( config.rayMarchMinMaxSPP->x ) );
				result = result && write( file, "rayMarchMaxSPP", uint32_t( config.rayMarchMinMaxSPP->y ) );
				result = result && write( file, "multipleScatteringFactor", config.multipleScatteringFactor );
				result = result && write( file, "solarIrradiance", config.solarIrradiance );
				result = result && write( file, "sunAngularRadius", config.sunAngularRadius );
				result = result && write( file, "absorptionExtinction", config.absorptionExtinction );
				result = result && write( file, "maxSunZenithAngle", acos( config.muSMin ) );
				result = result && write( file, "rayleighScattering", config.rayleighScattering );
				result = result && write( file, "mieScattering", config.mieScattering );
				result = result && write( file, "miePhaseFunctionG", config.miePhaseFunctionG );
				result = result && write( file, "mieExtinction", config.mieExtinction );
				result = result && write( file, "bottomRadius", config.bottomRadius );
				result = result && write( file, "topRadius", config.topRadius );
				result = result && write( file, "groundAlbedo", config.groundAlbedo );
				result = result && writeDensity( file, "minRayleighDensity", config.rayleighDensity[0] );
				result = result && writeDensity( file, "minRayleighDensity", config.rayleighDensity[1] );
				result = result && writeDensity( file, "minRayleighDensity", config.mieDensity[0] );
				result = result && writeDensity( file, "minRayleighDensity", config.mieDensity[1] );
				result = result && writeDensity( file, "minRayleighDensity", config.absorptionDensity[0] );
				result = result && writeDensity( file, "minRayleighDensity", config.absorptionDensity[1] );

				if ( auto wblock{ beginBlock( file, "weather" ) } )
				{
					auto & weather = background.getWeatherCfg();
					result = result && write( file, "worleyResolution", background.getWorleyResolution() );
					result = result && write( file, "perlinWorleyResolution", background.getPerlinWorleyResolution() );
					result = result && write( file, "curlResolution", background.getCurlResolution() );
					result = result && write( file, "weatherResolution", background.getWeatherResolution() );
					result = result && write( file, "amplitude", weather.perlinAmplitude );
					result = result && write( file, "frequency", weather.perlinFrequency );
					result = result && write( file, "scale", weather.perlinScale );
					result = result && write( file, "octaves", weather.perlinOctaves );
				}

				if ( auto cblock{ beginBlock( file, "clouds" ) } )
				{
					auto & clouds = background.getCloudsCfg();
					result = result && write( file, "windDirection", clouds.windDirection );
					result = result && write( file, "speed", clouds.speed );
					result = result && write( file, "coverage", clouds.coverage );
					result = result && write( file, "crispiness", clouds.crispiness );
					result = result && write( file, "curliness", clouds.curliness );
					result = result && write( file, "density", clouds.density );
					result = result && write( file, "absorption", clouds.absorption );
					result = result && write( file, "innerRadius", clouds.innerRadius );
					result = result && write( file, "outerRadius", clouds.outerRadius );
					result = result && write( file, "topColour", clouds.colorTop );
					result = result && write( file, "bottomColour", clouds.colorBottom );
					result = result && write( file, "enablePowder", clouds.enablePowder != 0 );
					result = result && write( file, "topOffset", clouds.topOffset );
				}
			}

			return result;
		}

	private:
		bool writeDensity( castor::StringStream & file
			, castor::String const & name
			, DensityProfileLayer const & config )
		{
			auto result = false;

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = write( file, "layerWidth", config.layerWidth );
				result = result && write( file, "expTerm", config.expTerm );
				result = result && write( file, "expScale", config.expScale );
				result = result && write( file, "linearTerm", config.linearTerm );
				result = result && write( file, "constantTerm", config.constantTerm );
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
	AtmosphereBackground::CameraPasses::CameraPasses( crg::FramePassGroup & graph
		, castor3d::RenderDevice const & device
		, AtmosphereBackground & background
		, crg::FramePass const & transmittancePass
		, crg::FramePass const & multiscatterPass
		, crg::FramePass const & weatherPass
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & multiscatter
		, crg::ImageViewId const & worley
		, crg::ImageViewId const & perlinWorley
		, crg::ImageViewId const & curl
		, crg::ImageViewId const & weather
		, crg::ImageViewIdArray const & colour
		, crg::ImageViewId const * depthObj
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CloudsUbo const & cloudsUbo
		, VkExtent2D const & size
		, castor::Point2ui const & skyViewResolution
		, uint32_t volumeResolution
		, uint32_t index
		, castor3d::BackgroundPassBase *& backgroundPass )
		: skyView{ device
			, background.getScene().getResources()
			, "AtmosphereSkyView" + std::to_string( index )
			, 0u
			, { skyViewResolution->x, skyViewResolution->y, 1u }
			, 1u
			, 1u
			, VK_FORMAT_B10G11R11_UFLOAT_PACK32
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST }
		, volume{ device
			, background.getScene().getResources()
			, "AtmosphereVolume" + std::to_string( index )
			, 0u
			, { volumeResolution, volumeResolution, volumeResolution }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST }
		, skyColour{ device
			, background.getScene().getResources()
			, "SkyColour" + std::to_string( index )
			, 0u
			, { size.width, size.height, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_REPEAT }
		, sunColour{ device
			, background.getScene().getResources()
			, "SunColour" + std::to_string( index )
			, 0u
			, { size.width, size.height, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_REPEAT }
		, cloudsColour{ device
			, background.getScene().getResources()
			, "CloudsColour" + std::to_string( index )
			, 0u
			, { size.width, size.height, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_REPEAT }
		, cloudsResult{ device
			, background.getScene().getResources()
			, "CloudsResult" + std::to_string( index )
			, 0u
			, { size.width, size.height, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_REPEAT }
		, cameraUbo{ device, camAtmoChanged }
		, skyViewPass{ std::make_unique< AtmosphereSkyViewPass >( graph
			, crg::FramePassArray{ &transmittancePass }
			, device
			, cameraUbo
			, atmosphereUbo
			, transmittance
			, skyView.targetViewId
			, index
			, camAtmoChanged ) }
		, volumePass{ std::make_unique< AtmosphereVolumePass >( graph
			, crg::FramePassArray{ &transmittancePass }
			, device
			, cameraUbo
			, atmosphereUbo
			, transmittance
			, volume.targetViewId
			, index
			, camAtmoChanged ) }
		, volumetricCloudsPass{ std::make_unique< CloudsVolumePass >( graph
			, crg::FramePassArray{ &transmittancePass , &weatherPass, &skyViewPass->getLastPass(), &multiscatterPass }
			, device
			, atmosphereUbo
			, cameraUbo
			, cloudsUbo
			, transmittance
			, multiscatter
			, skyView.sampledViewId
			, volume.sampledViewId
			, perlinWorley
			, worley
			, curl
			, weather
			, depthObj
			, skyColour.targetViewId
			, sunColour.targetViewId
			, cloudsColour.targetViewId
			, index ) }
		, cloudsResolvePass{ std::make_unique< CloudsResolvePass >( graph
			, crg::FramePassArray{ &volumetricCloudsPass->getLastPass() }
			, device
			, cameraUbo
			, atmosphereUbo
			, cloudsUbo
			, skyColour.sampledViewId
			, sunColour.sampledViewId
			, cloudsColour.sampledViewId
			, cloudsResult.targetViewId
			, index ) }
	{
		skyView.create();
		volume.create();
		skyColour.create();
		sunColour.create();
		cloudsColour.create();
		cloudsResult.create();
		auto & pass = graph.createPass( "Background"
			, [&background, &backgroundPass, &device, size, colour]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< AtmosphereBackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, background
					, size
					, colour );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		pass.addDependency( cloudsResolvePass->getLastPass() );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		pass.addSampledView( cloudsResult.sampledViewId
			, AtmosphereBackgroundPass::eClouds
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
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

	void AtmosphereBackground::CameraPasses::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( "Atmosphere SkyView"
			, skyView.sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );
		visitor.visit( "Sky Colour"
			, skyColour
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );
		visitor.visit( "Sun Colour"
			, sunColour
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );
		visitor.visit( "Clouds Colour"
			, cloudsColour
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );
		visitor.visit( "Clouds Result"
			, cloudsResult
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );
	}

	void AtmosphereBackground::CameraPasses::update( castor3d::CpuUpdater & updater
		, castor::Point3f const & sunDirection
		, castor::Vector3f const & planetPosition )const
	{
		update( *updater.camera
			, updater.isSafeBanded
			, sunDirection
			, planetPosition );
	}

	void AtmosphereBackground::CameraPasses::update( castor3d::Camera const & camera
		, bool safeBanded
		, castor::Point3f const & sunDirection
		, castor::Vector3f const & planetPosition )const
	{
		cameraUbo.cpuUpdate( camera
			, safeBanded
			, sunDirection
			, planetPosition );
	}

	//*********************************************************************************************

	static uint32_t constexpr SkyTexSize = 16u;

	AtmosphereBackground::AtmosphereBackground( castor3d::Engine & engine
		, castor3d::Scene & scene )
		: SceneBackground{ engine, scene, cuT( "Atmosphere" ), cuT( "atmosphere" ), false }
		, m_weatherUbo{ std::make_unique< WeatherUbo >( engine.getRenderSystem()->getRenderDevice()
			, m_weatherChanged ) }
		, m_cloudsUbo{ std::make_unique< CloudsUbo >( engine.getRenderSystem()->getRenderDevice()
			, m_cloudsChanged ) }
		, m_atmosphereUbo{ std::make_unique< AtmosphereScatteringUbo >( engine.getRenderSystem()->getRenderDevice()
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

	void AtmosphereBackground::accept( castor3d::BackgroundVisitor & visitor )
	{
		accept( static_cast< castor3d::PipelineVisitor & >( visitor ) );
	}

	void AtmosphereBackground::accept( castor3d::PipelineVisitor & visitor )
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
			, castor::makeRange( 0.0f, 1.0f )
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Crispiness" )
			, m_cloudsCfg.crispiness
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Curliness" )
			, m_cloudsCfg.curliness
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Density" )
			, m_cloudsCfg.density
			, castor::makeRange( 0.0f, 1.0f )
			, &m_cloudsChanged );
		visitor.visit( cuT( "Clouds Absorption" )
			, m_cloudsCfg.absorption
			, castor::makeRange( 0.0f, 1.0f )
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

		visitor.visit( "Atmosphere Transmittance"
			, m_transmittance
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );
		visitor.visit( "Atmosphere Multiscatter"
			, m_multiScatter
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );

		for ( auto & cameraPass : m_cameraPasses )
		{
			cameraPass.second->accept( visitor );
		}

		visitor.visit( "Weather Result"
			, m_weather
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );
		visitor.visit( "Curl Noise"
			, m_curl
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, castor3d::TextureFactors{}.invert( true ) );

		for ( uint32_t index = 0u; index < m_worley.subViewsId.size(); ++index )
		{
			visitor.visit( "Worley Noise Slice " + castor::string::toString( index )
				, m_worley.sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, castor3d::TextureFactors::tex3DSlice( index ).invert( true ) );
		}

		for ( uint32_t index = 0u; index < m_perlinWorley.subViewsId.size(); ++index )
		{
			visitor.visit( "Perlin Worley Noise Slice " + castor::string::toString( index )
				, m_perlinWorley.sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, castor3d::TextureFactors::tex3DSlice( index ).invert( true ) );
		}
	}

	crg::FramePass & AtmosphereBackground::createBackgroundPass( crg::FramePassGroup & graph
		, castor3d::RenderDevice const & device
		, castor3d::ProgressBar * progress
		, VkExtent2D const & size
		, crg::ImageViewIdArray const & colour
		, crg::ImageViewIdArray const & depth
		, crg::ImageViewId const * depthObj
		, castor3d::UniformBufferOffsetT< castor3d::ModelBufferConfiguration > const & modelUbo
		, castor3d::MatrixUbo const & matrixUbo
		, castor3d::HdrConfigUbo const & hdrConfigUbo
		, castor3d::SceneUbo const & sceneUbo
		, bool clearColour
		, castor3d::BackgroundPassBase *& backgroundPass )
	{
		if ( !m_transmittancePass )
		{
			m_worleyPass = std::make_unique< CloudsWorleyPass >( graph
				, crg::FramePassArray{}
				, device
				, m_worley.sampledViewId
				, m_generateWorley );
			m_perlinWorleyPass = std::make_unique< CloudsPerlinPass >( graph
				, crg::FramePassArray{}
				, device
				, m_perlinWorley.sampledViewId
				, m_generatePerlinWorley );
			m_curlPass = std::make_unique< CloudsCurlPass >( graph
				, crg::FramePassArray{}
				, device
				, m_curl.targetViewId
				, m_generateCurl );
			m_weatherPass = std::make_unique< CloudsWeatherPass >( graph
				, crg::FramePassArray{ &m_worleyPass->getLastPass()
					, &m_perlinWorleyPass->getLastPass()
					, &m_curlPass->getLastPass() }
				, device
				, *m_weatherUbo
				, m_weather.targetViewId
				, m_weatherChanged );
			m_transmittancePass = std::make_unique< AtmosphereTransmittancePass >( graph
				, crg::FramePassArray{}
				, device
				, *m_atmosphereUbo
				, m_transmittance.targetViewId
				, m_atmosphereChanged );
			m_multiScatteringPass = std::make_unique< AtmosphereMultiScatteringPass >( graph
				, crg::FramePassArray{ &m_transmittancePass->getLastPass() }
				, device
				, *m_atmosphereUbo
				, m_transmittance.sampledViewId
				, m_multiScatter.targetViewId
				, m_atmosphereChanged );
		}

		auto it = findCameraPass( colour );

		if ( it == m_cameraPasses.end() )
		{
			it = m_cameraPasses.emplace( colour.front().data->image.data
				, std::make_unique< CameraPasses >( graph
					, device
					, *this
					, m_transmittancePass->getLastPass()
					, m_multiScatteringPass->getLastPass()
					, m_weatherPass->getLastPass()
					, m_transmittance.sampledViewId
					, m_multiScatter.sampledViewId
					, m_worley.sampledViewId
					, m_perlinWorley.sampledViewId
					, m_curl.sampledViewId
					, m_weather.sampledViewId
					, colour
					, depthObj
					, *m_atmosphereUbo
					, *m_cloudsUbo
					, size
					, m_skyViewResolution
					, m_volumeResolution
					, uint32_t( m_cameraPasses.size() )
					, backgroundPass ) ).first;
			auto & pass = *it->second->lastPass;
			pass.addDependency( m_multiScatteringPass->getLastPass() );
			pass.addDependency( m_weatherPass->getLastPass() );
			pass.addImplicitDepthStencilView( depth
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			pass.addOutputColourView( colour
				, castor3d::transparentBlackClearColor );
			pass.addInOutDepthStencilView( depth );
		}

		return *it->second->lastPass;
	}

	bool AtmosphereBackground::write( castor::String const & tabs
		, castor::Path const & folder
		, castor::StringStream & stream )const
	{
		return castor::TextWriter< AtmosphereBackground >{ tabs, folder }( *this, stream );
	}

	castor::String const & AtmosphereBackground::getModelName()const
	{
		return AtmosphereBackgroundModel::Name;
	}

	void AtmosphereBackground::loadWorley( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_worleyResolution = dimension;
		m_worley = castor3d::Texture{ device
			, resources
			, "WorleyNoise"
			, 0u
			, { dimension, dimension, dimension }
			, 1u
			, castor3d::getMipLevels( VkExtent3D{ dimension, dimension, dimension }, VK_FORMAT_R8G8B8A8_UNORM )
			, VK_FORMAT_R8G8B8A8_UNORM
			, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		notifyChanged();
	}

	void AtmosphereBackground::loadPerlinWorley( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_perlinWorleyResolution = dimension;
		m_perlinWorley = castor3d::Texture{ device
			, resources
			, "PerlinWorleyNoise"
			, 0u
			, { dimension, dimension, dimension }
			, 1u
			, castor3d::getMipLevels( VkExtent3D{ dimension, dimension , dimension }, VK_FORMAT_R8G8B8A8_UNORM )
			, VK_FORMAT_R8G8B8A8_UNORM
			, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		notifyChanged();
	}

	void AtmosphereBackground::loadCurl( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_curlResolution = dimension;
		m_curl = castor3d::Texture{ device
			, resources
			, "CurlNoise"
			, 0u
			, { dimension, dimension, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R8G8_UNORM
			, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		notifyChanged();
	}

	void AtmosphereBackground::loadWeather( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_weatherResolution = dimension;
		m_weather = castor3d::Texture{ device
			, resources
			, "Weather"
			, 0u
			, { dimension, dimension, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R32G32_SFLOAT
			, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		notifyChanged();
	}

	void AtmosphereBackground::loadTransmittance( castor::Point2ui const & dimensions )
	{
		auto & resources = getScene().getResources();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_transmittance = castor3d::Texture{ device
			, resources
			, "Transmittance"
			, 0u
			, { dimensions->x, dimensions->y, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST };
		notifyChanged();
	}

	void AtmosphereBackground::loadMultiScatter( uint32_t dimension )
	{
		auto & resources = getScene().getResources();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_multiScatter = castor3d::Texture{ device
			, resources
			, "MultiScatter"
			, 0u
			, { dimension, dimension, 1u }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST };
		notifyChanged();
	}

	void AtmosphereBackground::loadAtmosphereVolume( uint32_t dimension )
	{
		m_volumeResolution = dimension;
		notifyChanged();
	}

	void AtmosphereBackground::loadSkyView( castor::Point2ui const & dimensions )
	{
		m_skyViewResolution = dimensions;
		notifyChanged();
	}

	bool AtmosphereBackground::doInitialise( castor3d::RenderDevice const & device )
	{
		auto data = device.graphicsData();
		auto & resources = getScene().getResources();
		m_textureId = castor3d::Texture{ device
			, resources
			, "Dummy"
			, 0u
			, { SkyTexSize, SkyTexSize, 1u }
			, 1u
			, 1u
			, VK_FORMAT_B10G11R11_UFLOAT_PACK32
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST };
		m_transmittance.create();
		m_multiScatter.create();
		m_textureId.create();
		m_texture = std::make_shared< castor3d::TextureLayout >( device.renderSystem
			, m_textureId.image
			, m_textureId.wholeViewId );
		m_hdr = true;
		m_srgb = false;
		m_timer.getElapsed();
		auto result = m_texture->initialise( device, *data );

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

	void AtmosphereBackground::doCpuUpdate( castor3d::CpuUpdater & updater )const
	{
		m_generateWorley = m_generateWorley && m_first;
		m_generatePerlinWorley = m_generatePerlinWorley && m_first;
		m_generateCurl = m_generateCurl && m_first;
		m_atmosphereChanged = m_first;
		m_weatherChanged = m_first;
		m_cloudsChanged = m_first;
		m_first = false;

		CU_Require( m_sunNode );
		CU_Require( m_planetNode );
		auto [sunDirection, planetPosition] = m_atmosphereUbo->cpuUpdate( m_atmosphereCfg, *m_sunNode, *m_planetNode );
		auto time = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );
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

	void AtmosphereBackground::doGpuUpdate( castor3d::GpuUpdater & updater )const
	{
	}

	void AtmosphereBackground::doAddPassBindings( crg::FramePass & pass
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		auto it = findCameraPass( targetImage );

		if ( it != m_cameraPasses.end() )
		{
			it->second->cameraUbo.createPassBinding( pass
				, index++ );
			m_atmosphereUbo->createPassBinding( pass
				, index++ );
			m_cloudsUbo->createPassBinding( pass
				, index++ );
			crg::SamplerDesc linearClampSampler{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR };
			pass.addSampledView( m_transmittance.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearClampSampler );
			pass.addSampledView( m_multiScatter.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearClampSampler );
			pass.addSampledView( it->second->skyView.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearClampSampler );
			pass.addSampledView( it->second->volume.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearClampSampler );
			pass.addSampledView( it->second->cloudsResult.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, linearClampSampler );
		}
	}

	void AtmosphereBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// CameraBuffer
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// AtmosphereBuffer
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// CloudsBuffer
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapTransmittance
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapMultiScatter
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapSkyView
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapVolume
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapClouds
	}

	void AtmosphereBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		auto it = findCameraPass( targetImage );

		if ( it != m_cameraPasses.end() )
		{
			descriptorWrites.push_back( it->second->cameraUbo.getDescriptorWrite( index++ ) );
			descriptorWrites.push_back( m_atmosphereUbo->getDescriptorWrite( index++ ) );
			descriptorWrites.push_back( m_cloudsUbo->getDescriptorWrite( index++ ) );
			castor3d::bindTexture( m_transmittance.sampledView
				, *m_transmittance.sampler
				, descriptorWrites
				, index );
			castor3d::bindTexture( m_multiScatter.sampledView
				, *m_multiScatter.sampler
				, descriptorWrites
				, index );
			castor3d::bindTexture( it->second->skyView.sampledView
				, *it->second->skyView.sampler
				, descriptorWrites
				, index );
			castor3d::bindTexture( it->second->volume.sampledView
				, *it->second->volume.sampler
				, descriptorWrites
				, index );
			castor3d::bindTexture( it->second->cloudsResult.sampledView
				, *it->second->volume.sampler
				, descriptorWrites
				, index );
		}
	}

	//************************************************************************************************
}

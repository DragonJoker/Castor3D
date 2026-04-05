#include "AtmosphereScattering/CloudsVolumePlugin.hpp"

#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/Shaders/GlslVolumeShaders.hpp>

namespace atmosphere_scattering
{
	//************************************************************************************************

	namespace details
	{
		static constexpr bool disablePassOptimisations = false;

		static uint32_t getNextBinding( uint32_t & binding )
		{
			auto result = binding;
			++binding;
			return result;
		}

		template< typename DataT, typename BufferT >
		static DataT createUniformBufferData( sdw::ShaderWriter & writer, uint32_t binding )
		{
			auto buffer = writer.declUniformBuffer<>( BufferT::Buffer, binding, 0u );
			auto result = buffer.template declMember< DataT >( BufferT::Data );
			buffer.end();
			return result;
		}
	}

	//************************************************************************************************

	VolumeCameraData::VolumeCameraData( c3d::RenderDevice const & device
		, crg::ResourcesCache & resources
		, c3d::Texture const * pdepthObj
		, c3d::Point2ui const & skyViewResolution
		, uint32_t volumeResolution )
		: skyView{ device
			, resources
			, cuT( "AtmosphereSkyView" )
			, { c3d::ImageCreateFlags::eNone
			, { skyViewResolution->x, skyViewResolution->y, 1u }, 1u, 1u
				, c3d::PixelFormat::eB10G11R11_UFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
				, { { .mipFilter = c3d::MipmapMode::eNearest } } }
		, volume{ device
			, resources
			, cuT( "AtmosphereVolume" )
			, { c3d::ImageCreateFlags::eNone
			, { volumeResolution, volumeResolution, volumeResolution }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
				, { { .mipFilter = c3d::MipmapMode::eNearest } } }
		, cameraUbo{ device }
		, depthObj{ pdepthObj }
		, m_device{ device }
		, m_position{ m_camAtmoChanged}
		, m_orientation{ m_camAtmoChanged}
	{
		skyView.create();
		volume.create();
	}

	VolumeCameraData::~VolumeCameraData()
	{
		skyView.destroy();
		volume.destroy();
	}

	void VolumeCameraData::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( cuT( "Atmosphere SkyView" )
			, skyView.getSampledViewId()
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
	}

	void VolumeCameraData::cpuUpdate( c3d::CpuUpdater & updater
		, bool atmosphereChanged
		, c3d::Point3f const & sunDirection
		, c3d::Vector3f const & planetPosition )
	{
		m_camAtmoChanged = atmosphereChanged;

		auto node = updater.camera->getParent();
		auto const & engine = *node->getScene()->getEngine();
		auto position = c3d::Vector3f::fromUnit( node->getDerivedPosition(), engine.getLengthUnit() ) - planetPosition;
		auto orientation = node->getDerivedOrientation();
		auto kmPosition = position.kilometres();
		m_position = kmPosition;
		m_orientation = orientation;
		cameraUbo.cpuUpdate( updater.renderSize, *updater.camera, updater.isSafeBanded
			, m_orientation, m_position, planetPosition );
	}

	void VolumeCameraData::registerPasses( crg::FramePassGroup & graph
		, VolumeData const & volumeData )
	{
		m_skyViewPass = c3d::makeRawUnique< AtmosphereSkyViewPass >( graph
			, m_device
			, cameraUbo
			, volumeData.atmosphereUbo
			, volumeData.transmittance
			, skyView
			, 0u
			, m_camAtmoChanged );
		m_volumePass = c3d::makeRawUnique< AtmosphereVolumePass >( graph
			, m_device
			, cameraUbo
			, volumeData.atmosphereUbo
			, volumeData.transmittance
			, volume
			, 0u
			, m_camAtmoChanged );
	}

	//************************************************************************************************

	VolumeData::VolumeData( c3d::Scene & scene )
		: atmosphereUbo{ getEngine( scene ).getRenderSystem()->getRenderDevice() }
		, cloudsUbo{ getEngine( scene ).getRenderSystem()->getRenderDevice() }
		, weatherUbo{ getEngine( scene ).getRenderSystem()->getRenderDevice() }
		, m_scene{ &scene }
		, m_atmosphereCheckedConfig{ m_atmosphereChanged }
		, m_weatherCheckedConfig{ m_weatherChanged }
		, m_cloudsCheckedConfig{ m_cloudsChanged }
		, m_sunDirection{ m_cloudsChanged }
		, m_planetPosition{ m_cloudsChanged }
		, m_mieAbsorption{ m_cloudsChanged }
	{
	}

	VolumeData::~VolumeData()
	{
		worley.destroy();
		perlinWorley.destroy();
		curl.destroy();
		weather.destroy();
		transmittance.destroy();
		multiScatter.destroy();
	}

	void VolumeData::accept( c3d::ConfigurationVisitorBase & visitor )
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
			, transmittance
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Atmosphere Multiscatter" )
			, multiScatter
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );

		for ( auto const & [_, data] : cameraData )
			data->accept( visitor );

		visitor.visit( cuT( "Weather Result" )
			, weather
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
		visitor.visit( cuT( "Curl Noise" )
			, curl
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );

		uint32_t index{};
		for ( auto & layerViews : worley )
		{
			visitor.visit( cuT( "Worley Noise Slice " ) + c3d::string::toString( index )
				, layerViews.sampledViewId
				, c3d::ImageLayout::eShaderReadOnly
				, c3d::TextureFactors::tex3DSlice( index ).invert( true ) );
			++index;
		}

		index = {};
		for ( auto & layerViews : perlinWorley )
		{
			visitor.visit( cuT( "Perlin Worley Noise Slice " ) + c3d::string::toString( index )
				, layerViews.sampledViewId
				, c3d::ImageLayout::eShaderReadOnly
				, c3d::TextureFactors::tex3DSlice( index ).invert( true ) );
			++index;
		}
	}

	void VolumeData::initialise( c3d::SceneNode const * sunNode
		, c3d::SceneNode const * planetNode )
	{
		transmittance.create();
		multiScatter.create();
		m_timer.getElapsed();
		m_time = 0.0f;
		atmosphereUbo.cpuUpdate( m_atmosphereCfg, *sunNode, *planetNode );
		weatherUbo.cpuUpdate( m_weatherCfg );
		cloudsUbo.cpuUpdate( m_cloudsCfg, m_time );
	}

	void VolumeData::cpuUpdate( c3d::CpuUpdater & updater
		, c3d::SceneNode const * sunNode
		, c3d::SceneNode const * planetNode )
	{
		m_generateWorley = m_generateWorley && m_first;
		m_generatePerlinWorley = m_generatePerlinWorley && m_first;
		m_generateCurl = m_generateCurl && m_first;

		if constexpr ( details::disablePassOptimisations )
			m_first = true;

		m_atmosphereChanged = m_first;
		m_weatherChanged = m_first;
		m_cloudsChanged = m_first;
		m_first = false;

		if ( planetNode && sunNode )
		{
			auto [sunDirection, planetPosition, mieAbsorption] = atmosphereUbo.cpuUpdate( m_atmosphereCfg, *sunNode, *planetNode );
			m_sunDirection = sunDirection;
			m_planetPosition = planetPosition.kilometres();
			m_mieAbsorption = mieAbsorption;
			auto time = updater.tslf > 0_ms
				? updater.tslf
				: std::chrono::duration_cast< c3d::Milliseconds >( m_timer.getElapsed() );
			m_time += float( time.count() ) / 1000.0f;
			weatherUbo.cpuUpdate( m_weatherCfg );
			cloudsUbo.cpuUpdate( m_cloudsCfg, m_time );

			m_atmosphereCheckedConfig = m_atmosphereCfg;
			m_weatherCheckedConfig = m_weatherCfg;
			m_cloudsCheckedConfig = m_cloudsCfg;

			if ( auto it = cameraData.find( updater.camera );
				it != cameraData.end() )
				it->second->cpuUpdate( updater, m_atmosphereChanged, sunDirection, planetPosition );
		}
	}

	void VolumeData::registerPasses( crg::FramePassGroup & graph )
	{
		auto & device = getEngine( *m_scene ).getRenderSystem()->getRenderDevice();
		m_worleyPass = c3d::makeRawUnique< CloudsWorleyPass >( graph
			, device
			, worley
			, m_generateWorley );
		m_perlinWorleyPass = c3d::makeRawUnique< CloudsPerlinPass >( graph
			, device
			, perlinWorley
			, m_generatePerlinWorley );
		m_curlPass = c3d::makeRawUnique< CloudsCurlPass >( graph
			, device
			, curl
			, m_generateCurl );
		m_weatherPass = c3d::makeRawUnique< CloudsWeatherPass >( graph
			, device
			, weatherUbo
			, weather
			, m_weatherChanged );
		m_transmittancePass = c3d::makeRawUnique< AtmosphereTransmittancePass >( graph
			, device
			, atmosphereUbo
			, transmittance
			, m_atmosphereChanged );
		m_multiScatteringPass = c3d::makeRawUnique< AtmosphereMultiScatteringPass >( graph
			, device
			, atmosphereUbo
			, transmittance
			, multiScatter
			, m_atmosphereChanged );
	}

	VolumeCameraData * VolumeData::registerCamera( c3d::Camera const & camera
		, c3d::Texture const * depthObj )
	{
		auto [it, inserted] = cameraData.try_emplace( &camera );
		if ( inserted )
			it->second = c3d::makeRawUnique< VolumeCameraData >( getEngine( *m_scene ).getRenderSystem()->getRenderDevice()
				, m_scene->getResources(), depthObj
				, m_skyViewResolution, m_volumeResolution );
		return it->second.get();
	}

	void VolumeData::unregisterCamera( c3d::Camera const & camera )noexcept
	{
		if ( auto it = cameraData.find( &camera );
			it != cameraData.end() )
			cameraData.erase( it );
	}
	
	void VolumeData::loadWorley( uint32_t dimension )
	{
		auto & resources = m_scene->getResources();
		auto const & device = m_scene->getEngine()->getRenderSystem()->getRenderDevice();
		m_worleyResolution = dimension;
		worley = c3d::Texture{ device
			, resources
			, cuT( "WorleyNoise" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, dimension }, 1u
				, c3d::getMipLevels( c3d::Extent3D{ dimension, dimension, dimension }, c3d::PixelFormat::eR8G8B8A8_UNORM )
				, c3d::PixelFormat::eR8G8B8A8_UNORM
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eTransferSrc | c3d::ImageUsageFlags::eTransferDst | c3d::ImageUsageFlags::eStorage }
			, { { .addressMode = c3d::WrapMode::eRepeat } } };
	}

	void VolumeData::loadPerlinWorley( uint32_t dimension )
	{
		auto & resources = m_scene->getResources();
		auto const & device = m_scene->getEngine()->getRenderSystem()->getRenderDevice();
		m_perlinWorleyResolution = dimension;
		perlinWorley = c3d::Texture{ device
			, resources
			, cuT( "PerlinWorleyNoise" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, dimension }, 1u
				, c3d::getMipLevels( c3d::Extent3D{ dimension, dimension , dimension }, c3d::PixelFormat::eR8G8B8A8_UNORM )
				, c3d::PixelFormat::eR8G8B8A8_UNORM
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eTransferSrc | c3d::ImageUsageFlags::eTransferDst | c3d::ImageUsageFlags::eStorage }
			, { { .addressMode = c3d::WrapMode::eRepeat } } };
	}

	void VolumeData::loadCurl( uint32_t dimension )
	{
		auto & resources = m_scene->getResources();
		auto const & device = m_scene->getEngine()->getRenderSystem()->getRenderDevice();
		m_curlResolution = dimension;
		curl = c3d::Texture{ device
			, resources
			, cuT( "CurlNoise" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, 1u }, 1u, 1u
				, c3d::PixelFormat::eR8G8_UNORM
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eTransferSrc | c3d::ImageUsageFlags::eTransferDst | c3d::ImageUsageFlags::eStorage }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } };
	}

	void VolumeData::loadWeather( uint32_t dimension )
	{
		auto & resources = m_scene->getResources();
		auto const & device = m_scene->getEngine()->getRenderSystem()->getRenderDevice();
		m_weatherResolution = dimension;
		weather = c3d::Texture{ device
			, resources
			, cuT( "Weather" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, 1u }, 1u, 1u
				, c3d::PixelFormat::eR32G32_SFLOAT
				, c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eStorage | c3d::ImageUsageFlags::eColorAttachment }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } };
	}

	void VolumeData::loadTransmittance( c3d::Point2ui const & dimensions )
	{
		auto & resources = m_scene->getResources();
		auto const & device = m_scene->getEngine()->getRenderSystem()->getRenderDevice();
		transmittance = c3d::Texture{ device
			, resources
			, cuT( "Transmittance" )
			, { c3d::ImageCreateFlags::eNone
			, { dimensions->x, dimensions->y, 1u }, 1u, 1u
			, c3d::PixelFormat::eR16G16B16A16_SFLOAT
			, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
			, { { .mipFilter = c3d::MipmapMode::eNearest } } };
		transmittance.create();
	}

	void VolumeData::loadMultiScatter( uint32_t dimension )
	{
		auto & resources = m_scene->getResources();
		auto const & device = m_scene->getEngine()->getRenderSystem()->getRenderDevice();
		multiScatter = c3d::Texture{ device
			, resources
			, cuT( "MultiScatter" )
			, { c3d::ImageCreateFlags::eNone
				, { dimension, dimension, 1u }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled | c3d::ImageUsageFlags::eStorage }
			, { { .mipFilter = c3d::MipmapMode::eNearest } } };
		multiScatter.create();
	}

	void VolumeData::loadAtmosphereVolume( uint32_t dimension )
	{
		m_volumeResolution = dimension;
	}

	void VolumeData::loadSkyView( c3d::Point2ui const & dimensions )
	{
		m_skyViewResolution = dimensions;
	}

	//************************************************************************************************

	AtmosphereVolumeTraversal::AtmosphereVolumeTraversal( sdw::ShaderWriter & writer
		, c3ds::VolumeShaders const & volumeShaders
		, AtmosphereModel & atmosphere )
		: m_writer{ writer }
		, m_volumeShaders{ volumeShaders }
		, m_atmosphere{ atmosphere }
	{
	}

	void AtmosphereVolumeTraversal::fillType( sdw::type::BaseStruct & type )const
	{
		type.declMember( "atmosphereData", AtmosphereTraverseData::makeType( type.getTypesCache() ) );
	}

	void AtmosphereVolumeTraversal::fillInit( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		inits.emplace_back( sdw::makeAggrInit( AtmosphereTraverseData::makeType( type.getTypesCache() )
			, AtmosphereTraverseData::getZeroInit() ) );
	}

	void AtmosphereVolumeTraversal::registerVolumes( c3ds::Volumes & volumes )
	{
		m_volumeType = volumes.registerVolumeType( getTraversalFunc(), getStepFunc() );
	}

	void AtmosphereVolumeTraversal::initialise( sdw::Vec2 const & pixelCoord, c3ds::Ray & ray
		, c3ds::Volumes & volumes, c3ds::VolumesTraversalResult & result
		, sdw::Vec4 const & depthObj )
	{
		auto traverseData = result.getMember< AtmosphereTraverseData >( "atmosphereData" );
		auto atmRay = m_writer.declLocale( "atmRay"
			, c3ds::Ray{ ray.origin * m_atmosphere.settings.length.kilometres() - m_atmosphere.getPlanetPosition(), ray.direction } );
		auto range = m_writer.declLocale( "range"
			, listVolumes( atmRay, depthObj.b(), depthObj.g(), pixelCoord, volumes ) );
		traverseData.initialise( m_atmosphere.hasVariableSampleCount(), m_atmosphere.atmosphereData, atmRay, range );
		ray.origin = ( atmRay.origin + m_atmosphere.getPlanetPosition() ) / m_atmosphere.settings.length.kilometres();
	}

	void AtmosphereVolumeTraversal::finalise( c3ds::Ray const & ray, c3ds::VolumesTraversalResult & result )
	{
		auto traverseData = result.getMember< AtmosphereTraverseData >( "atmosphereData" );

		auto const globalLuminance = m_writer.declLocale< sdw::Vec3 >( "globalLuminance"
			, m_atmosphere.settings.illuminanceIsOne
				// When building the scattering factor, we assume light illuminance is 1 to compute a transfert function relative to identity illuminance of 1.
				// This make the scattering factor independent of the light. It is now only linked to the atmosphere properties.
				? vec3( 1.0_f )
				: m_atmosphere.getSunIlluminance() );

		// Mie and Raylegh phases
		auto wi = m_writer.declLocale( "wi", traverseData.sunDirection );
		auto wo = m_writer.declLocale( "wo", ray.direction );
		auto cosTheta = m_writer.declLocale( "cosTheta", dot( wi, wo ) );
		// negate cosTheta because due to worldDir being a "in" direction. 
		auto miePhaseValue = m_writer.declLocale< sdw::Float >( "miePhaseValue"
			, m_atmosphere.hgPhase( m_atmosphere.getMiePhaseFunctionG(), -cosTheta ) );
		auto rayleighPhaseValue = m_writer.declLocale< sdw::Float >( "rayleighPhaseValue"
			, m_atmosphere.rayleighPhase( cosTheta ) );

		result.inscatter += traverseData.computeScattering( rayleighPhaseValue, miePhaseValue, globalLuminance );
	}

	c3ds::VolumeTraversalFunc const & AtmosphereVolumeTraversal::getTraversalFunc()
	{
		if ( !m_traverse )
		{
			m_traverse = m_writer.implementFunction< sdw::Void >( "atm_traverseVolume"
				, [this]( c3ds::Volume const & volume
					, Ray const & ray
					, sdw::Float const & sample
					, sdw::Float const & t
					, sdw::Float const & dt
					, sdw::Vec3 pos
					, c3ds::VolumesTraversalResult result )
				{
					auto atmRay = m_writer.declLocale( "atmRay"
						, c3ds::Ray{ ray.origin * m_atmosphere.settings.length.kilometres() - m_atmosphere.getPlanetPosition(), ray.direction } );
					pos = atmRay.step( t );
					auto traverseData = result.getMember< AtmosphereTraverseData >( "atmosphereData" );

					auto rayToSun = m_writer.declLocale( "rayToSun", Ray{ pos, traverseData.sunDirection } );
					auto medium = m_writer.declLocale( "medium", m_atmosphere.sampleMediumRGB( rayToSun.origin ) );
					auto sampleOpticalDepth = m_writer.declLocale( "sampleOpticalDepth", medium.extinction() * dt );
					auto sampleTransmittance = m_writer.declLocale( "sampleTransmittance", exp( -sampleOpticalDepth ) );

					auto pHeight = m_writer.declLocale( "pHeight", length( rayToSun.origin ) );
					auto upVector = m_writer.declLocale( "upVector", rayToSun.origin / pHeight );
					auto sunZenithCosAngle = m_writer.declLocale( "sunZenithCosAngle", dot( rayToSun.direction, upVector ) );
					auto trUv = m_writer.declLocale( "trUv"
						, m_atmosphere.lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle ) );
					auto transmittanceToSun = m_writer.declLocale( "transmittanceToSun"
						, ( m_atmosphere.transmittanceTexture
							? m_atmosphere.transmittanceTexture->lod( trUv, 0.0_f ).rgb()
							: vec3( 0.0_f ) ) );

					auto planetO = m_writer.declLocale( "planetO", vec3( 0.0_f, 0.0f, 0.0f ) );
					auto planetShadow = m_writer.declLocale( "planetShadow", m_atmosphere.getPlanetShadow( rayToSun, planetO, upVector ) );
					// Dual scattering for multi scattering
					auto multiScatteredLuminance = m_writer.declLocale( "multiScatteredLuminance"
						, ( m_atmosphere.hasMultiscattering()
							? m_atmosphere.getMultipleScattering( pHeight, sunZenithCosAngle )
							: vec3( 0.0_f ) ) );

					// See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
					traverseData.rayleighSingleScatter += planetShadow * m_atmosphere.integrateInscatter( transmittanceToSun * medium.scatteringRay()
						, result.transmittance, sampleTransmittance, medium.extinction() );
					traverseData.mieSingleScatter += planetShadow * m_atmosphere.integrateInscatter( transmittanceToSun * medium.scatteringMie()
						, result.transmittance, sampleTransmittance, medium.extinction() );
					traverseData.rayMieMultiScatter += ( m_atmosphere.hasMultiscattering()
						? m_atmosphere.integrateInscatter( multiScatteredLuminance * medium.scattering()
							, result.transmittance, sampleTransmittance, medium.extinction() )
						: vec3( 0.0_f ) );
					result.transmittance *= sampleTransmittance;
				}
				, c3ds::InVolume{ m_writer, "volume" }
				, c3ds::InRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InFloat{ m_writer, "t" }
				, sdw::InFloat{ m_writer, "dt" }
				, sdw::InVec3{ m_writer, "pos" }
				, c3ds::InOutVolumesTraversalResult{ m_writer, "result", m_volumeShaders } );
		}

		return m_traverse;
	}

	c3ds::VolumeStepFunc const & AtmosphereVolumeTraversal::getStepFunc()
	{
		if ( !m_step )
		{
			m_step = m_writer.implementFunction< sdw::Void >( "atm_stepInVolume"
				, [this]( c3ds::Volume const & volume
					, c3ds::Ray const & ray
					, sdw::Float const & sample
					, sdw::Float t
					, sdw::Float dt
					, c3ds::VolumesTraversalResult const & traversal )
				{
					auto sampleSegmentT = 0.3_f;
					auto result = m_writer.declLocale( "result", 0.0_f );
					auto traverseData = traversal.getMember< AtmosphereTraverseData >( "atmosphereData" );

					if ( m_atmosphere.hasVariableSampleCount() )
					{
						// More expensive but artifact free
						auto t0 = m_writer.declLocale( "t0", ( sample ) / traverseData.sampleCountFloor );
						auto t1 = m_writer.declLocale( "t1", ( sample + 1.0_f ) / traverseData.sampleCountFloor );
						// Non linear distribution of sample within the range.
						t0 = t0 * t0;
						t1 = t1 * t1;
						// Make t0 and t1 world space distances.
						t0 = traverseData.tMaxFloor * t0;

						sdwIF( m_writer, t1 > 1.0_f )
						{
							dt = volume.end - t0;
							//	t1 = tMaxFloor;	// this reveal depth slices
						}
						sdwELSE
						{
							dt = sdw::fma( traverseData.tMaxFloor, t1, -t0 );
						}
						sdwFI

						//t = t0 + (t1 - t0) * (whangHashNoise(pixPos.x, pixPos.y, gFrameId * 1920 * 1080)); // With dithering required to hide some sampling artefact relying on TAA later? This may even allow volumetric shadow?
						t = sdw::fma( dt, sampleSegmentT, t0 );
					}
					else
					{
						// Exact difference, important for accuracy of multiple scattering
						auto newT = m_writer.declLocale( "newT", volume.end * ( sample + sampleSegmentT ) / traverseData.sampleCount );
						dt = newT - t;
						t = newT;
					}
				}
				, c3ds::InVolume{ m_writer, "volume" }
				, c3ds::InOutRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InOutFloat{ m_writer, "t" }
				, sdw::InOutFloat{ m_writer, "dt" }
				, c3ds::InOutVolumesTraversalResult{ m_writer, "traversal", m_volumeShaders } );
		}

		return m_step;
	}

	sdw::RetFloat AtmosphereVolumeTraversal::getSamplesCount( sdw::Float const & range )const
	{
		return mix( m_atmosphere.getRayMarchMinSPP()
			, m_atmosphere.getRayMarchMaxSPP()
			, clamp( range * 0.01_f, 0.0_f, 1.0_f ) );
	}

	sdw::RetFloat AtmosphereVolumeTraversal::listVolumes( c3ds::Ray const & pray
		, sdw::Float const & pobjectId
		, sdw::Float const & plinearDepth
		, sdw::Vec2 const & ppixPos
		, c3ds::Volumes & pvolumes )
	{
		if ( !m_listVolumes )
		{
			m_listVolumes = m_writer.implementFunction< sdw::Float >( "atm_listVolumes"
				, [this]( Ray const & ray
					, sdw::Float const & objectId
					, sdw::Float const & linearDepth
					, sdw::Vec2 const & pixPos
					, sdw::Array< c3ds::Volume > volumes
					, sdw::UInt volumeCount )
				{
					auto tMax = m_writer.declLocale( "tMax", 0.0_f );
					auto movedToTop = m_writer.declLocale( "movedToTop", m_atmosphere.moveToTopAtmosphere( ray ) );
					//m_atmosphere.moveToBottomAtmosphere( ray, objectId, linearDepth );

					sdwIF( m_writer, movedToTop )
					{
						// Compute next intersection with atmosphere or ground
						auto planetO = m_writer.declLocale( "planetO", vec3( 0.0_f, 0.0f, 0.0f ) );
						auto tBottom = m_writer.declLocale( "tBottom", m_atmosphere.raySphereIntersectNearest( ray, planetO, m_atmosphere.getPlanetRadius() ) );
						auto tTop = m_writer.declLocale( "tTop", m_atmosphere.raySphereIntersectNearest( ray, planetO, m_atmosphere.getAtmosphereRadius() ) );

						sdwIF( m_writer, tBottom.valid() || tTop.valid() )
						{
							sdwIF( m_writer, !tBottom.valid() )
							{
								tMax = tTop.t();
							}
							sdwELSE
							{
								sdwIF( m_writer, tTop.t() > 0.0_f )
								{
									tMax = min( tTop.t(), tBottom.t() );
								}
								sdwFI
							}
							sdwFI

							sdwIF( m_writer, linearDepth > 0.0f )
							{
								auto targetExtent = sdw::vec2( float( m_atmosphere.transmittanceExtent.width ), float( m_atmosphere.transmittanceExtent.height ) );
								auto depthBufferWorldPos = m_writer.declLocale( "depthBufferWorldPos"
									, m_atmosphere.getWorldPos( linearDepth, pixPos, targetExtent ) );
								auto tDepth = m_writer.declLocale( "tDepth"
									, length( depthBufferWorldPos - ray.origin ) ); // apply planet offset to go back to origin as top of planet mode. 

								sdwIF( m_writer, tDepth < tMax )
								{
									tMax = tDepth;
								}
								sdwFI
							}
							sdwFI

							tMax = min( tMax, 9000000.0_f );
							auto samples = m_writer.declLocale( "samples", getSamplesCount( tMax ) );
							volumes[volumeCount] = c3ds::Volume{ volumeCount, sdw::UInt{ getVolumeType() }
								, 0.0_f, tMax
								, tMax / m_writer.cast< sdw::Float >( samples )
								, samples };
							tMax = volumes[volumeCount++].end;
						}
						sdwFI
					}
					sdwFI

					m_writer.returnStmt( tMax );
				}
				, c3ds::InOutRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "objectId" }
				, sdw::InFloat{ m_writer, "linearDepth" }
				, sdw::InVec2{ m_writer, "pixPos" }
				, sdw::InOutParam< sdw::Array< c3ds::Volume > >{ m_writer, "volumes", c3ds::MaxVolumeCount }
				, sdw::InOutUInt{ m_writer, "volumeCount" } );
		}

		return m_listVolumes( pray, pobjectId, plinearDepth, ppixPos, pvolumes.getVolumes(), pvolumes.getCount() );
	}

	//************************************************************************************************

	CloudsVolumeTraversal::CloudsVolumeTraversal( sdw::ShaderWriter & writer
		, c3ds::VolumeShaders const & volumeShaders
		, CloudsModel & clouds )
		: m_writer{ writer }
		, m_volumeShaders{ volumeShaders }
		, m_clouds{ clouds }
	{
	}

	void CloudsVolumeTraversal::fillType( sdw::type::BaseStruct & type )const
	{
		type.declMember( "cloudsData", CloudsTraverseData::makeType( type.getTypesCache() ) );
	}

	void CloudsVolumeTraversal::fillInit( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		inits.emplace_back( sdw::makeAggrInit( CloudsTraverseData::makeType( type.getTypesCache() )
			, CloudsTraverseData::getZeroInit() ) );
	}

	void CloudsVolumeTraversal::registerVolumes( c3ds::Volumes & volumes )
	{
		m_volumeType = volumes.registerVolumeType( getTraversalFunc(), getStepFunc() );
	}

	void CloudsVolumeTraversal::initialise( sdw::Vec2 const & pixelCoord, c3ds::Ray & ray
		, c3ds::Volumes & volumes, c3ds::VolumesTraversalResult & result
		, sdw::Vec4 const & depthObj )
	{
		sdwIF( m_writer, m_clouds.getCloudsCoverage() > 0.0_f )
		{
			auto cldRay = m_writer.declLocale( "cldRay"
				, c3ds::Ray{ ray.origin * m_clouds.getAtmosphere().settings.length.kilometres() - m_clouds.getAtmosphere().getPlanetPosition(), ray.direction } );
			listVolumes( cldRay, depthObj.b(), depthObj.g(), pixelCoord, volumes );
			auto cloudsTraversal = result.getMember< CloudsTraverseData >( "cloudsData" );
			cloudsTraversal.initialise( m_clouds.getAtmosphere(), m_clouds.getScattering(), ivec2( pixelCoord ), cldRay );
		}
		sdwFI
	}

	c3ds::VolumeTraversalFunc const & CloudsVolumeTraversal::getTraversalFunc()
	{
		if ( !m_traverse )
		{
			m_traverse = m_writer.implementFunction< sdw::Void >( "clouds_traverseVolume"
				, [this]( c3ds::Volume const & volume
					, c3ds::Ray const & ray
					, sdw::Float const & sample
					, sdw::Float const & t
					, sdw::Float const & dt
					, sdw::Vec3 const & pos
					, c3ds::VolumesTraversalResult result )
				{
					auto & atmosphere = m_clouds.getAtmosphere();
					auto & scattering = m_clouds.getScattering();
					auto & utils = m_clouds.getUtils();

					auto traverseData = result.getMember< CloudsTraverseData >( "cloudsData" );

					auto relativeHeight = m_writer.declLocale( "relativeHeight"
						, m_clouds.getHeightFraction( traverseData.currentPos ) );
					auto currentShadow = m_writer.declLocale( "currentShadow"
						, atmosphere.getPlanetShadow( vec3( 0.0_f ), traverseData.currentPos ) );
					traverseData.planetShadow += currentShadow / volume.sampleCount;

					sdwIF( m_writer, relativeHeight >= 0.0_f && relativeHeight <= 1.0_f
						&& result.transmittanceAboveThreshold )
					{
						auto volumeDensity = m_writer.declLocale( "volumeDensity"
							, m_clouds.sampleCloudDensity( traverseData.currentPos, 1_b, relativeHeight, 0.0_f ) );
						volumeDensity *= m_clouds.getCloudsDensity();

						sdwIF( m_writer, volumeDensity > 0.0_f )
						{
							traverseData.accumDensity += volumeDensity;
							auto lightEnergy = m_writer.declLocale( "lightEnergy"
								, m_clouds.raymarchToLight( ray.direction
									, traverseData.currentPos
									, volume.stepSize
									, atmosphere.getSunDirection() ) );
							auto lighting = m_writer.declLocale( "lighting"
								, vec3( lightEnergy ) );

							// ambient lighting
							auto ambientLight = m_writer.declLocale( "ambientLight"
								, mix( m_clouds.getCloudsBottomColour(), m_clouds.getCloudsTopColour(), vec3( relativeHeight ) ) );
							lighting += ambientLight;

							auto stepTransmittance = m_writer.declLocale( "stepTransmittance"
								, exp( -volumeDensity * dt ) );
							auto sunRadiance = m_writer.declLocale( "sunRadiance"
								, scattering.getSunRadiance( traverseData.currentPos, atmosphere.getSunDirection() ) );
							auto volumeLuminance = m_writer.declLocale( "volumeLuminance"
								, lighting * currentShadow * sunRadiance );

							// add sun glare to clouds
							auto sunIntensity = m_writer.declLocale( "sunIntensity"
								, c3ds::saturate( dot( atmosphere.getSunDirection(), ray.direction ) ) );
							auto sunGlare = m_writer.declLocale( "sunGlare"
								, sunRadiance * pow( sunIntensity, 256.0_f ) );
							auto sunGlareIntensity = m_writer.declLocale( "sunGlareIntensity"
								, ( 1.0_f - volumeDensity ) );
							auto minGlare = 0.5_f;
							sunGlareIntensity = utils.remap( sunGlareIntensity, 0.0_f, 1.0_f, minGlare, 1.0_f );
							volumeLuminance += sunGlare * ( sunGlareIntensity - minGlare );

							//// Fade out clouds into the horizon.
							//auto cubeMapEndPos = m_writer.declLocale( "cubeMapEndPos"
							//	, atmosphere.raySphereintersectSkyMap( ray.direction, 0.5_f ).point() );
							//volumeLuminance = currentShadow * mix( vec3( 1.0_f )
							//	, volumeLuminance
							//	, vec3( pow( max( length( cubeMapEndPos ) + 0.1_f, 0.0_f ), 0.2_f ) ) );

							result.inscatter += m_clouds.integrateInscatter( volumeDensity * volumeLuminance, traverseData.volumesTransmittance, stepTransmittance, volumeDensity );
							result.transmittance *= stepTransmittance;
							result.transmittanceAboveThreshold = traverseData.volumesTransmittance >= 0.01f && traverseData.accumDensity < 1.0_f;
							traverseData.volumesTransmittance *= stepTransmittance;
						}
						sdwFI
					}
					sdwFI

					traverseData.currentPos += traverseData.stepVector;
				}
				, c3ds::InVolume{ m_writer, "volume" }
				, c3ds::InRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InFloat{ m_writer, "t" }
				, sdw::InFloat{ m_writer, "dt" }
				, sdw::InVec3{ m_writer, "pos" }
				, c3ds::InOutVolumesTraversalResult{ m_writer, "result", m_volumeShaders } );
		}

		return m_traverse;
	}

	c3ds::VolumeStepFunc const & CloudsVolumeTraversal::getStepFunc()
	{
		if ( !m_step )
		{
			m_step = m_writer.implementFunction< sdw::Void >( "clouds_stepInVolume"
				, [this]( c3ds::Volume const & volume
					, c3ds::Ray ray
					, sdw::Float const & sample
					, sdw::Float t
					, sdw::Float dt
					, c3ds::VolumesTraversalResult const & traversal )
				{
					auto traverseData = traversal.getMember< CloudsTraverseData >( "cloudsData" );

					sdwIF( m_writer, sample == 0.0_f )
					{
						auto cldRay = m_writer.declLocale( "cldRay"
							, c3ds::Ray{ ray.origin * m_clouds.getAtmosphere().settings.length.kilometres() - m_clouds.getAtmosphere().getPlanetPosition(), ray.direction } );
						traverseData.currentPos = cldRay.step( volume.begin );
						traverseData.fogAmount = m_clouds.computeFogAmount( traverseData.currentPos, cldRay.origin, 2.0_f
							, traversal.getMember< AtmosphereTraverseData >( "atmosphereData" ).viewHeight );

						// Dithering on the starting ray position to reduce banding artifacts
						auto bayerFactor = 1.0_f / 16.0_f;
						auto bayerFilter = m_writer.declConstantArray( "bayerFilter"
							, c3d::Vector< sdw::Float >{ 0.0_f * bayerFactor, 8.0_f * bayerFactor, 2.0_f * bayerFactor, 10.0_f * bayerFactor
								, 12.0_f * bayerFactor, 4.0_f * bayerFactor, 14.0_f * bayerFactor, 6.0_f * bayerFactor
								, 3.0_f * bayerFactor, 11.0_f * bayerFactor, 1.0_f * bayerFactor, 9.0_f * bayerFactor
								, 15.0_f * bayerFactor, 7.0_f * bayerFactor, 13.0_f * bayerFactor, 5.0_f * bayerFactor } );
						traverseData.stepVector = ( cldRay.step( volume.end ) - traverseData.currentPos ) / ( volume.sampleCount - 1.0_f );
						// Dithering on the starting ray position to reduce banding artifacts
						auto a = traverseData.pixelCoord.x() % 4_i;
						auto b = traverseData.pixelCoord.y() % 4_i;
						traverseData.currentPos += traverseData.stepVector * bayerFilter[a * 4 + b];
					}
					sdwFI

					dt = volume.stepSize;
					t += dt;
				}
				, c3ds::InVolume{ m_writer, "volume" }
				, c3ds::InOutRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InOutFloat{ m_writer, "t" }
				, sdw::InOutFloat{ m_writer, "dt" }
				, c3ds::InOutVolumesTraversalResult{ m_writer, "traversal", m_volumeShaders } );
		}

		return m_step;
	}

	sdw::RetFloat CloudsVolumeTraversal::getSamplesCount( sdw::Float const & range )const
	{
		return ceil( mix( 48.0_f
			, 96.0_f
			, clamp( range / m_clouds.cloudsThickness, 0.0_f, 1.0_f ) ) );
	}

	void CloudsVolumeTraversal::listVolumes( c3ds::Ray const & pray
		, sdw::Float const & pobjectId
		, sdw::Float const & plinearDepth
		, sdw::Vec2 const & ppixCoord
		, c3ds::Volumes & pvolumes )
	{
		if ( !m_listVolumes )
		{
			m_listVolumes = m_writer.implementFunction< sdw::Void >( "clouds_listVolumes"
				, [this]( Ray ray
					, sdw::Float const & objectId
					, sdw::Float const & linearDepth
					, sdw::Vec2 const & pixCoord
					, sdw::Array< c3ds::Volume > volumes
					, sdw::UInt volumeCount )
				{
					auto & atmosphere = m_clouds.getAtmosphere();
					auto addVolume = [this, &volumes, &volumeCount]( uint32_t type, sdw::Float const & begin, sdw::Float const & end, sdw::Float const & samples )
						{
							volumes[volumeCount] = c3ds::Volume{ volumeCount, sdw::UInt{ type }
								, begin, end
								, ( end - begin ) / m_writer.cast< sdw::Float >( samples )
								, samples };
							return volumes[volumeCount++].end;
						};

					auto clampOuter = m_writer.declLocale( "clampOuter"
						, 0_b );
					auto interGround = m_writer.declLocale( "interGround"
						, atmosphere.raySphereIntersectNearest( ray, atmosphere.getPlanetRadius() ) );
					auto wasHittingGround = m_writer.declLocale( "wasHittingGround"
						, interGround.valid() );

					sdwIF( m_writer, objectId != 0.0_f
						&& linearDepth != -1.0_f
						&& ( ( !interGround.valid() ) || linearDepth < interGround.t() ) )
					{
						interGround.t() = linearDepth;
						interGround.point() = ray.step( linearDepth );
						interGround.valid() = 1_b;
						clampOuter = 1_b;
					}
					sdwFI

					auto interInnerN = m_writer.declLocale( "interInnerN", Intersection{ m_writer } );
					auto interInnerF = m_writer.declLocale( "interInnerF", Intersection{ m_writer } );
					auto interInnerCount = m_writer.declLocale( "interInnerCount"
						, atmosphere.raySphereIntersect( ray, m_clouds.cloudsInnerRadius, interGround, 0_b, interInnerN, interInnerF ) );

					auto interOuterN = m_writer.declLocale( "interOuterN", Intersection{ m_writer } );
					auto interOuterF = m_writer.declLocale( "interOuterF", Intersection{ m_writer } );
					auto interOuterCount = m_writer.declLocale( "interOuterCount"
						, atmosphere.raySphereIntersect( ray, m_clouds.cloudsOuterRadius, interGround, clampOuter, interOuterN, interOuterF ) );

					auto interAtmosphere = m_writer.declLocale( "interAtmosphere"
						, atmosphere.raySphereIntersectNearest( ray, atmosphere.getAtmosphereRadius() ) );

					auto viewHeight = m_writer.declLocale( "viewHeight"
						, length( ray.origin ) );
					auto begin = m_writer.declLocale( "begin"
						, 0.0_f );
					auto end = m_writer.declLocale( "end"
						, 0.0_f );

					sdwIF( m_writer, interInnerCount != 0_i || interOuterCount != 0_i )
					{
						// Compute raymarching starting and ending point
						sdwIF( m_writer, viewHeight <= m_clouds.cloudsInnerRadius )
						{
							// Ray starts below clouds layer, in the atmosphere
							begin = m_writer.ternary( wasHittingGround, interAtmosphere.t(), interInnerN.t() );

							sdwIF( m_writer, !wasHittingGround )
							{
								// Ray then crosses the clouds layer, between near inner and near outer
								end = interOuterN.t();
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// And finishes in the atmosphere, not added here
							}
							sdwFI
						}
						sdwELSEIF( viewHeight > m_clouds.cloudsInnerRadius
							&& viewHeight < m_clouds.cloudsOuterRadius )
						{
							// Ray starts inside clouds layer, three possibilities:
							sdwIF( m_writer, interGround.valid() )
							{
								// It goes through the first clouds layer.
								end = m_writer.ternary( interInnerN.valid(), interInnerN.t(), interOuterN.t() );
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then hits the ground, through the atmosphere, not added here
							}
							sdwELSEIF( interInnerCount > 1_i )
							{
								// It goes through the first clouds layer.
								end = interInnerN.t();
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then the atmosphere between layers, not added here
								begin = interInnerF.t();

								// The second clouds layer
								end = interOuterN.t(); // near outer because only one intersection with outer is available, stored in near.
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then finishes in the atmosphere, not added here
							}
							sdwELSE
							{
								// It goes through the first clouds layer.
								end = interOuterN.t(); // near outer because only one intersection with outer is available, stored in near.
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then finishes in the atmosphere, not added here
							}
							sdwFI
						}
						sdwELSE
						{
							// Ray starts over clouds layer, in the atmosphere
							begin = interOuterN.t();

							sdwIF( m_writer, interGround.valid() )
							{
								// It crosses the clouds layer
								end = interInnerN.t();
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then hits the ground, through the atmosphere, not added here
							}
							sdwELSEIF( interInnerCount > 1_i && interOuterCount > 1_i )
							{
								// It crosses the first clouds layer.
								end = interInnerN.t();
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then the atmosphere between layers, not added here
								begin = interInnerF.t();

								// The second clouds layer
								end = interOuterF.t();
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then finishes in the atmosphere, not added here
							}
							sdwELSE
							{
								// Ray goes through clouds layer, remaining inside clouds layer.
								end = interOuterF.t();
								begin = addVolume( getVolumeType(), begin, end, getSamplesCount( end - begin ) );

								// Then finishes in the atmosphere, not added here
							}
							sdwFI
						}
						sdwFI
					}
					sdwFI
				}
				, c3ds::InOutRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "objectId" }
				, sdw::InFloat{ m_writer, "linearDepth" }
				, sdw::InVec2{ m_writer, "pixCoord" }
				, sdw::InOutParam< sdw::Array< c3ds::Volume > >{ m_writer, "volumes", c3ds::MaxVolumeCount }
				, sdw::InOutUInt{ m_writer, "volumeCount" } );
		}

		m_listVolumes( pray, pobjectId, plinearDepth, ppixCoord, pvolumes.getVolumes(), pvolumes.getCount() );
	}

	//************************************************************************************************

	c3d::String const CloudsVolumePlugin::TypeName = cuT( "c3d.atmosphere" );

	CloudsVolumePlugin::Shader::Shader( sdw::ShaderWriter & writer
		, c3ds::VolumeShaders const & volumeShaders
		, c3d::Extent2D targetExtent
		, bool hasDepth
		, uint32_t & binding )
		: VolumeComponentShader{ writer, volumeShaders }
		, m_depthMap{ m_writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "depthMap", details::getNextBinding( binding ), 0u, hasDepth ) }
		, m_atmosphereData{ details::createUniformBufferData< AtmosphereData, AtmosphereScatteringUbo >( m_writer, details::getNextBinding( binding ) ) }
		, m_cloudsData{ details::createUniformBufferData< CloudsData, CloudsUbo >( m_writer, details::getNextBinding( binding ) ) }
		, m_cameraData{ details::createUniformBufferData< CameraData, CameraUbo >( m_writer, details::getNextBinding( binding ) ) }
		, m_atmosphere{ m_writer, m_atmosphereData
			, AtmosphereModel::Settings{ c3d::Length::fromUnit( 1.0f, volumeShaders.getEngine().getLengthUnit() ) }
				.setCameraData( &m_cameraData )
				.setVariableSampleCount( true )
				.setMieRayPhase( true )
				.setMultiScatApprox( true )
			, c3d::move( targetExtent ) }
		, m_scattering{ m_writer, m_atmosphere
			, ScatteringModel::Settings{}
				.setNeedsMultiscatter( true )
				.setBloomSunDisk( true )
			, binding, 0u }
		, m_utils{ m_writer }
		, m_clouds{ m_writer, m_utils, m_atmosphere, m_scattering
			, m_cloudsData, binding, 0u }
		, m_atmosphereTraversal{ m_writer, m_volumeShaders, m_atmosphere }
		, m_cloudsTraversal{ m_writer, m_volumeShaders, m_clouds }
	{
	}

	void CloudsVolumePlugin::Shader::fillType( sdw::type::BaseStruct & type )const
	{
		m_atmosphereTraversal.fillType( type );
		m_cloudsTraversal.fillType( type );
	}

	void CloudsVolumePlugin::Shader::fillInit( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		m_atmosphereTraversal.fillInit( type, inits );
		m_cloudsTraversal.fillInit( type, inits );
	}

	void CloudsVolumePlugin::Shader::registerVolumes( c3ds::Volumes & volumes )
	{
		m_atmosphereTraversal.registerVolumes( volumes );
		m_cloudsTraversal.registerVolumes( volumes );
	}

	void CloudsVolumePlugin::Shader::initialise( sdw::Vec2 const & pixelCoord, c3ds::Ray & ray
		, c3ds::Volumes & volumes, c3ds::VolumesTraversalResult & result )
	{
		auto sceneUv = m_writer.declLocale( "sceneUv"
			, pixelCoord / result.renderSize );
		auto depthObj = m_writer.declLocale( "depthObj"
			, m_depthMap.isEnabled() ? m_depthMap.lod( vec2( sceneUv.x(), 1.0_f - sceneUv.y() ), 0.0_f ) : vec4( -1.0_f, -1.0_f, 0.0_f, 0.0_f ) );
		if ( m_depthMap.isEnabled() && m_atmosphere.settings.length.lengthUnit() != c3d::LengthUnit::eKilometre )
			depthObj.g() *= m_atmosphere.settings.length.kilometres();

		m_atmosphereTraversal.initialise( pixelCoord, ray, volumes, result, depthObj );
		m_cloudsTraversal.initialise( pixelCoord, ray, volumes, result, depthObj );
	}

	void CloudsVolumePlugin::Shader::finalise( c3ds::Ray const & ray, c3ds::VolumesTraversalResult & result )
	{
		m_atmosphereTraversal.finalise( ray, result );
	}

	//************************************************************************************************

	void CloudsVolumePlugin::registerBindings( crg::FramePass & pass
		, c3d::Camera const & camera
		, uint32_t & binding )const
	{
		auto itScene = m_sceneData.find( camera.getScene() );
		if ( itScene == m_sceneData.end() )
		{
			c3d::log::error << "CloudsVolumePlugin: Scene not registered." << std::endl;
			return;
		}

		auto itCamera = itScene->second->cameraData.find( &camera );
		if ( itCamera == itScene->second->cameraData.end() )
		{
			c3d::log::error << "CloudsVolumePlugin: Camera not registered." << std::endl;
			return;
		}

		crg::SamplerDesc linearClampSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear };
		crg::SamplerDesc linearRepeatSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest
			, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat };
		crg::SamplerDesc mipLinearSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eLinear
			, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat };

		auto index = details::getNextBinding( binding );
		if ( itCamera->second->depthObj )
			pass.addInputSampledT( *itCamera->second->depthObj->getSampledLastAttach(), index, linearClampSampler );
		itScene->second->atmosphereUbo.createPassBinding( pass, details::getNextBinding( binding ) );
		itScene->second->cloudsUbo.createPassBinding( pass, details::getNextBinding( binding ) );
		itCamera->second->cameraUbo.createPassBinding( pass, details::getNextBinding( binding ) );
		pass.addInputSampledT( *itScene->second->transmittance.getSampledLastAttach(), details::getNextBinding( binding ), linearClampSampler );
		pass.addInputSampledT( *itScene->second->multiScatter.getSampledLastAttach(), details::getNextBinding( binding ), linearClampSampler );
		pass.addInputSampledT( *itCamera->second->skyView.getSampledLastAttach(), details::getNextBinding( binding ), linearClampSampler );
		pass.addInputSampledT( *itCamera->second->volume.getSampledLastAttach(), details::getNextBinding( binding ), linearClampSampler );
		pass.addInputSampledT( *itScene->second->perlinWorley.getSampledLastAttach(), details::getNextBinding( binding ), mipLinearSampler );
		pass.addInputSampledT( *itScene->second->worley.getSampledLastAttach(), details::getNextBinding( binding ), mipLinearSampler );
		pass.addInputSampledT( *itScene->second->curl.getSampledLastAttach(), details::getNextBinding( binding ), linearRepeatSampler );
		pass.addInputSampledT( *itScene->second->weather.getSampledLastAttach(), details::getNextBinding( binding ), linearRepeatSampler );
	}

	void CloudsVolumePlugin::registerScenePasses( crg::ResourcesCache & resources
		, crg::FramePassGroup & graph
		, c3d::Scene const & scene )const
	{
		auto itScene = m_sceneData.find( &scene );
		if ( itScene == m_sceneData.end() )
		{
			c3d::log::error << "CloudsVolumePlugin: Scene not registered." << std::endl;
			return;
		}

		itScene->second->registerPasses( graph );
	}

	void CloudsVolumePlugin::registerCameraPasses( crg::ResourcesCache & resources
		, crg::FramePassGroup & graph
		, c3d::Camera const & camera )const
	{
		auto itScene = m_sceneData.find( camera.getScene() );
		if ( itScene == m_sceneData.end() )
		{
			c3d::log::error << "CloudsVolumePlugin: Scene not registered." << std::endl;
			return;
		}

		auto itCamera = itScene->second->cameraData.find( &camera );
		if ( itCamera == itScene->second->cameraData.end() )
		{
			c3d::log::error << "CloudsVolumePlugin: Camera not registered." << std::endl;
			return;
		}

		itCamera->second->registerPasses( graph, *itScene->second );
	}

	VolumeData * CloudsVolumePlugin::registerScene( c3d::Scene & scene )
	{
		auto [it, inserted] = m_sceneData.try_emplace( &scene );
		if ( inserted )
			it->second = c3d::makeRawUnique< VolumeData >( scene );
		return it->second.get();
	}

	void CloudsVolumePlugin::unregisterScene( c3d::Scene & scene )noexcept
	{
		if ( auto it = m_sceneData.find( &scene );
			it != m_sceneData.end() )
			m_sceneData.erase( it );
	}

	void CloudsVolumePlugin::registerCamera( c3d::Camera const & camera
		, c3d::Texture const * depthObj )
	{
		registerScene( *camera.getScene() )->registerCamera( camera, depthObj );
	}

	//************************************************************************************************
}

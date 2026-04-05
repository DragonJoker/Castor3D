#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsVolumePlugin.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Miscellaneous/ProgressBar.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Volumetric/VolumeComponentRegister.hpp>
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
				auto & volumeData = background.getVolumeData();
				auto transmittance = volumeData.transmittance.getExtent();
				auto multiScatter = volumeData.multiScatter.getExtent().width;
				auto atmosphereVolume = volumeData.getVolumeResolution();
				result = ( background.getSunNode()
					? writeName( file, cuT( "sunNode" ), background.getSunNode()->getName() )
					: true );
				result = result && ( background.getPlanetNode()
					? writeName( file, cuT( "planetNode" ), background.getPlanetNode()->getName() )
					: true );
				result = result && write( file, cuT( "transmittanceResolution" ), transmittance.width, transmittance.height );
				result = result && write( file, cuT( "multiScatterResolution" ), multiScatter );
				result = result && write( file, cuT( "atmosphereVolumeResolution" ), atmosphereVolume );

				auto & config = volumeData.getAtmosphereCfg();
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
					auto & weather = volumeData.getWeatherCfg();
					result = result && write( file, cuT( "worleyResolution" ), volumeData.getWorleyResolution() );
					result = result && write( file, cuT( "perlinWorleyResolution" ), volumeData.getPerlinWorleyResolution() );
					result = result && write( file, cuT( "curlResolution" ), volumeData.getCurlResolution() );
					result = result && write( file, cuT( "weatherResolution" ), volumeData.getWeatherResolution() );
					result = result && write( file, cuT( "amplitude" ), weather.perlinAmplitude );
					result = result && write( file, cuT( "frequency" ), weather.perlinFrequency );
					result = result && write( file, cuT( "scale" ), weather.perlinScale );
					result = result && write( file, cuT( "octaves" ), weather.perlinOctaves );
				}

				if ( auto cblock{ beginBlock( file, cuT( "clouds" ) ) } )
				{
					auto & clouds = volumeData.getCloudsCfg();
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
	AtmosphereBackground::CameraPasses::CameraPasses( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, AtmosphereBackground & background
		, c3d::Camera const & camera
		, VolumeData & volumeData
		, c3d::Texture const * scattering
		, c3d::Texture const * transmittance
		, c3d::Texture const * depthObj
		, c3d::RenderUbo const & renderUbo
		, c3d::SceneUbo const & sceneUbo
		, c3d::CameraUbo const & mainCameraUbo
		, c3d::Extent2D const & size
		, uint32_t index
		, bool forceVisible
		, c3d::BackgroundPassBase *& backgroundPass )
		: cameraData{ volumeData.registerCamera( camera, depthObj ) }
		, cloudsResult{ device
			, background.getScene().getResources()
			, cuT( "CloudsResult" ) + c3d::string::toString( index )
			, { c3d::ImageCreateFlags::eNone
				, { size.width, size.height, 1u }, 1u, 1u
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::ImageUsageFlags::eColorAttachment | c3d::ImageUsageFlags::eSampled }
			, { { .addressMode = c3d::WrapMode::eRepeat
				, .mipFilter = c3d::MipmapMode::eNearest } } }
		, cloudsResolvePass{ c3d::makeRawUnique< CloudsResolvePass >( graph
			, device
			, cameraData->cameraUbo
			, volumeData.atmosphereUbo
			, volumeData.cloudsUbo
			, volumeData.transmittance
			, volumeData.multiScatter
			, cameraData->skyView
			, cameraData->volume
			, *scattering
			, *transmittance
			, depthObj
			, cloudsResult
			, index ) }
	{
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
				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		renderUbo.createPassBinding( pass, AtmosphereBackgroundBindings::eRenderConfig );
		sceneUbo.createPassBinding( pass, AtmosphereBackgroundBindings::eScene );
		pass.addInputSampledT( *cloudsResult.getSampledLastAttach(), AtmosphereBackgroundBindings::eClouds
			, crg::SamplerDesc{ c3d::FilterMode::eNearest, c3d::FilterMode::eNearest } );
		lastPass = &pass;
	}

	AtmosphereBackground::CameraPasses::~CameraPasses()
	{
		cloudsResult.destroy();
	}

	void AtmosphereBackground::CameraPasses::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( cuT( "Clouds Result" )
			, cloudsResult
			, c3d::ImageLayout::eShaderReadOnly
			, c3d::TextureFactors{}.invert( true ) );
	}

	//*********************************************************************************************

	static uint32_t constexpr SkyTexSize = 16u;

	AtmosphereBackground::AtmosphereBackground( c3d::Engine & engine
		, c3d::Scene & scene
		, CloudsVolumePlugin & plugin )
		: SceneBackground{ engine, scene, cuT( "Atmosphere" ), cuT( "atmosphere" ), false }
		, m_plugin{ &plugin }
		, m_volumeData{ plugin.registerScene( scene ) }
	{
	}

	AtmosphereBackground::~AtmosphereBackground()noexcept
	{
		m_plugin->unregisterScene( getScene() );
	}

	void AtmosphereBackground::accept( c3d::BackgroundVisitor & visitor )
	{
		accept( static_cast< c3d::ConfigurationVisitorBase & >( visitor ) );
	}

	void AtmosphereBackground::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		m_volumeData->accept( visitor );
	}

	void AtmosphereBackground::createBackgroundPass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, c3d::ProgressBar * progress
		, c3d::Extent2D const & size
		, c3d::Camera const & camera
		, c3d::Texture & colour
		, c3d::Texture const * scattering
		, c3d::Texture const * transmittance
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
		auto it = findCameraPass( &colour );

		if ( it == m_cameraPasses.end() )
		{
			it = m_cameraPasses.emplace( &colour
				, c3d::makeRawUnique< CameraPasses >( graph
					, device
					, *this
					, camera
					, *m_volumeData
					, scattering
					, transmittance
					, depthObj
					, renderUbo
					, sceneUbo
					, cameraUbo
					, size
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

	c3d::Texture const & AtmosphereBackground::getTransmittance()const noexcept
	{
		return m_volumeData->transmittance;
	}

	c3d::Texture const & AtmosphereBackground::getMultiScatter()const noexcept
	{
		return m_volumeData->multiScatter;
	}

	void AtmosphereBackground::setAtmosphereCfg( AtmosphereScatteringConfig config )noexcept
	{
		m_volumeData->setAtmosphereCfg( c3d::move( config ) );
	}

	void AtmosphereBackground::setWeatherCfg( WeatherConfig config )noexcept
	{
		m_volumeData->setWeatherCfg( c3d::move( config ) );
	}

	void AtmosphereBackground::setCloudsCfg( CloudsConfig config )noexcept
	{
		m_volumeData->setCloudsCfg( c3d::move( config ) );
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
		m_textureId.create();
		m_texture = c3d::makeUnique< c3d::TextureLayout >( device.renderSystem
			, cuT( "AtmosphereBackground/Dummy" )
			, *m_textureId.image
			, m_textureId.getWholeViewId() );
		m_hdr = true;
		m_srgb = false;
		auto result = m_texture->initialise( device );

		if ( result )
			m_volumeData->initialise( m_sunNode, m_planetNode );

		return result;
	}

	void AtmosphereBackground::doCleanup()
	{
	}

	void AtmosphereBackground::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		static c3d::Point3f const Scale{ 1, -1, 1 };
		static c3d::Quaternion const Orientation{ c3d::Quaternion::identity() };

		auto const & camera = *updater.camera;
		auto node = camera.getParent();
		c3d::matrix::setTransform( updater.bgMtxModl
			, node->getDerivedPosition(), Scale, Orientation );

		CU_Require( m_sunNode );
		CU_Require( m_planetNode );
		m_volumeData->cpuUpdate( updater, m_sunNode, m_planetNode );

		auto lengthUnit = node->getScene()->getEngine()->getLengthUnit();
		auto length = c3d::Length::fromUnit( 1.0f, lengthUnit );

		auto rawPosition = node->getDerivedPosition();
		if ( auto planetNode = getPlanetNode() )
			rawPosition -= planetNode->getDerivedPosition();
		auto position = c3d::Vector3f::fromUnit( rawPosition, lengthUnit );
		auto orientation = node->getDerivedOrientation();
		auto right{ c3d::Vector3f::fromKilometres( c3d::Point3f{ 1.0, 0.0, 0.0 } ) };
		auto up{ c3d::Vector3f::fromKilometres( c3d::Point3f{ 0.0, 1.0, 0.0 } ) };
		orientation.transform( right, right );
		orientation.transform( up, up );
		auto front{ c3d::point::cross( right, up ) };
		up = c3d::point::cross( front, right );

		updater.bgPosition = position.kilometres();
		c3d::matrix::lookAt( updater.bgMtxView
			, position.kilometres()
			, ( position + front ).kilometres()
			, up.kilometres() );
		updater.bgMtxProj = camera.getRescaledProjection( updater.renderSize
			, length.kilometres()
			, updater.isSafeBanded );
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
			it->second->cameraData->cameraUbo.createPassBinding( pass, index );
			++index;
			m_volumeData->atmosphereUbo.createPassBinding( pass, index );
			++index;
			m_volumeData->cloudsUbo.createPassBinding( pass, index );
			++index;
			crg::SamplerDesc linearClampSampler{ c3d::FilterMode::eLinear
				, c3d::FilterMode::eLinear };
			pass.addInputSampled( *m_volumeData->transmittance.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *m_volumeData->multiScatter.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *it->second->cameraData->skyView.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *it->second->cameraData->volume.getSampledLastAttach(), index, linearClampSampler );
			++index;
			pass.addInputSampled( *it->second->cloudsResult.getSampledLastAttach(), index, linearClampSampler );
			++index;
		}
	}

	void AtmosphereBackground::doAddLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
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

	void AtmosphereBackground::doAddDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
		, c3d::Texture * targetImage
		, uint32_t & index )const
	{
		auto it = findCameraPass( targetImage );

		if ( it != m_cameraPasses.end() )
		{
			it->second->cameraData->cameraUbo.addDescriptorWrite( descriptorWrites, index );
			m_volumeData->atmosphereUbo.addDescriptorWrite( descriptorWrites, index );
			m_volumeData->cloudsUbo.addDescriptorWrite( descriptorWrites, index );
			m_volumeData->transmittance.addTextureDescriptorWrite( descriptorWrites, index );
			m_volumeData->multiScatter.addTextureDescriptorWrite( descriptorWrites, index );
			it->second->cameraData->skyView.addTextureDescriptorWrite( descriptorWrites, index );
			it->second->cameraData->volume.addTextureDescriptorWrite( descriptorWrites, index );
			it->second->cloudsResult.addTextureDescriptorWrite( descriptorWrites, *it->second->cameraData->volume.sampler, index );
		}
	}

	//************************************************************************************************
}

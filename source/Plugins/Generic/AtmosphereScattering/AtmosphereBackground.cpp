#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
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
				auto atmosphereVolume = background.getVolume().getExtent().width;
				result = write( file, "transmittanceResolution", transmittance.width, transmittance.height );
				result = result && write( file, "multiScatterResolution", multiScatter );
				result = result && write( file, "atmosphereVolumeResolution", atmosphereVolume );

				auto & config = background.getConfiguration();
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
	castor::String const AtmosphereBackgroundModel::Name = cuT( "c3d.atmosphere" );

	AtmosphereBackgroundModel::AtmosphereBackgroundModel( sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, uint32_t & binding
		, uint32_t set )
		: castor3d::shader::BackgroundModel{ writer, utils }
	{
	}

	castor3d::shader::BackgroundModelPtr AtmosphereBackgroundModel::create( sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< AtmosphereBackgroundModel >( writer, utils, binding, set );
	}

	sdw::Vec3 AtmosphereBackgroundModel::computeReflections( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, castor3d::shader::LightMaterial const & material
		, sdw::CombinedImage2DRg32 const & brdf )
	{
		return vec3( 0.0_f );
	}

	sdw::Vec3 AtmosphereBackgroundModel::computeRefractions( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, castor3d::shader::LightMaterial const & material )
	{
		return vec3( 0.0_f );
	}

	sdw::Void AtmosphereBackgroundModel::mergeReflRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, castor3d::shader::LightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )
	{
		return sdw::Void{};
	}

	//*********************************************************************************************

	static uint32_t constexpr SkyTexSize = 16u;

	AtmosphereBackground::AtmosphereBackground( castor3d::Engine & engine
		, castor3d::Scene & scene )
		: SceneBackground{ engine, scene, cuT( "Atmosphere" ), cuT( "atmosphere" ) }
		, m_atmosphereUbo{ std::make_unique< AtmosphereScatteringUbo >( engine.getRenderSystem()->getRenderDevice() ) }
	{
	}

	AtmosphereBackground::~AtmosphereBackground()
	{
		m_transmittance.destroy();
		m_multiScatter.destroy();
		m_skyView.destroy();
		m_volume.destroy();
	}

	void AtmosphereBackground::accept( castor3d::BackgroundVisitor & visitor )
	{
		//visitor.visit( *this );
	}

	crg::FramePass & AtmosphereBackground::createBackgroundPass( crg::FramePassGroup & graph
		, castor3d::RenderDevice const & device
		, castor3d::ProgressBar * progress
		, VkExtent2D const & size
		, crg::ImageViewId const & colour
		, crg::ImageViewId const * depth
		, castor3d::UniformBufferOffsetT< castor3d::ModelBufferConfiguration > const & modelUbo
		, castor3d::MatrixUbo const & matrixUbo
		, castor3d::HdrConfigUbo const & hdrConfigUbo
		, castor3d::SceneUbo const & sceneUbo
		, bool clearColour
		, castor3d::BackgroundPassBase *& backgroundPass )
	{
		if ( !m_transmittancePass )
		{
			m_cameraUbo = device.uboPool->getBuffer< CameraConfig >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			auto & viewData = *depth->data;
			m_depthView = graph.createView( { "BackgroundDepth"
				, viewData.image
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, viewData.info.format
				, { VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u } } );
			m_transmittancePass = std::make_unique< AtmosphereTransmittancePass >( graph
				, crg::FramePassArray{}
				, device
				, *m_atmosphereUbo
				, m_transmittance.targetViewId );
			m_multiScatteringPass = std::make_unique< AtmosphereMultiScatteringPass >( graph
				, crg::FramePassArray{ &m_transmittancePass->getLastPass() }
				, device
				, *m_atmosphereUbo
				, m_transmittance.sampledViewId
				, m_multiScatter.targetViewId );
			m_skyViewPass = std::make_unique< AtmosphereSkyViewPass >( graph
				, crg::FramePassArray{ &m_transmittancePass->getLastPass() }
				, device
				, m_cameraUbo
				, *m_atmosphereUbo
				, m_transmittance.sampledViewId
				, m_skyView.targetViewId
				, 0u );
			m_volumePass = std::make_unique< AtmosphereVolumePass >( graph
				, crg::FramePassArray{ &m_multiScatteringPass->getLastPass() }
				, device
				, m_cameraUbo
				, *m_atmosphereUbo
				, m_transmittance.sampledViewId
				, m_volume.targetViewId
				, 0u );
		}

		auto & pass = graph.createPass( "Background"
			, [this, &backgroundPass, &device, progress, size]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising background pass" );
				auto res = std::make_unique< AtmosphereBackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, *this
					, size
					, nullptr );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		pass.addDependency( m_multiScatteringPass->getLastPass() );
		pass.addDependency( m_skyViewPass->getLastPass() );
		pass.addDependency( m_volumePass->getLastPass() );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		pass.addImplicitDepthStencilView( *depth
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		m_cameraUbo.createPassBinding( pass
			, "Camera"
			, AtmosphereBackgroundPass::eCamera );
		m_atmosphereUbo->createPassBinding( pass
			, AtmosphereBackgroundPass::eAtmosphere );
		pass.addSampledView( m_transmittance.sampledViewId
			, AtmosphereBackgroundPass::eTransmittance
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addSampledView( m_multiScatter.sampledViewId
			, AtmosphereBackgroundPass::eMultiScatter
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addSampledView( m_skyView.sampledViewId
			, AtmosphereBackgroundPass::eSkyView
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addSampledView( m_volume.sampledViewId
			, AtmosphereBackgroundPass::eVolume
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addSampledView( m_depthView
			, AtmosphereBackgroundPass::eDepth 
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addInOutColourView( colour );
		return pass;
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

	void AtmosphereBackground::loadTransmittance( castor::Point2ui const & dimensions )
	{
		auto & handler = getScene().getEngine()->getGraphResourceHandler();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_transmittance = castor3d::Texture{ device
			, handler
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
		auto & handler = getScene().getEngine()->getGraphResourceHandler();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_multiScatter = castor3d::Texture{ device
			, handler
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
		auto & handler = getScene().getEngine()->getGraphResourceHandler();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_volume = castor3d::Texture{ device
			, handler
			, "Volume"
			, 0u
			, { dimension, dimension, dimension }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST };
		notifyChanged();
	}

	void AtmosphereBackground::loadSkyView( castor::Point2ui const & dimensions )
	{
		auto & handler = getScene().getEngine()->getGraphResourceHandler();
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();
		m_skyView = castor3d::Texture{ device
			, handler
			, "SkyView"
			, 0u
			, { dimensions->x, dimensions->y, 1u }
			, 1u
			, 1u
			, VK_FORMAT_B10G11R11_UFLOAT_PACK32
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST };
		notifyChanged();
	}

	bool AtmosphereBackground::doInitialise( castor3d::RenderDevice const & device )
	{
		auto data = device.graphicsData();
		auto & engine = *getEngine();
		m_textureId = castor3d::Texture{ device
			, engine.getGraphResourceHandler()
			, "Dummy"
			, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			, { SkyTexSize, SkyTexSize, 1u }
			, 6u
			, 1u
			, VK_FORMAT_B10G11R11_UFLOAT_PACK32
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST };
		m_transmittance.create();
		m_multiScatter.create();
		m_skyView.create();
		m_volume.create();
		m_textureId.create();
		m_texture = std::make_shared< castor3d::TextureLayout >( device.renderSystem
			, m_textureId.image
			, m_textureId.wholeViewId );
		m_hdr = true;
		m_srgb = false;
		return m_texture->initialise( device, *data );
	}

	void AtmosphereBackground::doCleanup()
	{
	}

	void AtmosphereBackground::doCpuUpdate( castor3d::CpuUpdater & updater )const
	{
		auto & viewport = *updater.viewport;
		viewport.resize( updater.camera->getSize() );
		viewport.setPerspective( updater.camera->getViewport().getFovY()
			, updater.camera->getRatio()
			, 0.1f
			, 2.0f );
		viewport.update();
		updater.bgMtxView = updater.camera->getView();
		updater.bgMtxProj = updater.isSafeBanded
			? viewport.getSafeBandedProjection()
			: viewport.getProjection();
		m_atmosphereUbo->cpuUpdate( m_config );

		auto viewProj = updater.bgMtxProj * updater.bgMtxView;
		auto & data = m_cameraUbo.getData();
		data.invViewProj = viewProj.getInverse();
		data.position = updater.camera->getParent()->getDerivedPosition();
		//std::swap( data.position->y, data.position->z );
	}

	void AtmosphereBackground::doGpuUpdate( castor3d::GpuUpdater & updater )const
	{
	}

	void AtmosphereBackground::doAddPassBindings( crg::FramePass & pass
		, uint32_t & index )const
	{
	}

	void AtmosphereBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
	}

	void AtmosphereBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
	}

	//************************************************************************************************
}

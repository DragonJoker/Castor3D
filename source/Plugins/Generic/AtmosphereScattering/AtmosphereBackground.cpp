#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
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
	//*********************************************************************************************

	AtmosphereBackground::CameraPasses::CameraPasses( crg::FramePassGroup & graph
		, castor3d::RenderDevice const & device
		, AtmosphereBackground & background
		, crg::FramePass const & transmittancePass
		, crg::ImageViewId const & transmittance
		, AtmosphereScatteringUbo const & atmosphereUbo
		, VkExtent2D const & size
		, castor::Point2ui const & skyViewResolution
		, uint32_t volumeResolution
		, uint32_t index
		, castor3d::BackgroundPassBase *& backgroundPass )
		: skyView{ device
			, graph.getHandler()
			, "SkyView" + std::to_string( index )
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
			, graph.getHandler()
			, "Volume" + std::to_string( index )
			, 0u
			, { volumeResolution, volumeResolution, volumeResolution }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST }
		, cameraUbo{ device }
		, skyViewPass{ std::make_unique< AtmosphereSkyViewPass >( graph
			, crg::FramePassArray{ &transmittancePass }
			, device
			, cameraUbo
			, atmosphereUbo
			, transmittance
			, skyView.targetViewId
			, index ) }
		, volumePass{ std::make_unique< AtmosphereVolumePass >( graph
			, crg::FramePassArray{ &transmittancePass }
			, device
			, cameraUbo
			, atmosphereUbo
			, transmittance
			, volume.targetViewId
			, index ) }
	{
		skyView.create();
		volume.create();
		auto & pass = graph.createPass( "Background"
			, [&background, &backgroundPass, &device, size]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< AtmosphereBackgroundPass >( framePass
					, context
					, runnableGraph
					, device
					, background
					, size
					, nullptr );
				backgroundPass = res.get();
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		pass.addDependency( skyViewPass->getLastPass() );
		pass.addDependency( volumePass->getLastPass() );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		cameraUbo.createPassBinding( pass
			, AtmosphereBackgroundPass::eCamera );
		pass.addSampledView( skyView.sampledViewId
			, AtmosphereBackgroundPass::eSkyView
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addSampledView( volume.sampledViewId
			, AtmosphereBackgroundPass::eVolume
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		lastPass = &pass;
	}

	AtmosphereBackground::CameraPasses::~CameraPasses()
	{
		skyView.destroy();
		volume.destroy();
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
		}

		auto it = m_cameraPasses.find( colour.data->image.data );

		if ( it == m_cameraPasses.end() )
		{
			it = m_cameraPasses.emplace( colour.data->image.data
				, std::make_unique< CameraPasses >( graph
					, device
					, *this
					, m_transmittancePass->getLastPass()
					, m_transmittance.sampledViewId
					, *m_atmosphereUbo
					, size
					, m_skyViewResolution
					, m_volumeResolution
					, uint32_t( m_cameraPasses.size() )
					, backgroundPass ) ).first;
			auto & pass = *it->second->lastPass;
			pass.addDependency( m_multiScatteringPass->getLastPass() );
			crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR };
			pass.addImplicitDepthStencilView( *depth
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
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
			pass.addOutputColourView( colour
				, castor3d::transparentBlackClearColor );
			pass.addInOutDepthStencilView( *depth );
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
		auto & engine = *getEngine();
		m_textureId = castor3d::Texture{ device
			, engine.getGraphResourceHandler()
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
		return m_texture->initialise( device, *data );
	}

	void AtmosphereBackground::doCleanup()
	{
	}

	void AtmosphereBackground::doCpuUpdate( castor3d::CpuUpdater & updater )const
	{
		CU_Require( m_node );
		m_atmosphereUbo->cpuUpdate( m_config, *m_node );
		auto it = m_cameraPasses.find( updater.targetImage );

		if ( it != m_cameraPasses.end() )
		{
			it->second->cameraUbo.cpuUpdate( *updater.camera, updater.isSafeBanded );
		}
	}

	void AtmosphereBackground::doGpuUpdate( castor3d::GpuUpdater & updater )const
	{
	}

	void AtmosphereBackground::doAddPassBindings( crg::FramePass & pass
		, crg::ImageData const & targetImage
		, uint32_t & index )const
	{
		auto it = m_cameraPasses.find( &targetImage );

		if ( it != m_cameraPasses.end() )
		{
			it->second->cameraUbo.createPassBinding( pass
				, index++ );
			m_atmosphereUbo->createPassBinding( pass
				, index++ );
			pass.addSampledView( m_transmittance.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
			pass.addSampledView( it->second->skyView.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
			pass.addSampledView( it->second->volume.wholeViewId
				, index++
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, crg::SamplerDesc{ VK_FILTER_LINEAR
					, VK_FILTER_LINEAR
					, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
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
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapTransmittance
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapSkyView
		bindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapVolume
	}

	void AtmosphereBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageData const & targetImage
		, uint32_t & index )const
	{
		auto it = m_cameraPasses.find( &targetImage );

		if ( it != m_cameraPasses.end() )
		{
			descriptorWrites.push_back( it->second->cameraUbo.getDescriptorWrite( index++ ) );
			descriptorWrites.push_back( m_atmosphereUbo->getDescriptorWrite( index++ ) );
			castor3d::bindTexture( m_transmittance.sampledView
				, *m_transmittance.sampler
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
		}
	}

	//************************************************************************************************
}

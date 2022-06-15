#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Miscellaneous/ProgressBar.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Background/Visitor.hpp>

#include <ashespp/Image/StagingTexture.hpp>

#include <RenderGraph/FramePassGroup.hpp>

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
			auto result = true;
			file << ( cuT( "\n" ) + tabs() + cuT( "//Skybox\n" ) );

			if ( auto block{ beginBlock( file, "atmospheric_scattering" ) } )
			{
				auto transmittance = background.getTransmittance().getExtent();
				auto multiScatter = background.getMultiScatter().getExtent();
				auto atmosphereVolume = background.getAtmosphereVolume().getExtent();
				file << tabs() << cuT( "transmittance " ) << transmittance.width << cuT( " " ) << transmittance.height << cuT( "\n" )
					<< tabs() << cuT( "multiScatter " ) << multiScatter.width << cuT( " " ) << multiScatter.height << cuT( "\n" )
					<< tabs() << cuT( "atmosphereVolume " ) << atmosphereVolume.width << cuT( " " ) << atmosphereVolume.height << cuT( " " ) << atmosphereVolume.depth << cuT( "\n" );
			}

			return result;
		}

	private:
		Path m_folder;
	};
}

namespace atmosphere_scattering
{
	static uint32_t constexpr SkyTexSize = 512u;

	AtmosphereBackground::AtmosphereBackground( castor3d::Engine & engine
		, castor3d::Scene & scene )
		: SceneBackground{ engine, scene, cuT( "Atmosphere" ), cuT( "atmosphere" ) }
		, m_atmosphereUbo{ std::make_unique< AtmosphereScatteringUbo >( engine.getRenderSystem()->getRenderDevice() ) }
	{
	}

	void AtmosphereBackground::accept( castor3d::BackgroundVisitor & visitor )
	{
		//visitor.visit( *this );
	}

	bool AtmosphereBackground::write( castor::String const & tabs
		, castor::Path const & folder
		, castor::StringStream & stream )const
	{
		return castor::TextWriter< AtmosphereBackground >{ tabs, folder }( *this, stream );
	}

	crg::FramePass & AtmosphereBackground::createBackgroundPass( crg::FramePassGroup & graph
		, castor3d::RenderDevice const & device
		, castor3d::ProgressBar * progress
		, VkExtent2D const & size
		, bool usesDepth
		, castor3d::MatrixUbo const & matrixUbo
		, castor3d::SceneUbo const & sceneUbo
		, castor3d::BackgroundPassBase *& backgroundPass )
	{
		auto ires = m_atmospherePasses.emplace( &matrixUbo, BackgroundPasses{ nullptr, nullptr } );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second.transmittance = std::make_unique< AtmosphereTransmittancePass >( graph
				, crg::FramePassArray{}
				, device
				, matrixUbo
				, *m_atmosphereUbo
				, m_transmittance.targetViewId );
			it->second.multiScattering = std::make_unique< AtmosphereMultiScatteringPass >( graph
				, crg::FramePassArray{ &it->second.transmittance->getLastPass() }
				, device
				, matrixUbo
				, *m_atmosphereUbo
				, m_transmittance.sampledViewId
				, m_multiScatter.targetViewId );
			it->second.skyView = std::make_unique< AtmosphereVolumePass >( graph
				, crg::FramePassArray{ &it->second.multiScattering->getLastPass() }
				, device
				, matrixUbo
				, sceneUbo
				, *m_atmosphereUbo
				, m_transmittance.sampledViewId
				, m_multiScatter.sampledViewId
				, m_atmosphereVolume.targetViewId );
			auto & pass = graph.createPass( "Background"
				, [this, &backgroundPass, &device, progress, size, usesDepth]( crg::FramePass const & framePass
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
						, usesDepth );
					backgroundPass = res.get();
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, res->getTimer() );
					return res;
				} );
			pass.addDependency( it->second.skyView->getLastPass() );
			it->second.lastPass = &pass;
		}

		return *it->second.lastPass;
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
		m_atmosphereVolume = castor3d::Texture{ device
			, handler
			, "AtmosphereVolume"
			, 0u
			, { dimension, dimension, dimension }
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
		m_textureId = { device
			, engine.getGraphResourceHandler()
			, cuT( "AtmosphereResult" )
			, 0u
			, { SkyTexSize, SkyTexSize, 1u }
			, 1u
			, 1u
			, VK_FORMAT_B10G11R11_UFLOAT_PACK32
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) };
		m_transmittance.create();
		m_multiScatter.create();
		m_atmosphereVolume.create();
		m_textureId.create();
		m_texture = std::make_shared< castor3d::TextureLayout >( device.renderSystem
			, m_textureId.image
			, m_textureId.wholeViewId );
		m_hdr = true;
		m_srgb = isSRGBFormat( castor3d::convert( m_texture->getPixelFormat() ) );
		return m_texture->initialise( device, *data );
	}

	void AtmosphereBackground::doCleanup()
	{
		m_atmosphereVolume.destroy();
		m_multiScatter.destroy();
		m_transmittance.destroy();
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
	}

	void AtmosphereBackground::doGpuUpdate( castor3d::GpuUpdater & updater )const
	{
	}

	//************************************************************************************************
}

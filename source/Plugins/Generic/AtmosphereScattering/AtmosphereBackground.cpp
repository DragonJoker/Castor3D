#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Background/Visitor.hpp>

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

			if ( auto block{ beginBlock( file, "skybox" ) } )
			{
				Path subfolder{ cuT( "Textures" ) };

				if ( auto transmBlock{ beginBlock( file, "transmittance" ) } )
				{
					auto & transmittance = background.getTransmittance();

					if ( !transmittance.path.empty() )
					{
						String relative = copyFile( transmittance.path
							, m_folder
							, subfolder );
						string::replace( relative, cuT( "\\" ), cuT( "/" ) );
						file << tabs() << cuT( "image \"" ) << relative << cuT( "\"\n" )
							<< tabs() << cuT( "dimensions " ) << transmittance.dimensions->x << cuT( " " ) << transmittance.dimensions->y << cuT( "\n" )
							<< tabs() << cuT( "format " ) << getFormatName( transmittance.format ) << cuT( "\n" );
					}
				}

				if ( auto inscBlock{ beginBlock( file, "inscatter" ) } )
				{
					auto & inscattering = background.getInscattering();

					if ( !inscattering.path.empty() )
					{
						String relative = copyFile( inscattering.path
							, m_folder
							, subfolder );
						string::replace( relative, cuT( "\\" ), cuT( "/" ) );
						file << tabs() << cuT( "image \"" ) << relative << cuT( "\"\n" )
							<< tabs() << cuT( "dimensions " ) << inscattering.dimensions->x << cuT( " " ) << inscattering.dimensions->y << cuT( " " ) << inscattering.dimensions->z << cuT( "\n" )
							<< tabs() << cuT( "format " ) << getFormatName( inscattering.format ) << cuT( "\n" );
					}
				}

				if ( auto irradBlock{ beginBlock( file, "irradiance" ) } )
				{
					auto & irradiance = background.getIrradiance();

					if ( !irradiance.path.empty() )
					{
						String relative = copyFile( irradiance.path
							, m_folder
							, subfolder );
						string::replace( relative, cuT( "\\" ), cuT( "/" ) );
						file << tabs() << cuT( "image \"" ) << relative << cuT( "\"\n" )
							<< tabs() << cuT( "dimensions " ) << irradiance.dimensions->x << cuT( " " ) << irradiance.dimensions->y << cuT( "\n" )
							<< tabs() << cuT( "format " ) << getFormatName( irradiance.format ) << cuT( "\n" );
					}
				}
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

	std::unique_ptr< castor3d::BackgroundPassBase > AtmosphereBackground::createBackgroundPass( crg::FramePass const & framePass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, VkExtent2D const & size
		, bool usesDepth )
	{
		return std::make_unique< AtmosphereBackgroundPass >( framePass
			, context
			, graph
			, device
			, *this
			, size
			, usesDepth );
	}

	void AtmosphereBackground::loadTransmittance( castor::Path const & folder
		, castor::Path const & relative
		, castor::Point2ui const & dimensions
		, castor::PixelFormat format )
	{
		ashes::ImageCreateInfo image{ 0u
			, VK_IMAGE_TYPE_2D
			, castor3d::convert( format )
			, { dimensions->x, dimensions->y, 1u }
			, 1u
			, 1u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_IMAGE_TILING_OPTIMAL
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) };
		m_transmittance = { relative
			, dimensions
			, format
			, castor::makeUnique< castor3d::TextureLayout >( *getScene().getEngine()->getRenderSystem()
				, std::move( image )
				, VkMemoryPropertyFlags( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT )
				, cuT( "AtmosphereTransmittance" ) ) };

		if ( relative.getExtension() == "raw" )
		{
			FILE * file{};

			if ( castor::fileOpen( file, ( folder / relative ).c_str(), "rb" ) )
			{
				castor::ByteArray data;
				data.resize( ashes::getSize( VkExtent2D{ dimensions->x, dimensions->y }
				, castor3d::convert( format ) ) );
				auto read = fread( data.data(), 1, data.size(), file );

				if ( read == data.size() )
				{
					auto buffer = castor::PxBufferBase::create( castor::Size{ dimensions->x, dimensions->y }
						, 1u
						, 1u
						, format
						, data.data()
						, format );
					m_transmittance.texture->setSource( std::move( buffer ), true );
					notifyChanged();
				}
				else
				{
					castor3d::log::error << "RAW image file binary size doesn't match expected size." << std::endl;
				}
			}
		}
		else
		{
			m_transmittance.texture->setSource( folder, relative, { false, false, false } );
			notifyChanged();
		}
	}

	void AtmosphereBackground::loadInscaterring( castor::Path const & folder
		, castor::Path const & relative
		, castor::Point3ui const & dimensions
		, castor::PixelFormat format )
	{
		ashes::ImageCreateInfo image{ 0u
			, VK_IMAGE_TYPE_3D
			, castor3d::convert( format )
			, { dimensions->x, dimensions->y, dimensions->z }
			, 1u
			, 1u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_IMAGE_TILING_OPTIMAL
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) };
		m_inscattering = { relative
			, dimensions
			, format
			, castor::makeUnique< castor3d::TextureLayout >( *getScene().getEngine()->getRenderSystem()
				, std::move( image )
				, VkMemoryPropertyFlags( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT )
				, cuT( "AtmosphereInscattering" ) ) };

		if ( relative.getExtension() == "raw" )
		{
			FILE * file{};

			if ( castor::fileOpen( file, ( folder / relative ).c_str(), "rb" ) )
			{
				castor::ByteArray data;
				auto vkfmt = castor3d::convert( format );
				auto extent = VkExtent3D{ dimensions->x, dimensions->y, dimensions->z };
				data.resize( ashes::getSize( extent, vkfmt ) );
				auto read = fread( data.data(), 1, data.size(), file );

				if ( read == data.size() )
				{
					m_inscattering.texture->setSource( extent, vkfmt );
					std::memcpy( m_inscattering.texture->getImage().getPixels()->getPtr()
						, data.data()
						, data.size() );
					notifyChanged();
				}
				else
				{
					castor3d::log::error << "RAW image file binary size doesn't match expected size." << std::endl;
				}
			}
		}
		else
		{
			m_inscattering.texture->setSource( folder, relative, { false, false, false } );
			notifyChanged();
		}
	}

	void AtmosphereBackground::loadIrradiance( castor::Path const & folder
		, castor::Path const & relative
		, castor::Point2ui const & dimensions
		, castor::PixelFormat format )
	{
		ashes::ImageCreateInfo image{ 0u
			, VK_IMAGE_TYPE_2D
			, castor3d::convert( format )
			, { dimensions->x, dimensions->y, 1u }
			, 1u
			, 1u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_IMAGE_TILING_OPTIMAL
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) };
		m_irradiance = { relative
			, dimensions
			, format
			, castor::makeUnique< castor3d::TextureLayout >( *getScene().getEngine()->getRenderSystem()
				, std::move( image )
				, VkMemoryPropertyFlags( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT )
				, cuT( "AtmosphereIrradiance" ) ) };

		if ( relative.getExtension() == "raw" )
		{
			FILE * file{};

			if ( castor::fileOpen( file, ( folder / relative ).c_str(), "rb" ) )
			{
				castor::ByteArray data;
				data.resize( ashes::getSize( VkExtent2D{ dimensions->x, dimensions->y }
				, castor3d::convert( format ) ) );
				auto read = fread( data.data(), 1, data.size(), file );

				if ( read == data.size() )
				{
					auto buffer = castor::PxBufferBase::create( castor::Size{ dimensions->x, dimensions->y }
						, 1u
						, 1u
						, format
						, data.data()
						, format );
					m_irradiance.texture->setSource( std::move( buffer ), true );
					notifyChanged();
				}
				else
				{
					castor3d::log::error << "RAW image file binary size doesn't match expected size." << std::endl;
				}
			}
		}
		else
		{
			m_irradiance.texture->setSource( folder, relative, { false, false, false } );
			notifyChanged();
		}
	}

	bool AtmosphereBackground::doInitialise( castor3d::RenderDevice const & device )
	{
		auto data = device.graphicsData();
		m_transmittance.texture->initialise( device, *data );
		m_inscattering.texture->initialise( device, *data );
		m_irradiance.texture->initialise( device, *data );
		auto & engine = *getEngine();
		m_textureId = { device
			, engine.getGraphResourceHandler()
			, cuT( "AtmosphereResult" )
			, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			, { SkyTexSize, SkyTexSize, 1u }
			, 6u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) };
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
	}

	void AtmosphereBackground::doCpuUpdate( castor3d::CpuUpdater & updater )const
	{
		auto & viewport = *updater.viewport;
		viewport.resize( updater.camera->getSize() );
		viewport.setOrtho( -1.0f
			, 1.0f
			, -m_ratio
			, m_ratio
			, 0.1f
			, 2.0f );
		viewport.update();
		auto node = updater.camera->getParent();
		castor::Matrix4x4f view;
		castor::matrix::lookAt( view
			, node->getDerivedPosition()
			, node->getDerivedPosition() + castor::Point3f{ 0.0f, 0.0f, 1.0f }
		, castor::Point3f{ 0.0f, 1.0f, 0.0f } );
		updater.bgMtxView = view;
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

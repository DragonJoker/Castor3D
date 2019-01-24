#include "Scene/Background/Colour.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Shader/Program.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/StagingTexture.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

using namespace castor;
using namespace sdw;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		static uint32_t constexpr Dim = 16u;

		ashes::ImageCreateInfo doGetImageCreate()
		{
			Size size{ Dim, Dim };
			ashes::ImageCreateInfo result;
			result.flags = ashes::ImageCreateFlag::eCubeCompatible;
			result.arrayLayers = 6u;
			result.extent.width = Dim;
			result.extent.height = Dim;
			result.extent.depth = 1u;
			result.format = ashes::Format::eR32G32B32A32_SFLOAT;
			result.imageType = ashes::TextureType::e2D;
			result.initialLayout = ashes::ImageLayout::eUndefined;
			result.mipLevels = 1u;
			result.samples = ashes::SampleCountFlag::e1;
			result.sharingMode = ashes::SharingMode::eExclusive;
			result.tiling = ashes::ImageTiling::eOptimal;
			result.usage = ashes::ImageUsageFlag::eSampled | ashes::ImageUsageFlag::eTransferDst;
			return result;
		}
	}

	//************************************************************************************************

	ColourBackground::ColourBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eColour }
		, m_viewport{ engine }
	{
		m_hdr = false;
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate()
			, ashes::MemoryPropertyFlag::eDeviceLocal );
	}

	ColourBackground::~ColourBackground()
	{
	}

	void ColourBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	bool ColourBackground::doInitialise( ashes::RenderPass const & renderPass )
	{
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
		auto & device = getCurrentDevice( *this );
		m_stagingTexture = device.createStagingTexture( ashes::Format::eR32G32B32A32_SFLOAT 
			, { Dim, Dim } );

		m_texture->getImage( 0u ).initialiseSource();
		m_texture->getImage( 1u ).initialiseSource();
		m_texture->getImage( 2u ).initialiseSource();
		m_texture->getImage( 3u ).initialiseSource();
		m_texture->getImage( 4u ).initialiseSource();
		m_texture->getImage( 5u ).initialiseSource();
		auto result = m_texture->initialise();
		m_colour.reset();

		if ( result )
		{
			m_cmdCopy = device.getGraphicsCommandPool().createCommandBuffer( true );
			m_cmdCopy->begin();

			for ( uint32_t i = 0; i < 6u; ++i )
			{
				m_stagingTexture->copyTextureData( *m_cmdCopy
					, ashes::Format::eR32G32B32A32_SFLOAT
					, m_texture->getImage( i ).getView() );
			}

			m_cmdCopy->end();
		}

		doUpdateColour();
		return result;
	}

	void ColourBackground::doCleanup()
	{
		m_stagingTexture.reset();
		m_cmdCopy.reset();
	}

	void ColourBackground::doUpdate( Camera const & camera )
	{
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );

		if ( m_colour.isDirty() )
		{
			doUpdateColour();
		}

		auto node = camera.getParent();
		m_viewport.setPerspective( 45.0_degrees
			, camera.getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.update( camera.getView()
			, m_viewport.getProjection() );
	}

	void ColourBackground::doUpdateColour()
	{
		ashes::Extent2D lockExtent{ Dim, Dim };

		if ( auto * buffer = reinterpret_cast< Point4f * >( m_stagingTexture->lock( lockExtent, ashes::MemoryMapFlag::eWrite ) ) )
		{
			Point4f colour{ m_colour->red().value(), m_colour->green().value(), m_colour->blue().value(), 1.0f };

			for ( auto i = 0u; i < Dim * Dim; ++i )
			{
				*buffer = colour;
				++buffer;
			}

			m_stagingTexture->flush( lockExtent );
			m_stagingTexture->unlock();
			auto & device = getCurrentDevice( *this );

			device.getGraphicsQueue().submit( *m_cmdCopy, nullptr );
			device.getGraphicsQueue().waitIdle();

			m_colour.reset();
		}
	}

	//************************************************************************************************
}

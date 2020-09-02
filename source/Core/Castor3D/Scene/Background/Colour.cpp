#include "Castor3D/Scene/Background/Colour.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderWriter/Source.hpp>
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

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
			return ashes::ImageCreateInfo
			{
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				{ Dim, Dim, 1u },
				1u,
				6u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
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
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "ColourBackground_Colour" ) );
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
		auto & device = getCurrentRenderDevice( *this );
		m_stagingTexture = device->createStagingTexture( "ColourBackgroundStaging"
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, { Dim, Dim } );
		auto result = m_texture->initialise();
		m_colour.reset();

		if ( result )
		{
			m_cmdCopy = device.graphicsCommandPool->createCommandBuffer( "ColourBackgroundCopy"
				, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
			m_cmdCopy->begin();

			for ( uint32_t i = 0; i < 6u; ++i )
			{
				m_stagingTexture->copyTextureData( *m_cmdCopy
					, VK_FORMAT_R32G32B32A32_SFLOAT
					, m_texture->getLayerCubeFaceView( 0u, CubeMapFace( i ) ).getTargetView() );
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

		m_viewport.setPerspective( 45.0_degrees
			, camera.getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.cpuUpdate( camera.getView()
			, m_viewport.getProjection() );
	}

	void ColourBackground::doUpdateColour()
	{
		auto & device = getCurrentRenderDevice( *this );
		VkDeviceSize lockSize = Dim * Dim * sizeof( Point4f );

		if ( auto * buffer = reinterpret_cast< Point4f * >( m_stagingTexture->lock( 0u, lockSize, 0u ) ) )
		{
			Point4f colour{ m_colour->red().value(), m_colour->green().value(), m_colour->blue().value(), 1.0f };

			for ( auto i = 0u; i < Dim * Dim; ++i )
			{
				*buffer = colour;
				++buffer;
			}

			m_stagingTexture->flush( 0u, lockSize );
			m_stagingTexture->unlock();
			auto & renderSystem = *getEngine()->getRenderSystem();

			device.graphicsQueue->submit( *m_cmdCopy, nullptr );
			device.graphicsQueue->waitIdle();

			m_colour.reset();
		}
	}

	//************************************************************************************************
}

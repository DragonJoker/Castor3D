#include "Castor3D/Scene/Background/Colour.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Render/RenderModule.hpp"
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
	}

	//************************************************************************************************

	ColourBackground::ColourBackground( Engine & engine
		, Scene & scene
		, castor::String const & name )
		: SceneBackground{ engine, scene, name + cuT( "Colour" ), BackgroundType::eColour }
	{
		m_hdr = false;
		m_textureId = { engine.getRenderSystem()->getRenderDevice()
			, engine.getGraphResourceHandler()
			, cuT( "ColourBackground_Colour" )
			, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			, { Dim, Dim, 1u }
			, 6u
			, 1u
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
		m_textureId.create();
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, m_textureId.image
			, m_textureId.wholeViewId );
	}

	void ColourBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	bool ColourBackground::doInitialise( RenderDevice const & device )
	{
		m_textureId.create();
		auto data = device.graphicsData();
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
		m_stagingTexture = device->createStagingTexture( "ColourBackgroundStaging"
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, { Dim, Dim } );
		auto result = m_texture->initialise( device, *data );
		m_colour.reset();

		if ( result )
		{
			m_cmdCopy = data->commandPool->createCommandBuffer( "ColourBackgroundCopy"
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

		doUpdateColour( device );
		return result;
	}

	void ColourBackground::doCleanup()
	{
		m_stagingTexture.reset();
		m_cmdCopy.reset();
		m_textureId.destroy();
	}

	void ColourBackground::doCpuUpdate( CpuUpdater & updater )const
	{
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
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
	}

	void ColourBackground::doGpuUpdate( GpuUpdater & updater )const
	{
		if ( m_colour.isDirty() )
		{
			doUpdateColour( updater.device );
		}
	}

	void ColourBackground::doUpdateColour( RenderDevice const & device )const
	{
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

			auto data = device.graphicsData();
			data->queue->submit( *m_cmdCopy, nullptr );
			data->queue->waitIdle();

			m_colour.reset();
		}
	}

	//************************************************************************************************
}

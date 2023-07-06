#include "Castor3D/Scene/Background/Colour.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslIblBackground.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslImgBackground.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslNoIblBackground.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

CU_ImplementSmartPtr( castor3d, ColourBackground )

namespace castor3d
{
	//************************************************************************************************

	namespace bgcolour
	{
		static uint32_t constexpr Dim = 16u;
	}

	//************************************************************************************************

	ColourBackground::ColourBackground( Engine & engine
		, Scene & scene
		, castor::String const & name )
		: SceneBackground{ engine
			, scene
			, name + cuT( "Colour" )
			, cuT( "colour" )
			, true }
	{
		m_hdr = false;
		m_textureId = { engine.getRenderSystem()->getRenderDevice()
			, getScene().getResources()
			, cuT( "ColourBackground_Colour" )
			, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			, { bgcolour::Dim, bgcolour::Dim, 1u }
			, 6u
			, 1u
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
		m_textureId.create();
		m_texture = castor::makeUnique< TextureLayout >( *engine.getRenderSystem()
			, cuT( "ColourBackground_Colour" )
			, *m_textureId.image
			, m_textureId.wholeViewId );
	}

	void ColourBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	void ColourBackground::accept( PipelineVisitor & visitor )
	{
	}

	bool ColourBackground::write( castor::String const & tabs
		, castor::Path const & folder
		, castor::StringStream & stream )const
	{
		return true;
	}

	bool ColourBackground::doInitialise( RenderDevice const & device )
	{
		m_textureId.create();
		auto data = device.graphicsData();
		auto & value = m_scene.getBackgroundColour();
		m_colour = castor::HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
		m_stagingTexture = device->createStagingTexture( "ColourBackgroundStaging"
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, VkExtent2D{ bgcolour::Dim, bgcolour::Dim } );
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

		doUpdateColour( *data, device );
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
		m_colour = castor::HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
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
			auto data = updater.device.graphicsData();
			doUpdateColour( *data, updater.device );
		}
	}

	void ColourBackground::doAddPassBindings( crg::FramePass & pass
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		pass.addSampledView( m_textureId.wholeViewId
			, index++
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
	}

	void ColourBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBackground
	}

	void ColourBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		bindTexture( m_textureId.wholeView
			, *m_textureId.sampler
			, descriptorWrites
			, index );
	}

	void ColourBackground::doUpdateColour( QueueData const & queueData
		, RenderDevice const & device )const
	{
		VkDeviceSize lockSize = bgcolour::Dim * bgcolour::Dim * sizeof( castor::Point4f );

		if ( auto * buffer = reinterpret_cast< castor::Point4f * >( m_stagingTexture->lock( 0u, lockSize, 0u ) ) )
		{
			castor::Point4f colour{ m_colour->red().value(), m_colour->green().value(), m_colour->blue().value(), 1.0f };

			for ( auto i = 0u; i < bgcolour::Dim * bgcolour::Dim; ++i )
			{
				*buffer = colour;
				++buffer;
			}

			m_stagingTexture->flush( 0u, lockSize );
			m_stagingTexture->unlock();

			queueData.queue->submit( *m_cmdCopy, nullptr );
			queueData.queue->waitIdle();

			m_colour.reset();
		}
	}

	//************************************************************************************************
}

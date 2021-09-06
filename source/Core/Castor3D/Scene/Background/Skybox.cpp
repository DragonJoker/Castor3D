#include "Castor3D/Scene/Background/Skybox.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/ToTexture/EquirectangularToCube.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Shader/ShaderModule.hpp>
#include <ashespp/Sync/Fence.hpp>

#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

using namespace castor;
using namespace sdw;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		ashes::ImageCreateInfo doGetImageCreate( VkFormat format
			, Size const & dimensions
			, bool attachment
			, uint32_t mipLevel = 1u )
		{
			return ashes::ImageCreateInfo
			{
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				format,
				{ dimensions.getWidth(), dimensions.getHeight(), 1u },
				mipLevel,
				6u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VkImageUsageFlags( attachment
						? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
						: VkImageUsageFlagBits( 0u ) ) ),
			};
		}
	}

	//************************************************************************************************

	SkyboxBackground::SkyboxBackground( Engine & engine
		, Scene & scene
		, castor::String const & name )
		: SceneBackground{ engine, scene, name + cuT( "Skybox" ), BackgroundType::eSkybox }
	{
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate( VK_FORMAT_R8G8B8A8_UNORM, { 16u, 16u }, false )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackground_Colour" )
			, true /* isStatic */ );
	}

	SkyboxBackground::~SkyboxBackground()
	{
	}

	void SkyboxBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	void SkyboxBackground::loadLeftImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::eNegativeX );
	}

	void SkyboxBackground::loadRightImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::ePositiveX );
	}

	void SkyboxBackground::loadTopImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::eNegativeY );
	}

	void SkyboxBackground::loadBottomImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::ePositiveY );
	}

	void SkyboxBackground::loadFrontImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::eNegativeZ );
	}

	void SkyboxBackground::loadBackImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::ePositiveZ );
	}

	void SkyboxBackground::loadFaceImage( castor::Path const & folder
		, castor::Path const & relative
		, CubeMapFace face )
	{
		setFaceTexture( folder, relative, face );
	}

	void SkyboxBackground::setFaceTexture( castor::Path const & folder
		, castor::Path const & relative
		, CubeMapFace face )
	{
		ashes::ImageCreateInfo image{ 0u
			, VK_IMAGE_TYPE_2D
			, VK_FORMAT_UNDEFINED
			, { 1u, 1u, 1u }
			, 1u
			, 1u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_IMAGE_TILING_OPTIMAL
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) };
		auto texture = std::make_shared< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackground" ) + castor3d::getName( face ) );
		texture->setSource( folder, relative, false, false );
		m_layerTexturePath[size_t( face )] = castor::Path( texture->getImage().getPath() );
		m_layerTexture[size_t( face )] = texture;
		notifyChanged();
	}

	void SkyboxBackground::loadEquiTexture( castor::Path const & folder
		, castor::Path const & relative
		, uint32_t size )
	{
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			{ size, size, 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ),
		};
		auto texture = std::make_shared< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackgroundEquirectangular" ) );
		texture->setSource( folder, relative, false, false );
		setEquiTexture( texture, size );
	}

	void SkyboxBackground::setEquiTexture( TextureLayoutSPtr texture
		, uint32_t size )
	{
		m_equiTexturePath = castor::Path( texture->getImage().getPath() );
		m_equiTexture = texture;
		m_equiSize.set( size, size );
		notifyChanged();
	}

	void SkyboxBackground::setEquiSize( uint32_t size )
	{
		m_equiSize.set( size, size );
		notifyChanged();
	}

	void SkyboxBackground::loadCrossTexture( castor::Path const & folder
		, castor::Path const & relative )
	{
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			{ 1u, 1u, 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ),
		};
		auto texture = std::make_shared< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackgroundCross" ) );
		texture->setSource( folder, relative, false, false );
		setCrossTexture( texture );
	}

	void SkyboxBackground::setCrossTexture( TextureLayoutSPtr texture )
	{
		m_crossTexturePath = castor::Path( texture->getImage().getPath() );
		m_crossTexture = texture;
		notifyChanged();
	}

	bool SkyboxBackground::doInitialise( RenderDevice const & device )
	{
		CU_Require( m_texture );
		return doInitialiseTexture( device );
	}

	void SkyboxBackground::doCleanup()
	{
	}

	void SkyboxBackground::doCpuUpdate( CpuUpdater & updater )const
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
	}

	void SkyboxBackground::doGpuUpdate( GpuUpdater & updater )const
	{
	}

	bool SkyboxBackground::doInitialiseTexture( RenderDevice const & device )
	{
		auto data = device.graphicsData();

		if ( m_equiTexture )
		{
			doInitialiseEquiTexture( device, *data );
		}
		else if ( m_crossTexture )
		{
			doInitialiseCrossTexture( device, *data );
		}
		else
		{
			doInitialiseLayerTexture( device, *data );
		}

		m_hdr = m_texture->getPixelFormat() == VK_FORMAT_R32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32B32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32B32A32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16B16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16B16A16_SFLOAT;
		return m_texture->initialise( device, *data );
	}

	void SkyboxBackground::doInitialiseLayerTexture( RenderDevice const & device
		, QueueData const & queueData )
	{
		uint32_t maxDim{};

		for ( auto & layer : m_layerTexture )
		{
			layer->initialise( device, queueData );
			auto dim = layer->getDimensions();
			maxDim = std::max( maxDim
				, std::max( dim.width, dim.height ) );
		}

		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();

		// create the cube texture if needed.
		m_textureId = { device
			, getEngine()->getGraphResourceHandler()
			, cuT( "SkyboxBackgroundLayerCube" )
			, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			, { maxDim, maxDim, 1u }
			, 6u
			, 1u
			, m_layerTexture[0]->getPixelFormat()
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) };
		m_textureId.create();
		m_texture = std::make_shared< TextureLayout >( device.renderSystem
			, m_textureId.image
			, m_textureId.wholeViewId );

		auto commandBuffer = queueData.commandPool->createCommandBuffer( "SkyboxBackground" );
		commandBuffer->begin();
		commandBuffer->beginDebugBlock(
			{
				"Layer to Skybox",
				makeFloatArray( getScene().getEngine()->getNextRainbowColour() ),
			} );
		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_texture->getLayerCubeView( 0u ).getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		VkImageSubresourceLayers srcSubresource{ VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u };
		VkImageSubresourceLayers dstSubresource{ VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u };

		for ( auto & layer : m_layerTexture )
		{
			auto dim = layer->getDimensions();
			VkImageBlit blitInfo{ srcSubresource
				, { {}, { int32_t( dim.width ), int32_t( dim.height ), 1 } }
				, dstSubresource
				, { {}, { int32_t( maxDim ), int32_t( maxDim ), 1 } } };
			dstSubresource.baseArrayLayer++;

			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, layer->getDefaultView().getTargetView().makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			commandBuffer->blitImage( layer->getTexture()
				, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, m_texture->getTexture()
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, { blitInfo }
				, VK_FILTER_LINEAR );
		}

		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_texture->getLayerCubeView( 0u ).getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		commandBuffer->endDebugBlock();
		commandBuffer->end();

		queueData.queue->submit( *commandBuffer, nullptr );
		queueData.queue->waitIdle();

		for ( auto & layer : m_layerTexture )
		{
			layer->cleanup();
		}
	}

	void SkyboxBackground::doInitialiseEquiTexture( RenderDevice const & device
		, QueueData const & queueData )
	{
		m_equiTexture->initialise( device, queueData );

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != m_equiSize.getWidth()
			|| m_texture->getDimensions().height != m_equiSize.getHeight() )
		{
			m_textureId = { device
				, getEngine()->getGraphResourceHandler()
				, cuT( "SkyboxBackgroundEquiCube" )
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, makeExtent3D( m_equiSize )
				, 6u
				, 1u
				, m_equiTexture->getPixelFormat()
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) };
			m_textureId.create();
			m_texture = std::make_shared< TextureLayout >( device.renderSystem
				, m_textureId.image
				, m_textureId.wholeViewId );

			EquirectangularToCube equiToCube{ *m_equiTexture
				, device
				, *m_texture };
			equiToCube.render( queueData );

			if ( m_scene.getEngine()->getPassFactory().hasIBL( m_scene.getPassesType() ) )
			{
				m_texture->generateMipmaps( queueData );
			}
		}

		m_equiTexture->cleanup();
	}

	void SkyboxBackground::doInitialiseCrossTexture( RenderDevice const & device
		, QueueData const & queueData )
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_crossTexture->initialise( device, queueData );
		auto width = m_crossTexture->getWidth() / 4u;
		auto height = m_crossTexture->getHeight() / 3u;
		CU_Require( width == height );

		// create the cube texture if needed.
		m_textureId = { device
			, getEngine()->getGraphResourceHandler()
			, cuT( "SkyboxBackgroundCrossCube" )
			, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			, { width, width, 1u }
			, 6u
			, 1u
			, m_crossTexture->getPixelFormat()
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) };
		m_textureId.create();
		m_texture = std::make_shared< TextureLayout >( device.renderSystem
			, m_textureId.image
			, m_textureId.wholeViewId );

		VkImageSubresourceLayers srcSubresource
		{
			m_crossTexture->getDefaultView().getTargetView()->subresourceRange.aspectMask,
			0,
			0,
			1,
		};
		VkImageSubresourceLayers dstSubresource
		{
			m_texture->getDefaultView().getTargetView()->subresourceRange.aspectMask,
			0,
			0,
			1,
		};
		ashes::VkImageCopyArray copyInfos
		{
			6u,
			VkImageCopy
			{
				srcSubresource,
				{},
				dstSubresource,
				{},
				{ width, height, 1u }
			}
		};
		auto iwidth = int32_t( width );
		std::array< VkOffset3D, 6u > srcOffsets
		{
			{
				{ iwidth * 2, iwidth, 0 },
				{ 0, iwidth, 0 },
				{ 0, 0, 0 },
				{ iwidth, iwidth * 2, 0 },
				{ iwidth, iwidth, 0 },
				{ iwidth * 3, iwidth, 0 },
			}
		};

		for ( uint32_t i = 0u; i < 6u; ++i )
		{
			copyInfos[i].srcOffset = srcOffsets[i];
			copyInfos[i].dstSubresource.baseArrayLayer = i;
		}

		auto commandBuffer = queueData.commandPool->createCommandBuffer( "SkyboxBackground" );
		commandBuffer->begin();
		commandBuffer->beginDebugBlock(
			{
				"Cross to Skybox",
				makeFloatArray( getScene().getEngine()->getNextRainbowColour() ),
			} );
		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_crossTexture->getDefaultView().getTargetView().makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_texture->getLayerCubeView( 0u ).getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		commandBuffer->copyImage( copyInfos
			, m_crossTexture->getTexture()
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, m_texture->getTexture()
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_texture->getLayerCubeView( 0u ).getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		commandBuffer->endDebugBlock();
		commandBuffer->end();

		queueData.queue->submit( *commandBuffer, nullptr );
		queueData.queue->waitIdle();

		m_crossTexture->cleanup();
	}
}

#include "Castor3D/Scene/Background/Skybox.hpp"

#include "Castor3D/Engine.hpp"

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
		, m_viewport{ engine }
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
		getTexture().setLayerCubeFaceSource( 0u, CubeMapFace::eNegativeX, folder, relative, false );
		notifyChanged();
	}

	void SkyboxBackground::loadRightImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().setLayerCubeFaceSource( 0u, CubeMapFace::ePositiveX, folder, relative, false );
		notifyChanged();
	}

	void SkyboxBackground::loadTopImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().setLayerCubeFaceSource( 0u, CubeMapFace::eNegativeY, folder, relative, false );
		notifyChanged();
	}

	void SkyboxBackground::loadBottomImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().setLayerCubeFaceSource( 0u, CubeMapFace::ePositiveY, folder, relative, false );
		notifyChanged();
	}

	void SkyboxBackground::loadFrontImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().setLayerCubeFaceSource( 0u, CubeMapFace::eNegativeZ, folder, relative, false );
		notifyChanged();
	}

	void SkyboxBackground::loadBackImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().setLayerCubeFaceSource( 0u, CubeMapFace::ePositiveZ, folder, relative, false );
		notifyChanged();
	}

	void SkyboxBackground::loadFaceImage( castor::Path const & folder
		, castor::Path const & relative
		, CubeMapFace face )
	{
		getTexture().setLayerCubeFaceSource( 0u, face, folder, relative, false );
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
		texture->setSource( folder, relative, false );
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
		texture->setSource( folder, relative, false );
		setCrossTexture( texture );
	}

	void SkyboxBackground::setCrossTexture( TextureLayoutSPtr texture )
	{
		m_crossTexturePath = castor::Path( texture->getImage().getPath() );
		m_crossTexture = texture;
		notifyChanged();
	}

	bool SkyboxBackground::doInitialise( RenderDevice const & device
		, ashes::RenderPass const & renderPass )
	{
		CU_Require( m_texture );
		return doInitialiseTexture( device );
	}

	void SkyboxBackground::doCleanup()
	{
	}

	void SkyboxBackground::doCpuUpdate( CpuUpdater & updater )
	{
		m_viewport.setPerspective( 45.0_degrees
			, updater.camera->getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.cpuUpdate( updater.camera->getView()
			, m_viewport.getProjection() );
	}

	void SkyboxBackground::doGpuUpdate( GpuUpdater & updater )
	{
	}

	bool SkyboxBackground::doInitialiseTexture( RenderDevice const & device )
	{
		if ( m_equiTexture )
		{
			doInitialiseEquiTexture( device );
		}
		else if ( m_crossTexture )
		{
			doInitialiseCrossTexture( device );
		}

		m_hdr = m_texture->getPixelFormat() == VK_FORMAT_R32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32B32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32B32A32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16B16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16B16A16_SFLOAT;
		return m_texture->initialise( device );
	}

	void SkyboxBackground::doInitialiseEquiTexture( RenderDevice const & device )
	{
		m_equiTexture->initialise( device );

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != m_equiSize.getWidth()
			|| m_texture->getDimensions().height != m_equiSize.getHeight() )
		{
			m_texture = std::make_shared< TextureLayout >( device.renderSystem
				, doGetImageCreate( m_equiTexture->getPixelFormat(), m_equiSize, true )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "SkyboxBackgroundEquiCube" ) );
			m_texture->initialise( device );

			EquirectangularToCube equiToCube{ *m_equiTexture
				, device
				, *m_texture };
			equiToCube.render();

			if ( m_scene.getMaterialsType() != MaterialType::ePhong )
			{
				m_texture->generateMipmaps( device );
			}
		}

		m_equiTexture->cleanup();
	}

	void SkyboxBackground::doInitialiseCrossTexture( RenderDevice const & device )
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_crossTexture->initialise( device );
		auto width = m_crossTexture->getWidth() / 4u;
		auto height = m_crossTexture->getHeight() / 3u;
		CU_Require( width == height );

		// create the cube texture if needed.
		m_texture = std::make_shared< TextureLayout >( renderSystem
			, doGetImageCreate( m_crossTexture->getPixelFormat(), Size{ width, width }, true )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackgroundCrossCube" ) );
		m_texture->initialise( device );

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

		auto commandBuffer = device.graphicsCommandPool->createCommandBuffer( "SkyboxBackground" );
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

		device.graphicsQueue->submit( *commandBuffer, nullptr );
		device.graphicsQueue->waitIdle();

		m_crossTexture->cleanup();
	}
}

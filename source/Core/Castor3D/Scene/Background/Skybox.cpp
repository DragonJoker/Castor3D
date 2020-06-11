#include "Castor3D/Scene/Background/Skybox.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/ToTexture/EquirectangularToCube.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
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

	SkyboxBackground::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< SkyboxBackground >{ tabs }
	{
	}

	bool SkyboxBackground::TextWriter::operator()( SkyboxBackground const & obj, TextFile & file )
	{
		static String const faces[]
		{
			cuT( "right" ),
			cuT( "left" ),
			cuT( "bottom" ),
			cuT( "top" ),
			cuT( "back" ),
			cuT( "front" ),
		};

		auto result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "// Skybox\n" ) ) > 0;

		if ( !obj.getEquiTexturePath().empty()
			&& castor::File::fileExists( obj.getEquiTexturePath() ) )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

			if ( result )
			{
				Path subfolder{ cuT( "Textures" ) };
				String relative = Scene::TextWriter::copyFile( obj.getEquiTexturePath()
					, file.getFilePath()
					, subfolder );
				string::replace( relative, cuT( "\\" ), cuT( "/" ) );
				auto & size = obj.getEquiSize();
				result = file.writeText( m_tabs + cuT( "\tequirectangular" )
					+ cuT( " \"" ) + relative + cuT( "\" " )
					+ string::toString( size.getWidth(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SkyboxBackground >::checkError( result, "Skybox equi-texture" );

				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0
					&& result;
			}
		}
		else if ( !obj.getCrossTexturePath().empty()
			&& castor::File::fileExists( obj.getCrossTexturePath() ) )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

			if ( result )
			{
				Path subfolder{ cuT( "Textures" ) };
				String relative = Scene::TextWriter::copyFile( obj.getCrossTexturePath()
					, file.getFilePath()
					, subfolder );
				string::replace( relative, cuT( "\\" ), cuT( "/" ) );

				result = file.writeText( m_tabs + cuT( "\tcross" ) + cuT( " \"" ) + relative + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< SkyboxBackground >::checkError( result, "Skybox cross-texture" );

				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0
					&& result;
			}
		}
		else if ( castor::File::fileExists( Path{ obj.m_texture->getLayerCubeFaceView( 0, CubeMapFace( 0u ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getLayerCubeFaceView( 0, CubeMapFace( 1u ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getLayerCubeFaceView( 0, CubeMapFace( 2u ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getLayerCubeFaceView( 0, CubeMapFace( 3u ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getLayerCubeFaceView( 0, CubeMapFace( 4u ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getLayerCubeFaceView( 0, CubeMapFace( 5u ) ).toString() } ) )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			Path subfolder{ cuT( "Textures" ) };

			for ( uint32_t i = 0; i < 6 && result; ++i )
			{
				String relative = Scene::TextWriter::copyFile( Path{ obj.m_texture->getLayerCubeFaceView( 0u, CubeMapFace( i ) ).toString() }
					, file.getFilePath()
					, subfolder );
				string::replace( relative, cuT( "\\" ), cuT( "/" ) );
				result = file.writeText( m_tabs + cuT( "\t" ) + faces[i] + cuT( " \"" ) + relative + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< SkyboxBackground >::checkError( result, ( "Skybox " + faces[i] ).c_str() );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//************************************************************************************************

	SkyboxBackground::SkyboxBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eSkybox }
		, m_viewport{ engine }
	{
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate( VK_FORMAT_R8G8B8A8_UNORM, { 16u, 16u }, false )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackground_Colour" ) );
	}

	SkyboxBackground::~SkyboxBackground()
	{
	}

	void SkyboxBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	void SkyboxBackground::loadLeftImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getLayerCubeFaceView( 0u, CubeMapFace::eNegativeX ).initialiseSource( folder, relative );
		notifyChanged();
	}

	void SkyboxBackground::loadRightImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getLayerCubeFaceView( 0u, CubeMapFace::ePositiveX ).initialiseSource( folder, relative );
		notifyChanged();
	}

	void SkyboxBackground::loadTopImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getLayerCubeFaceView( 0u, CubeMapFace::eNegativeY ).initialiseSource( folder, relative );
		notifyChanged();
	}

	void SkyboxBackground::loadBottomImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getLayerCubeFaceView( 0u, CubeMapFace::ePositiveY ).initialiseSource( folder, relative );
		notifyChanged();
	}

	void SkyboxBackground::loadFrontImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getLayerCubeFaceView( 0u, CubeMapFace::eNegativeZ ).initialiseSource( folder, relative );
		notifyChanged();
	}

	void SkyboxBackground::loadBackImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getLayerCubeFaceView( 0u, CubeMapFace::ePositiveZ ).initialiseSource( folder, relative );
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
		texture->getDefaultView().initialiseSource( folder, relative );
		setEquiTexture( texture, size );
	}

	void SkyboxBackground::setEquiTexture( TextureLayoutSPtr texture
		, uint32_t size )
	{
		m_equiTexturePath = castor::Path( texture->getDefaultView().toString() );
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
		texture->getDefaultView().initialiseSource( folder, relative );
		setCrossTexture( texture );
	}

	void SkyboxBackground::setCrossTexture( TextureLayoutSPtr texture )
	{
		m_crossTexturePath = castor::Path( texture->getDefaultView().toString() );
		m_crossTexture = texture;
		notifyChanged();
	}

	bool SkyboxBackground::doInitialise( ashes::RenderPass const & renderPass )
	{
		CU_Require( m_texture );
		return doInitialiseTexture();
	}

	void SkyboxBackground::doCleanup()
	{
	}

	void SkyboxBackground::doUpdate( Camera const & camera )
	{
		m_viewport.setPerspective( 45.0_degrees
			, camera.getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.update( camera.getView()
			, m_viewport.getProjection() );
	}

	bool SkyboxBackground::doInitialiseTexture()
	{
		if ( m_equiTexture )
		{
			doInitialiseEquiTexture();
		}
		else if ( m_crossTexture )
		{
			doInitialiseCrossTexture();
		}

		m_hdr = m_texture->getPixelFormat() == VK_FORMAT_R32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32B32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R32G32B32A32_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16B16_SFLOAT
			|| m_texture->getPixelFormat() == VK_FORMAT_R16G16B16A16_SFLOAT;
		return m_texture->initialise();
	}

	void SkyboxBackground::doInitialiseEquiTexture()
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );
		m_equiTexture->initialise();

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != m_equiSize.getWidth()
			|| m_texture->getDimensions().height != m_equiSize.getHeight() )
		{
			m_texture = std::make_shared< TextureLayout >( renderSystem
				, doGetImageCreate( m_equiTexture->getPixelFormat(), m_equiSize, true )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "SkyboxBackgroundEquiCube" ) );
			m_texture->getLayerCubeFaceView( 0u, CubeMapFace::ePositiveX ).initialiseSource();
			m_texture->getLayerCubeFaceView( 0u, CubeMapFace::eNegativeX ).initialiseSource();
			m_texture->getLayerCubeFaceView( 0u, CubeMapFace::ePositiveY ).initialiseSource();
			m_texture->getLayerCubeFaceView( 0u, CubeMapFace::eNegativeY ).initialiseSource();
			m_texture->getLayerCubeFaceView( 0u, CubeMapFace::ePositiveZ ).initialiseSource();
			m_texture->getLayerCubeFaceView( 0u, CubeMapFace::eNegativeZ ).initialiseSource();
			m_texture->initialise();

			EquirectangularToCube equiToCube{ *m_equiTexture
				, device
				, *m_texture };
			equiToCube.render();

			if ( m_scene.getMaterialsType() != MaterialType::ePhong )
			{
				m_texture->generateMipmaps();
			}
		}

		m_equiTexture->cleanup();
	}

	void SkyboxBackground::doInitialiseCrossTexture()
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_crossTexture->initialise();
		auto width = m_crossTexture->getWidth() / 4u;
		auto height = m_crossTexture->getHeight() / 3u;
		CU_Require( width == height );

		// create the cube texture if needed.
		m_texture = std::make_shared< TextureLayout >( renderSystem
			, doGetImageCreate( m_crossTexture->getPixelFormat(), Size{ width, width }, true )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackgroundCrossCube" ) );
		m_texture->getLayerCubeFaceView( 0u, CubeMapFace::ePositiveX ).initialiseSource();
		m_texture->getLayerCubeFaceView( 0u, CubeMapFace::eNegativeX ).initialiseSource();
		m_texture->getLayerCubeFaceView( 0u, CubeMapFace::ePositiveY ).initialiseSource();
		m_texture->getLayerCubeFaceView( 0u, CubeMapFace::eNegativeY ).initialiseSource();
		m_texture->getLayerCubeFaceView( 0u, CubeMapFace::ePositiveZ ).initialiseSource();
		m_texture->getLayerCubeFaceView( 0u, CubeMapFace::eNegativeZ ).initialiseSource();
		m_texture->initialise();

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
		VkImageCopy copyInfos[6];
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].extent = { width, width, 1u };
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].srcOffset.x = width * 2;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].srcOffset.y = width;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].srcOffset.z = 0u;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::ePositiveX );
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].dstOffset = { 0u, 0u, 0u };

		copyInfos[uint32_t( CubeMapFace::eNegativeX )].extent = { width, width, 1u };
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].srcOffset.x = 0u;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].srcOffset.y = width;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].srcOffset.z = 0u;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::eNegativeX );
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].dstOffset = { 0u, 0u, 0u };

		copyInfos[uint32_t( CubeMapFace::ePositiveY )].extent = { width, width, 1u };
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].srcOffset.x = width;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].srcOffset.y = 0u;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].srcOffset.z = 0u;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::ePositiveY );
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].dstOffset = { 0u, 0u, 0u };

		copyInfos[uint32_t( CubeMapFace::eNegativeY )].extent = { width, width, 1u };
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].srcOffset.x = width;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].srcOffset.y = width * 2u;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].srcOffset.z = 0u;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::eNegativeY );
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].dstOffset = { 0u, 0u, 0u };

		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].extent = { width, width, 1u };
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].srcOffset.x = width;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].srcOffset.y = width;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].srcOffset.z = 0u;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::ePositiveZ );
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].dstOffset = { 0u, 0u, 0u };

		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].extent = { width, width, 1u };
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].srcOffset.x = width * 3u;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].srcOffset.y = width;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].srcOffset.z = 0u;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::eNegativeZ );
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].dstOffset = { 0u, 0u, 0u };

		auto & device = getCurrentRenderDevice( renderSystem );
		auto commandBuffer = device.graphicsCommandPool->createCommandBuffer( "SkyboxBackground" );
		commandBuffer->begin();
		commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_crossTexture->getDefaultView().getTargetView().makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		uint32_t index{ 0u };

		for ( auto & copyInfo : copyInfos )
		{
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_texture->getLayerCubeFaceView( 0u, CubeMapFace( index ) ).getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			commandBuffer->copyImage( copyInfo
				, m_crossTexture->getTexture()
				, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, m_texture->getTexture()
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_texture->getLayerCubeFaceView( 0u, CubeMapFace( index ) ).getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
			++index;
		}

		commandBuffer->end();

		device.graphicsQueue->submit( *commandBuffer, nullptr );
		device.graphicsQueue->waitIdle();

		m_crossTexture->cleanup();
	}
}

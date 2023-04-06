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

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	using namespace castor3d;

	template<>
	class TextWriter< castor3d::SkyboxBackground >
		: public TextWriterT< castor3d::SkyboxBackground >
	{
	public:
		explicit TextWriter( castor::String const & tabs
			, Path const & folder )
			: TextWriterT< SkyboxBackground >{ tabs }
			, m_folder{ folder }
		{
		}

		bool operator()( SkyboxBackground const & background
			, castor::StringStream & file )override
		{
			log::info << tabs() << cuT( "Writing SkyboxBackground" ) << std::endl;
			static String const faces[]
			{
				cuT( "right" ),
				cuT( "left" ),
				cuT( "bottom" ),
				cuT( "top" ),
				cuT( "back" ),
				cuT( "front" ),
			};

			auto result = true;
			file << ( cuT( "\n" ) + tabs() + cuT( "//Skybox\n" ) );

			if ( !background.getEquiTexturePath().empty()
				&& castor::File::fileExists( background.getEquiTexturePath() ) )
			{
				if ( auto block{ beginBlock( file, "skybox" ) } )
				{
					Path subfolder{ cuT( "Textures" ) };
					String relative = copyFile( background.getEquiTexturePath()
						, m_folder
						, subfolder );
					string::replace( relative, cuT( "\\" ), cuT( "/" ) );
					auto & size = background.getEquiSize();
					file << ( tabs() + cuT( "equirectangular" )
						+ cuT( " \"" ) + relative + cuT( "\" " )
						+ string::toString( size.getWidth() ) + cuT( "\n" ) );
					castor::TextWriter< SkyboxBackground >::checkError( result, "Skybox equi-texture" );
				}
			}
			else if ( !background.getCrossTexturePath().empty()
				&& castor::File::fileExists( background.getCrossTexturePath() ) )
			{
				result = false;

				if ( auto block{ beginBlock( file, "skybox" ) } )
				{
					result = writeFile( file, cuT( "cross" ), background.getCrossTexturePath(), m_folder, cuT( "Textures" ) );
				}
			}
			else if ( castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 0u ) ).toString() } )
				&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 1u ) ).toString() } )
				&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 2u ) ).toString() } )
				&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 3u ) ).toString() } )
				&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 4u ) ).toString() } )
				&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 5u ) ).toString() } ) )
			{
				result = false;

				if ( auto block{ beginBlock( file, "skybox" ) } )
				{
					result = true;

					Path subfolder{ cuT( "Textures" ) };

					for ( uint32_t i = 0; i < 6 && result; ++i )
					{
						result = writeFile( file
							, faces[i]
							, Path{ background.getTexture().getLayerCubeFaceView( 0u, CubeMapFace( i ) ).toString() }
							, m_folder
							, cuT( "Textures" ) );
					}
				}
			}

			return result;
		}

	private:
		Path m_folder;
	};
}

namespace castor3d
{
	//************************************************************************************************

	namespace skybox
	{
		static ashes::ImageCreateInfo doGetImageCreate( VkFormat format
			, castor::Size const & dimensions
			, bool attachment
			, uint32_t mipLevel = 1u )
		{
			return ashes::ImageCreateInfo{ VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, VK_IMAGE_TYPE_2D
				, format
				, { dimensions.getWidth(), dimensions.getHeight(), 1u }
				, mipLevel
				, 6u
				, VK_SAMPLE_COUNT_1_BIT
				, VK_IMAGE_TILING_OPTIMAL
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VkImageUsageFlags( attachment
						? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
						: VkImageUsageFlagBits( 0u ) ) ) };
		}
	}

	//************************************************************************************************

	SkyboxBackground::SkyboxBackground( Engine & engine
		, Scene & scene
		, castor::String const & name )
		: SceneBackground{ engine, scene, name + cuT( "Skybox" ), cuT( "skybox" ), true }
	{
		m_texture = castor::makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, skybox::doGetImageCreate( VK_FORMAT_R8G8B8A8_UNORM, { 16u, 16u }, false )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackground_Colour" )
			, true /* isStatic */ );
	}

	void SkyboxBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	void SkyboxBackground::accept( PipelineVisitor & visitor )
	{
	}

	bool SkyboxBackground::write( castor::String const & tabs
		, castor::Path const & folder
		, castor::StringStream & stream )const
	{
		return castor::TextWriter< SkyboxBackground >{ tabs, folder }( *this, stream );
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
		auto texture = castor::makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackground" ) + castor3d::getName( face ) );
		texture->setSource( folder, relative, { false, false, false } );
		m_layerTexturePath[size_t( face )] = texture->getImage().getPath();
		m_layerTexture[size_t( face )] = std::move( texture );
		notifyChanged();
	}

	void SkyboxBackground::loadEquiTexture( castor::Path const & folder
		, castor::Path const & relative
		, uint32_t size )
	{
		ashes::ImageCreateInfo image{ 0u
			, VK_IMAGE_TYPE_2D
			, VK_FORMAT_UNDEFINED
			, { size, size, 1u }
			, 1u
			, 1u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_IMAGE_TILING_OPTIMAL
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) };
		auto texture = castor::makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackgroundEquirectangular" ) );
		texture->setSource( folder, relative, { false, false, false } );
		setCrossTexture( std::move( texture ) );
	}

	void SkyboxBackground::setEquiTexture( TextureLayoutUPtr texture
		, uint32_t size )
	{
		m_equiTexturePath = texture->getImage().getPath();
		m_equiTexture = std::move( texture );
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
		auto texture = castor::makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackgroundCross" ) );
		texture->setSource( folder, relative, { false, false, false } );
		setCrossTexture( std::move( texture ) );
	}

	void SkyboxBackground::setCrossTexture( TextureLayoutUPtr texture )
	{
		m_crossTexturePath = texture->getImage().getPath();
		m_crossTexture = std::move( texture );
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
			, updater.camera->getNear()
			, updater.camera->getFar() );
		viewport.update();
		updater.bgMtxView = updater.camera->getView();
		updater.bgMtxProj = updater.isSafeBanded
			? viewport.getSafeBandedProjection()
			: viewport.getProjection();
	}

	void SkyboxBackground::doGpuUpdate( GpuUpdater & updater )const
	{
	}

	void SkyboxBackground::doAddPassBindings( crg::FramePass & pass
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		pass.addSampledView( m_textureId.wholeViewId
			, index++
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
	}

	void SkyboxBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBackground
	}

	void SkyboxBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		bindTexture( m_textureId.wholeView
			, *m_textureId.sampler
			, descriptorWrites
			, index );
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
		m_srgb = isSRGBFormat( convert( m_texture->getPixelFormat() ) );
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

		// create the cube texture if needed.
		m_textureId = { device
			, getScene().getResources()
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
		m_texture = castor::makeUnique< TextureLayout >( device.renderSystem
			, m_textureId.image
			, m_textureId.wholeViewId );

		auto commandBuffer = queueData.commandPool->createCommandBuffer( "SkyboxBackground" );
		commandBuffer->begin();
		commandBuffer->beginDebugBlock(
			{
				"Layer to Skybox",
				makeFloatArray( engine.getNextRainbowColour() ),
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
				, getScene().getResources()
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
			m_texture = castor::makeUnique< TextureLayout >( device.renderSystem
				, m_textureId.image
				, m_textureId.wholeViewId );

			EquirectangularToCube equiToCube{ *m_equiTexture
				, device
				, *m_texture };
			equiToCube.render( queueData );
			m_texture->generateMipmaps( queueData );
		}

		m_equiTexture->cleanup();
	}

	void SkyboxBackground::doInitialiseCrossTexture( RenderDevice const & device
		, QueueData const & queueData )
	{
		m_crossTexture->initialise( device, queueData );
		auto width = m_crossTexture->getWidth() / 4u;
		auto height = m_crossTexture->getHeight() / 3u;
		CU_Require( width == height );

		// create the cube texture if needed.
		m_textureId = { device
			, getScene().getResources()
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
		m_texture = castor::makeUnique< TextureLayout >( device.renderSystem
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

#include "Castor3D/Scene/Background/Image.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslImgBackground.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

CU_ImplementSmartPtr( c3d, ImageBackground )

namespace c3d
{
	template<>
	class TextWriter< ImageBackground >
		: public TextWriterT< ImageBackground >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder )
			: TextWriterT< ImageBackground >{ tabs }
			, m_folder{ folder }
		{
		}

		bool operator()( ImageBackground const & background
			, StringStream & file )override
		{
			log::info << tabs() << cuT( "Writing ImageBackground" ) << std::endl;
			return writeFile( file
				, cuT( "background_image" )
				, background.getImagePath()
				, m_folder
				, cuT( "Textures" ) );
		}

	private:
		Path const & m_folder;
	};

	//************************************************************************************************

	namespace bgimage
	{
		static ImageCreateInfo doGetImageCreate( PixelFormat format
			, Size const & dimensions
			, bool attachment
			, uint32_t mipLevel = 1u )
		{
			return ImageCreateInfo{ ImageCreateFlags::eCubeCompatible
				, ImageType::e2D
				, format
				, { dimensions.getWidth(), dimensions.getHeight(), 1u }
				, mipLevel
				, 6u
				, SampleCount::e1
				, ImageTiling::eOptimal
				, ( ImageUsageFlags::eSampled
					| ImageUsageFlags::eTransferDst
					| ( attachment ? ImageUsageFlags::eColorAttachment : ImageUsageFlags::eNone ) ) };
		}
	}

	//************************************************************************************************

	ImageBackground::ImageBackground( Engine & engine
		, Scene & scene
		, String const & name )
		: SceneBackground{ engine, scene, name + cuT( "Image" ), cuT( "image" ), false }
	{
		m_texture = makeUnique< TextureLayout >( *engine.getRenderSystem()
			, bgimage::doGetImageCreate( PixelFormat::eR8G8B8A8_UNORM, { 16u, 16u }, false )
			, MemoryPropertyFlags::eDeviceLocal
			, cuT( "ImageBackground_Dummy" ) );
	}

	void ImageBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	void ImageBackground::accept( ConfigurationVisitorBase & visitor )
	{
	}

	bool ImageBackground::write( String const & tabs
		, Path const & folder
		, StringStream & stream )const
	{
		return TextWriter< ImageBackground >{ tabs, folder }( *this, stream );
	}

	bool ImageBackground::setImage( Path const & folder, Path const & relative )
	{
		bool result = false;

		try
		{
			ImageCreateInfo image{ ImageCreateFlags::eNone
				, ImageType::e2D
				, PixelFormat::eUNDEFINED
				, { 1u, 1u, 1u }
				, 1u
				, 1u
				, SampleCount::e1
				, ImageTiling::eOptimal
				, ( ImageUsageFlags::eTransferSrc | ImageUsageFlags::eTransferDst ) };
			m_2dTexture = makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
				, c3d::move( image )
				, MemoryPropertyFlags::eDeviceLocal
				, cuT( "SkyboxBackground2D" ) );
			m_2dTexture->setSource( folder, relative );

			m_2dTexturePath = folder / relative;
			notifyChanged();
			result = true;
		}
		catch ( Exception & exc )
		{
			log::error << exc.what() << std::endl;
		}

		return result;
	}

	String const & ImageBackground::getModelName()const
	{
		return shader::ImgBackgroundModel::Name;
	}

	bool ImageBackground::doInitialise( RenderDevice const & device )
	{
		doInitialise2DTexture( device );
		m_hdr = m_texture->getPixelFormat() == PixelFormat::eR32_SFLOAT
			|| m_texture->getPixelFormat() == PixelFormat::eR32G32_SFLOAT
			|| m_texture->getPixelFormat() == PixelFormat::eR32G32B32_SFLOAT
			|| m_texture->getPixelFormat() == PixelFormat::eR32G32B32A32_SFLOAT
			|| m_texture->getPixelFormat() == PixelFormat::eR16_SFLOAT
			|| m_texture->getPixelFormat() == PixelFormat::eR16G16_SFLOAT
			|| m_texture->getPixelFormat() == PixelFormat::eR16G16B16_SFLOAT
			|| m_texture->getPixelFormat() == PixelFormat::eR16G16B16A16_SFLOAT;
		m_srgb = isSRGBFormat( m_texture->getPixelFormat() );
		return m_texture->initialise( device );
	}

	void ImageBackground::doCleanup()
	{
	}

	void ImageBackground::doCpuUpdate( CpuUpdater & updater )const
	{
		auto & viewport = *updater.viewport;
		viewport.setOrtho( -1.0f
			, 1.0f
			, -m_ratio
			, m_ratio
			, 0.1f
			, 2.0f );
		viewport.update();
		auto node = updater.camera->getParent();
		Matrix4x4f view;
		matrix::lookAt( view
			, node->getDerivedPosition()
			, node->getDerivedPosition() + Point3f{ 0.0f, 0.0f, 1.0f }
			, Point3f{ 0.0f, 1.0f, 0.0f } );
		updater.bgMtxView = view;
		updater.bgMtxProj = updater.isSafeBanded
			? viewport.getSafeBandedProjection( updater.renderSize )
			: viewport.getProjection();
	}

	void ImageBackground::doGpuUpdate( GpuUpdater & updater )const
	{
	}

	void ImageBackground::doUpload( UploadData & uploader )
	{
	}

	void ImageBackground::doAddPassBindings( crg::FramePass & pass
		, Texture * targetImage
		, uint32_t & index )const
	{
		pass.addInputSampledImage( m_textureId.getWholeViewId()
			, index
			, crg::SamplerDesc{ FilterMode::eLinear
				, FilterMode::eLinear
				, MipmapMode::eLinear } );
		++index;
	}

	void ImageBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages ) );	// c3d_mapBackground
		++index;
	}

	void ImageBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, Texture * targetImage
		, uint32_t & index )const
	{
		bindTexture( m_textureId.getSampledView()
			, *m_textureId.sampler
			, descriptorWrites
			, index );
	}

	void ImageBackground::doInitialise2DTexture( RenderDevice const & device )
	{
		auto data = device.graphicsData();
		auto & queueData = *data;
		m_2dTexture->initialise( device );
		{
			auto & image = m_2dTexture->getImage();
			auto & texture = m_2dTexture->getTexture();
			InstantDirectUploadData upload{ *queueData.queue
				, device, image.getName() + cuT( "/Upload" ), *queueData.commandPool };
			upload->pushUpload( image.getPxBuffer().getConstPtr()
				, image.getPxBuffer().getSize()
				, texture
				, image.getLayout()
				, { ImageAspectFlags::eColor, 0u, 1u, 0u, image.getLayout().depthLayers() }
				, ImageLayout::eTransferSrc, PipelineStageFlags::eTransfer );
		}

		VkExtent3D extent{ m_2dTexture->getWidth(), m_2dTexture->getHeight(), 1u };
		auto dim = std::max( extent.width, extent.height );

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != dim
			|| m_texture->getDimensions().height != dim )
		{
			m_ratio = float( extent.height ) / float( extent.width );
			m_textureId = Texture{ device
				, getScene().getResources()
				, cuT( "ImageBackgroundCube" )
				, { ImageCreateFlags::eCubeCompatible
					, { dim, dim, 1u }, 6u, 1u
					, m_2dTexture->getPixelFormat()
					, ImageUsageFlags::eSampled | ImageUsageFlags::eTransferDst }
				, {} };
			m_textureId.create();
			m_texture = makeUnique< TextureLayout >( device.renderSystem
				, cuT( "ImageBackgroundCube" )
				, *m_textureId.image
				, m_textureId.getWholeViewId() );
		}

		auto xOffset = ( dim - extent.width ) / 2u;
		auto yOffset = ( dim - extent.height ) / 2u;
		VkOffset3D const srcOffset{ 0, 0, 0 };
		VkOffset3D const dstOffset{ int32_t( xOffset ), int32_t( yOffset ), 0 };
		VkImageSubresourceLayers srcSubresource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		VkImageSubresourceLayers dstSubresource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };

		Array< VkImageCopy, 6u > copyInfos;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].extent = extent;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].srcOffset = srcOffset;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::ePositiveX );
		copyInfos[uint32_t( CubeMapFace::ePositiveX )].dstOffset = dstOffset;

		copyInfos[uint32_t( CubeMapFace::eNegativeX )].extent = extent;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].srcOffset = srcOffset;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::eNegativeX );
		copyInfos[uint32_t( CubeMapFace::eNegativeX )].dstOffset = dstOffset;

		copyInfos[uint32_t( CubeMapFace::ePositiveY )].extent = extent;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].srcOffset = srcOffset;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::ePositiveY );
		copyInfos[uint32_t( CubeMapFace::ePositiveY )].dstOffset = dstOffset;

		copyInfos[uint32_t( CubeMapFace::eNegativeY )].extent = extent;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].srcOffset = srcOffset;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::eNegativeY );
		copyInfos[uint32_t( CubeMapFace::eNegativeY )].dstOffset = dstOffset;

		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].extent = extent;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].srcOffset = srcOffset;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::ePositiveZ );
		copyInfos[uint32_t( CubeMapFace::ePositiveZ )].dstOffset = dstOffset;

		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].extent = extent;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].srcSubresource = srcSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].srcOffset = srcOffset;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].dstSubresource = dstSubresource;
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].dstSubresource.baseArrayLayer = uint32_t( CubeMapFace::eNegativeZ );
		copyInfos[uint32_t( CubeMapFace::eNegativeZ )].dstOffset = dstOffset;

		auto commandBuffer = queueData.commandPool->createCommandBuffer( "ImageBackground" );
		commandBuffer->begin();
		uint32_t index{ 0u };

		for ( auto const & copyInfo : copyInfos )
		{
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_texture->getLayerCubeFaceTargetView( 0, CubeMapFace( index ) ).makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			commandBuffer->copyImage( copyInfo
				, m_2dTexture->getTexture()
				, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, m_texture->getTexture()
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_texture->getLayerCubeFaceTargetView( 0, CubeMapFace( index ) ).makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
			++index;
		}

		commandBuffer->end();

		queueData.queue->submit( *commandBuffer, nullptr );
		queueData.queue->waitIdle();

		m_2dTexture->cleanup();
	}

	//************************************************************************************************
}

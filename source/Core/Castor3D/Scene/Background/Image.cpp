#include "Castor3D/Scene/Background/Image.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Scene/Background/Shaders/GlslImgBackground.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	using namespace castor3d;

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
				, "background_image"
				, castor::Path{ background.getTexture().getDefaultView().toString() }
				, m_folder
				, cuT( "Textures" ) );
		}

	private:
		Path const & m_folder;
	};
}

namespace castor3d
{
	//************************************************************************************************

	namespace bgimage
	{
		static ashes::ImageCreateInfo doGetImageCreate( VkFormat format
			, castor::Size const & dimensions
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

	ImageBackground::ImageBackground( Engine & engine
		, Scene & scene
		, castor::String const & name )
		: SceneBackground{ engine, scene, name + cuT( "Image" ), cuT( "image" ) }
	{
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, bgimage::doGetImageCreate( VK_FORMAT_R8G8B8A8_UNORM, { 16u, 16u }, false )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "ImageBackground_Dummy" ) );
	}

	bool ImageBackground::loadImage( castor::Path const & folder, castor::Path const & relative )
	{
		bool result = false;

		try
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
			auto texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "ImageBackground_Colour" ) );
			texture->setSource( folder, relative, { false, false, false } );
			m_2dTexture = texture;
			m_2dTexturePath = castor::Path( m_2dTexture->getDefaultView().toString() );
			notifyChanged();
			result = true;
		}
		catch ( castor::Exception & p_exc )
		{
			log::error << p_exc.what() << std::endl;
		}

		return result;
	}

	void ImageBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	bool ImageBackground::write( castor::String const & tabs
		, castor::Path const & folder
		, castor::StringStream & stream )const
	{
		return castor::TextWriter< ImageBackground >{ tabs, folder }( *this, stream );
	}

	castor::String const & ImageBackground::getModelName()const
	{
		return shader::ImgBackgroundModel::Name;
	}

	bool ImageBackground::doInitialise( RenderDevice const & device )
	{
		auto data = device.graphicsData();
		doInitialise2DTexture( device, *data );
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

	void ImageBackground::doCleanup()
	{
	}

	void ImageBackground::doCpuUpdate( CpuUpdater & updater )const
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
	}

	void ImageBackground::doGpuUpdate( GpuUpdater & updater )const
	{
	}

	void ImageBackground::doAddPassBindings( crg::FramePass & pass
		, uint32_t & index )const
	{
		pass.addSampledView( m_textureId.wholeViewId
			, index++
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR } );
	}

	void ImageBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBackground
	}

	void ImageBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		bindTexture( m_textureId.wholeView
			, *m_textureId.sampler
			, descriptorWrites
			, index );
	}

	void ImageBackground::doInitialise2DTexture( RenderDevice const & device
		, QueueData const & queueData )
	{
		m_2dTexture->initialise( device, queueData );
		VkExtent3D extent{ m_2dTexture->getWidth(), m_2dTexture->getHeight(), 1u };
		auto dim = std::max( m_2dTexture->getWidth(), m_2dTexture->getHeight() );

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != dim
			|| m_texture->getDimensions().height != dim )
		{
			m_ratio = float( m_2dTexture->getHeight() ) / float( m_2dTexture->getWidth() );
			auto xOffset = ( dim - extent.width ) / 2u;
			auto yOffset = ( dim - extent.height ) / 2u;
			VkOffset3D const srcOffset{ 0, 0, 0 };
			VkOffset3D const dstOffset{ int32_t( xOffset ), int32_t( yOffset ), 0 };
			m_textureId = { device
				, getEngine()->getGraphResourceHandler()
				, cuT( "ImageBackgroundCube" )
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, { dim, dim, 1u }
				, 6u
				, 1u
				, m_2dTexture->getPixelFormat()
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) };
			m_textureId.create();
			m_texture = std::make_shared< TextureLayout >( device.renderSystem
				, m_textureId.image
				, m_textureId.wholeViewId );

			VkImageSubresourceLayers srcSubresource
			{
				m_2dTexture->getDefaultView().getTargetView()->subresourceRange.aspectMask,
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
			commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_2dTexture->getDefaultView().getTargetView().makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
			uint32_t index{ 0u };

			for ( auto & copyInfo : copyInfos )
			{
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
					, VK_PIPELINE_STAGE_TRANSFER_BIT
					, m_texture->getLayerCubeFaceView( 0, CubeMapFace( index ) ).getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
				commandBuffer->copyImage( copyInfo
					, m_2dTexture->getTexture()
					, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
					, m_texture->getTexture()
					, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
				commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, m_texture->getLayerCubeFaceView( 0, CubeMapFace( index ) ).getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
				++index;
			}

			commandBuffer->end();

			queueData.queue->submit( *commandBuffer, nullptr );
			queueData.queue->waitIdle();

			m_2dTexture->cleanup();
		}
	}

	//************************************************************************************************
}

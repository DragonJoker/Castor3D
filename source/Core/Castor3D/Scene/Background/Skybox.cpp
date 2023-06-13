#include "Castor3D/Scene/Background/Skybox.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
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

CU_ImplementSmartPtr( castor3d, SkyboxBackground )

#pragma GCC diagnostic ignored "-Wrestrict"

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

			if ( auto block{ beginBlock( file, "skybox" ) } )
			{
				result = writeOpt( file, cuT( "visible" ), background.isVisible(), true );

				if ( !background.getEquiTexturePath().empty()
					&& castor::File::fileExists( background.getEquiTexturePath() ) )
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
				else if ( !background.getCrossTexturePath().empty()
					&& castor::File::fileExists( background.getCrossTexturePath() ) )
				{
					result = writeFile( file, cuT( "cross" ), background.getCrossTexturePath(), m_folder, cuT( "Textures" ) );
				}
				else if ( castor::File::fileExists( background.getLayerTexturePath()[0u] )
					&& castor::File::fileExists( background.getLayerTexturePath()[1u] )
					&& castor::File::fileExists( background.getLayerTexturePath()[2u] )
					&& castor::File::fileExists( background.getLayerTexturePath()[3u] )
					&& castor::File::fileExists( background.getLayerTexturePath()[4u] )
					&& castor::File::fileExists( background.getLayerTexturePath()[5u] ) )
				{
					result = true;

					Path subfolder{ cuT( "Textures" ) };

					for ( uint32_t i = 0; i < 6 && result; ++i )
					{
						result = writeFile( file
							, faces[i]
							, background.getLayerTexturePath()[i]
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

	void SkyboxBackground::setLeftImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::eNegativeX );
	}

	void SkyboxBackground::setRightImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::ePositiveX );
	}

	void SkyboxBackground::setTopImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::eNegativeY );
	}

	void SkyboxBackground::setBottomImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::ePositiveY );
	}

	void SkyboxBackground::setFrontImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::eNegativeZ );
	}

	void SkyboxBackground::setBackImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		setFaceTexture( folder, relative, CubeMapFace::ePositiveZ );
	}

	void SkyboxBackground::setFaceTexture( castor::Path const & folder
		, castor::Path const & relative
		, CubeMapFace face )
	{
		m_layerTexturePath[size_t( face )] = folder / relative;
		m_layerTexture[size_t( face )] = SceneBackground::loadImage( *getScene().getEngine()
			, cuT( "SkyboxBackground" ) + castor3d::getName( face )
			, folder
			, relative
			, true );
		notifyChanged();
	}

	void SkyboxBackground::setEquiTexture( castor::Path const & folder
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
		m_equiTexture = castor::makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "SkyboxBackgroundEquirectangular" ) );
		m_equiTexture->setSource( folder, relative, { false, false, false } );

		m_equiTexturePath = folder / relative;
		m_equiSize.set( size, size );
		notifyChanged();
	}

	void SkyboxBackground::setEquiSize( uint32_t size )
	{
		m_equiSize.set( size, size );
		notifyChanged();
	}

	void SkyboxBackground::setCrossTexture( castor::Path const & folder
		, castor::Path const & relative )
	{
		m_crossTexturePath = folder / relative;
		m_layerTexture = splitCrossImageBuffer( *getScene().getEngine()
			, getFileImage( *getScene().getEngine()
				, cuT( "SkyboxBackgroundCross" )
				, folder
				, relative ) );
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

	void SkyboxBackground::doUpload( UploadData & uploader )
	{
		VkImageSubresourceRange dstSubresource{ VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u };

		for ( auto & layer : m_layerTexture )
		{
			dstSubresource.levelCount = layer->getLevels();
			uploader.pushUpload( layer->getPxBuffer().getConstPtr()
				, layer->getPxBuffer().getSize()
				, m_texture->getTexture()
				, layer->getLayout()
				, dstSubresource
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );
			dstSubresource.baseArrayLayer++;
		}
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
		if ( m_equiTexture )
		{
			doInitialiseEquiTexture( device );
		}
		else
		{
			doInitialiseLayerTexture( device );
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
		return m_texture->initialise( device );
	}

	void SkyboxBackground::doInitialiseLayerTexture( RenderDevice const & device )
	{
		uint32_t maxDim{};

		for ( auto & layer : m_layerTexture )
		{
			auto dim = layer->getDimensions();
			maxDim = std::max( maxDim
				, std::max( dim->x, dim->y ) );
		}

		// create the cube texture if needed.
		m_textureId = { device
			, getScene().getResources()
			, cuT( "SkyboxBackgroundLayerCube" )
			, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
			, { maxDim, maxDim, 1u }
			, 6u
			, ashes::getMaxMipCount( { maxDim, maxDim, maxDim } )
			, convert( m_layerTexture[0]->getPxBuffer().getFormat() )
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) };
		m_textureId.create();
		m_texture = castor::makeUnique< TextureLayout >( device.renderSystem
			, cuT( "SkyboxBackgroundLayerCube" )
			, *m_textureId.image
			, m_textureId.wholeViewId );
		m_needsUpload = true;
	}

	void SkyboxBackground::doInitialiseEquiTexture( RenderDevice const & device )
	{
		auto data = device.graphicsData();
		auto & queueData = *data;
		m_equiTexture->initialise( device );
		{
			auto & image = m_equiTexture->getImage();
			auto & texture = m_equiTexture->getTexture();
			InstantDirectUploadData upload{ *queueData.queue
				, device
				, image.getName()
				, *queueData.commandPool };
			upload->pushUpload( image.getPxBuffer().getConstPtr()
				, image.getPxBuffer().getSize()
				, texture
				, image.getLayout()
				, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, image.getLayout().depthLayers() }
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );
		}

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
				, ashes::getMaxMipCount( makeExtent3D( m_equiSize ) )
				, m_equiTexture->getPixelFormat()
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) };
			m_textureId.create();
			m_texture = castor::makeUnique< TextureLayout >( device.renderSystem
				, cuT( "SkyboxBackgroundEquiCube" )
				, *m_textureId.image
				, m_textureId.wholeViewId );
		}

		EquirectangularToCube equiToCube{ *m_equiTexture
			, device
			, *m_texture };
		equiToCube.render( queueData );
		m_texture->generateMipmaps( queueData
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		m_equiTexture->cleanup();
	}

	castor::ImageUPtr SkyboxBackground::copyCrossImageFace( Engine & engine
		, castor::StringView faceName
		, castor::Image const & lines
		, uint32_t index )
	{
		auto name = lines.getName() + faceName.data();
		auto height = lines.getHeight();
		auto width = height;
		auto blockExtent = ashes::getMinimalExtent2D( convert( lines.getPixelFormat() ) );
		auto blockSize = ashes::getMinimalSize( convert( lines.getPixelFormat() ) );
		auto lineSize = lines.getWidth() * blockSize;
		auto sectionSize = lineSize / 4u;
		auto lineOffset = ptrdiff_t( index * sectionSize );
		auto buffer = castor::PxBufferBase::create( { width, height }, lines.getPixelFormat() );
		auto srcData = lines.getBuffer().data() + lineOffset;
		auto dstData = buffer->getPtr();
		auto blockCount = height / blockExtent.height;

		for ( uint32_t i = 0u; i < blockCount; ++i )
		{
			std::memcpy( dstData, srcData, sectionSize );
			srcData += lineSize;
			dstData += sectionSize;
		}

		buffer = adaptBuffer( engine
			, *buffer
			, name
			, true );
		castor::ImageLayout layout{ lines.getLayout().type, *buffer };
		log::info << "Loaded image [" << name << "] (" << layout << ")" << std::endl;
		return castor::makeUnique< castor::Image >( lines.getName()
			, lines.getPath()
			, layout
			, std::move( buffer ) );
	}

	std::array< castor::ImageUPtr, 6u > SkyboxBackground::splitCrossImageBuffer( Engine & engine
		, castor::Image const & cross )
	{
		auto height = cross.getHeight() / 3u;
		CU_Require( cross.getWidth() / 4u == height );
		uint8_t const * buffer = cross.getBuffer().data();

		// First, split vertically, since it's the most straightforward.
		auto linesStride = height * cross.getPxBuffer().getSize() / cross.getHeight();
		std::array< castor::ImageUPtr, 3u > lines{
			castor::makeUnique< castor::Image >( cross.getName(), cross.getPath(), castor::Size{ cross.getWidth(), height }, cross.getPixelFormat(), buffer + ptrdiff_t( linesStride * 0 ), cross.getPixelFormat() ),
			castor::makeUnique< castor::Image >( cross.getName(), cross.getPath(), castor::Size{ cross.getWidth(), height }, cross.getPixelFormat(), buffer + ptrdiff_t( linesStride * 1 ), cross.getPixelFormat() ),
			castor::makeUnique< castor::Image >( cross.getName(), cross.getPath(), castor::Size{ cross.getWidth(), height }, cross.getPixelFormat(), buffer + ptrdiff_t( linesStride * 2 ), cross.getPixelFormat() ) };

		// Then split horizontally
		return { copyCrossImageFace( engine, cuT( "/Face/Left" ), *lines[1], 2u )
			, copyCrossImageFace( engine, cuT( "/Face/Right" ), *lines[1], 0u )
			, copyCrossImageFace( engine, cuT( "/Face/Top" ), *lines[0], 1u )
			, copyCrossImageFace( engine, cuT( "/Face/Bottom" ), *lines[2], 1u )
			, copyCrossImageFace( engine, cuT( "/Face/Front" ), *lines[1], 1u )
			, copyCrossImageFace( engine, cuT( "/Face/Back" ), *lines[1], 3u ) };
	}
}

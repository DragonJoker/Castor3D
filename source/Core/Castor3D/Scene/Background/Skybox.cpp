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
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Background/Visitor.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Shader/ShaderModule.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementSmartPtr( c3d, SkyboxBackground )

#pragma GCC diagnostic ignored "-Wrestrict"

namespace c3d
{
	template<>
	class TextWriter< c3d::SkyboxBackground >
		: public TextWriterT< c3d::SkyboxBackground >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder )
			: TextWriterT< SkyboxBackground >{ tabs }
			, m_folder{ folder }
		{
		}

		bool operator()( SkyboxBackground const & background
			, StringStream & file )override
		{
			log::info << tabs() << cuT( "Writing SkyboxBackground" ) << std::endl;
			auto result = true;
			file << ( cuT( "\n" ) + tabs() + cuT( "//Skybox\n" ) );

			if ( auto block{ beginBlock( file, cuT( "skybox" ) ) } )
			{
				if ( background.isIrradianceShown() )
				{
					result = write( file, cuT( "show_irradiance" ), true );
				}
				else
				{
					result = writeOpt( file, cuT( "visible" ), background.isVisible(), true );
				}

				if ( !background.getEquiTexturePath().empty()
					&& File::fileExists( background.getEquiTexturePath() ) )
				{
						Path subfolder{ cuT( "Textures" ) };
						auto relative = String{ copyFile( background.getEquiTexturePath()
							, m_folder
							, subfolder ) };
						string::replace( relative, cuT( "\\" ), cuT( "/" ) );
						auto & size = background.getEquiSize();
						file << ( tabs() + cuT( "equirectangular" )
							+ cuT( " \"" ) + relative + cuT( "\" " )
							+ string::toString( size.getWidth() ) + cuT( "\n" ) );
						TextWriter< SkyboxBackground >::checkError( result, cuT( "Skybox equi-texture" ) );
				}
				else if ( !background.getCrossTexturePath().empty()
					&& File::fileExists( background.getCrossTexturePath() ) )
				{
					result = writeFile( file, cuT( "cross" ), background.getCrossTexturePath(), m_folder, cuT( "Textures" ) );
				}
				else if ( File::fileExists( background.getLayerTexturePath()[0u] )
					&& File::fileExists( background.getLayerTexturePath()[1u] )
					&& File::fileExists( background.getLayerTexturePath()[2u] )
					&& File::fileExists( background.getLayerTexturePath()[3u] )
					&& File::fileExists( background.getLayerTexturePath()[4u] )
					&& File::fileExists( background.getLayerTexturePath()[5u] ) )
				{
					result = true;

					Path subfolder{ cuT( "Textures" ) };

					for ( uint32_t i = 0; i < 6 && result; ++i )
					{
						result = writeFile( file
							, getName( SkyboxFace( i ) )
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

	//************************************************************************************************

	namespace skybox
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

		static CU_ImplementAttributeParserBlock( parserVisible, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->skybox )
			{
				CU_ParsingError( cuT( "No skybox initialised." ) );
			}
			else
			{
				blockContext->skybox->setVisible( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserIrradiance, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->skybox )
			{
				CU_ParsingError( cuT( "No skybox initialised." ) );
			}
			else
			{
				blockContext->skybox->showIrradiance( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEqui, SkyboxContext )
		{
			if ( params.size() <= 1 )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->skybox )
			{
				CU_ParsingError( cuT( "No skybox initialised." ) );
			}
			else
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setEquiTexture( filePath, path, params[1]->get< uint32_t >() );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCross, SkyboxContext )
		{
			if ( params.size() < 1 )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->skybox )
			{
				CU_ParsingError( cuT( "No skybox initialised." ) );
			}
			else
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setCrossTexture( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLeft, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setLeftImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRight, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setRightImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTop, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setTopImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBottom, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setBottomImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFront, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setFrontImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBack, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< Path >();
				auto filePath = context.file.getPath();

				if ( File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setBackImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, SkyboxContext )
		{
			if ( blockContext->skybox )
			{
				log::info << "Loaded skybox" << std::endl;
				blockContext->skybox->getScene().setBackground( ptrRefCast< SceneBackground >( blockContext->skybox ) );
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttributePop()
	}

	//************************************************************************************************

	SkyboxBackground::SkyboxBackground( Engine & engine
		, Scene & scene
		, String const & name )
		: SceneBackground{ engine, scene, name + cuT( "Skybox" ), cuT( "skybox" ), true }
	{
		m_texture = makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, skybox::doGetImageCreate( PixelFormat::eR8G8B8A8_UNORM, { 16u, 16u }, false )
			, MemoryPropertyFlags::eDeviceLocal
			, cuT( "SkyboxBackground_Colour" )
			, true /* isStatic */ );
	}

	void SkyboxBackground::accept( BackgroundVisitor & visitor )
	{
		visitor.visit( *this );
	}

	void SkyboxBackground::accept( ConfigurationVisitorBase & visitor )
	{
	}

	bool SkyboxBackground::write( String const & tabs
		, Path const & folder
		, StringStream & stream )const
	{
		return TextWriter< SkyboxBackground >{ tabs, folder }( *this, stream );
	}

	void SkyboxBackground::setLeftImage( Path const & folder
		, Path const & relative )
	{
		setFaceTexture( folder, relative, SkyboxFace::eLeft );
	}

	void SkyboxBackground::setRightImage( Path const & folder
		, Path const & relative )
	{
		setFaceTexture( folder, relative, SkyboxFace::eRight );
	}

	void SkyboxBackground::setTopImage( Path const & folder
		, Path const & relative )
	{
		setFaceTexture( folder, relative, SkyboxFace::eTop );
	}

	void SkyboxBackground::setBottomImage( Path const & folder
		, Path const & relative )
	{
		setFaceTexture( folder, relative, SkyboxFace::eBottom );
	}

	void SkyboxBackground::setFrontImage( Path const & folder
		, Path const & relative )
	{
		setFaceTexture( folder, relative, SkyboxFace::eFront );
	}

	void SkyboxBackground::setBackImage( Path const & folder
		, Path const & relative )
	{
		setFaceTexture( folder, relative, SkyboxFace::eBack );
	}

	void SkyboxBackground::setFaceTexture( Path const & folder
		, Path const & relative
		, SkyboxFace face )
	{
		m_layerTexturePath[size_t( face )] = folder / relative;
		m_layerTexture[size_t( face )] = SceneBackground::loadImage( *getScene().getEngine()
			, cuT( "SkyboxBackground" ) + String{ c3d::getName( face ) }
			, folder
			, relative
			, true );
		notifyChanged();
	}

	void SkyboxBackground::setEquiTexture( Path const & folder
		, Path const & relative
		, uint32_t size )
	{
		ImageCreateInfo image{ ImageCreateFlags::eNone
			, ImageType::e2D
			, PixelFormat::eUNDEFINED
			, { size, size, 1u }
			, 1u
			, 1u
			, SampleCount::e1
			, ImageTiling::eOptimal
			, ( ImageUsageFlags::eSampled | ImageUsageFlags::eTransferDst ) };
		m_equiTexture = makeUnique< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, c3d::move( image )
			, MemoryPropertyFlags::eDeviceLocal
			, cuT( "SkyboxBackgroundEquirectangular" ) );
		m_equiTexture->setSource( folder, relative );

		m_equiTexturePath = folder / relative;
		m_equiSize.set( size, size );
		notifyChanged();
	}

	void SkyboxBackground::setEquiSize( uint32_t size )
	{
		m_equiSize.set( size, size );
		notifyChanged();
	}

	void SkyboxBackground::setCrossTexture( Path const & folder
		, Path const & relative )
	{
		m_crossTexturePath = folder / relative;
		m_layerTexture = splitCrossImageBuffer( getFileImage( *getScene().getEngine()
			, cuT( "SkyboxBackgroundCross" )
			, folder
			, relative ) );
		notifyChanged();
	}

	void SkyboxBackground::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SkyboxContext > context{ result, CSCNSection::eSkybox, CSCNSection::eScene };

		context.addParser( cuT( "visible" ), skybox::parserVisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		context.addParser( cuT( "show_irradiance" ), skybox::parserIrradiance, { makeDefaultedParameter< ParameterType::eBool >( false ) } );
		context.addParser( cuT( "equirectangular" ), skybox::parserEqui, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eUInt32 >() } );
		context.addParser( cuT( "cross" ), skybox::parserCross, { makeParameter< ParameterType::ePath >() } );
		context.addParser( cuT( "left" ), skybox::parserLeft, { makeParameter< ParameterType::ePath >() } );
		context.addParser( cuT( "right" ), skybox::parserRight, { makeParameter< ParameterType::ePath >() } );
		context.addParser( cuT( "top" ), skybox::parserTop, { makeParameter< ParameterType::ePath >() } );
		context.addParser( cuT( "bottom" ), skybox::parserBottom, { makeParameter< ParameterType::ePath >() } );
		context.addParser( cuT( "front" ), skybox::parserFront, { makeParameter< ParameterType::ePath >() } );
		context.addParser( cuT( "back" ), skybox::parserBack, { makeParameter< ParameterType::ePath >() } );
		context.addPopParser( cuT( "}" ), skybox::parserEnd );
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
		viewport.setPerspective( updater.camera->getViewport().getFovY()
			, updater.camera->getRatio()
			, updater.camera->getNear()
			, updater.camera->getFar() );
		viewport.update();
		updater.bgMtxView = updater.camera->getView();
		updater.bgMtxProj = updater.isSafeBanded
			? viewport.getSafeBandedProjection( updater.renderSize )
			: viewport.getProjection();
	}

	void SkyboxBackground::doGpuUpdate( GpuUpdater & updater )const
	{
	}

	void SkyboxBackground::doUpload( UploadData & uploader )
	{
		ImageSubresourceRange dstSubresource{ ImageAspectFlags::eColor, 0u, 1u, 0u, 1u };

		for ( auto const & layer : m_layerTexture )
		{
			dstSubresource.levelCount = layer->getLevels();
			uploader.pushUpload( layer->getPxBuffer().getConstPtr()
				, layer->getPxBuffer().getSize()
				, m_texture->getTexture()
				, layer->getLayout()
				, dstSubresource
				, ImageLayout::eShaderReadOnly
				, PipelineStageFlags::eFragmentShader );
			dstSubresource.baseArrayLayer++;
		}
	}

	void SkyboxBackground::doAddPassBindings( crg::FramePass & pass
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

	void SkyboxBackground::doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, shaderStages ) );	// c3d_mapBackground
		++index;
	}

	void SkyboxBackground::doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
		, Texture * targetImage
		, uint32_t & index )const
	{
		bindTexture( m_textureId.getSampledView()
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

	void SkyboxBackground::doInitialiseLayerTexture( RenderDevice const & device )
	{
		uint32_t maxDim{};

		for ( auto const & layer : m_layerTexture )
		{
			auto dim = layer->getDimensions();
			maxDim = std::max( maxDim
				, std::max( dim->x, dim->y ) );
		}

		// create the cube texture if needed.
		m_textureId = { device
			, getScene().getResources()
			, cuT( "SkyboxBackgroundLayerCube" )
			, { ImageCreateFlags::eCubeCompatible
				, { maxDim, maxDim, 1u }, 6u, ashes::getMaxMipCount( { maxDim, maxDim, maxDim } )
				, m_layerTexture[0]->getPxBuffer().getFormat()
				, ( ImageUsageFlags::eSampled
					| ImageUsageFlags::eTransferDst ) }
			, {} };
		m_textureId.create();
		m_texture = makeUnique< TextureLayout >( device.renderSystem
			, cuT( "SkyboxBackgroundLayerCube" )
			, *m_textureId.image
			, m_textureId.getWholeViewId() );
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
				, device, image.getName() + cuT( "/Upload" ), *queueData.commandPool };
			upload->pushUpload( image.getPxBuffer().getConstPtr()
				, image.getPxBuffer().getSize()
				, texture
				, image.getLayout()
				, { ImageAspectFlags::eColor, 0u, 1u, 0u, image.getLayout().depthLayers() }
				, ImageLayout::eShaderReadOnly
				, PipelineStageFlags::eFragmentShader );
		}

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != m_equiSize.getWidth()
			|| m_texture->getDimensions().height != m_equiSize.getHeight() )
		{
			m_textureId = { device
				, getScene().getResources()
				, cuT( "SkyboxBackgroundEquiCube" )
				, { ImageCreateFlags::eCubeCompatible
					, makeExtent3D( m_equiSize ), 6u, ashes::getMaxMipCount( makeVkExtent3D( m_equiSize ) )
					, m_equiTexture->getPixelFormat()
					, ( ImageUsageFlags::eSampled
						| ImageUsageFlags::eTransferDst
						| ImageUsageFlags::eColorAttachment ) }
				, {} };
			m_textureId.create();
			m_texture = makeUnique< TextureLayout >( device.renderSystem
				, cuT( "SkyboxBackgroundEquiCube" )
				, *m_textureId.image
				, m_textureId.getWholeViewId() );
		}

		transformEquirectangularToCube( *m_equiTexture, *m_texture, device, queueData );
		m_texture->generateMipmaps( queueData
			, ImageLayout::eShaderReadOnly );
		m_equiTexture->cleanup();
	}

	ImageUPtr SkyboxBackground::copyCrossImageFace( StringView faceName
		, Image const & lines
		, uint32_t index )
	{
		auto name = lines.getName() + String{ faceName };
		auto height = lines.getHeight();
		auto width = height;
		auto blockExtent = ashes::getMinimalExtent2D( convert( lines.getPixelFormat() ) );
		auto blockSize = ashes::getMinimalSize( convert( lines.getPixelFormat() ) );
		auto lineSize = lines.getWidth() * blockSize;
		auto sectionSize = lineSize / 4u;
		auto lineOffset = ptrdiff_t( index * sectionSize );
		auto buffer = PxBufferBase::create( { width, height }, lines.getPixelFormat() );
		auto srcData = lines.getBuffer().data() + lineOffset;
		auto dstData = buffer->getPtr();
		auto blockCount = height / blockExtent.height;

		for ( uint32_t i = 0u; i < blockCount; ++i )
		{
			std::memcpy( dstData, srcData, sectionSize );
			srcData += lineSize;
			dstData += sectionSize;
		}

		buffer = adaptBuffer( *buffer
			, name
			, true );
		ImageMemoryLayout layout{ lines.getLayout().type, *buffer };
		log::info << "Loaded skybox image [" << name << "] (" << layout << ")" << std::endl;
		return makeUnique< Image >( lines.getName()
			, lines.getPath()
			, layout
			, c3d::move( buffer ) );
	}

	Array< ImageUPtr, 6u > SkyboxBackground::splitCrossImageBuffer( Image const & cross )
	{
		auto height = cross.getHeight() / 3u;
		CU_Require( cross.getWidth() / 4u == height );
		uint8_t const * buffer = cross.getBuffer().data();

		// First, split vertically, since it's the most straightforward.
		auto linesStride = height * cross.getPxBuffer().getSize() / cross.getHeight();
		Array< ImageUPtr, 3u > lines{
			makeUnique< Image >( cross.getName(), cross.getPath(), Size{ cross.getWidth(), height }, cross.getPixelFormat(), buffer + ptrdiff_t( linesStride * 0 ), cross.getPixelFormat() ),
			makeUnique< Image >( cross.getName(), cross.getPath(), Size{ cross.getWidth(), height }, cross.getPixelFormat(), buffer + ptrdiff_t( linesStride * 1 ), cross.getPixelFormat() ),
			makeUnique< Image >( cross.getName(), cross.getPath(), Size{ cross.getWidth(), height }, cross.getPixelFormat(), buffer + ptrdiff_t( linesStride * 2 ), cross.getPixelFormat() ) };

		// Then split horizontally
		return { copyCrossImageFace( cuT( "/Face/Left" ), *lines[1], 2u )
			, copyCrossImageFace( cuT( "/Face/Right" ), *lines[1], 0u )
			, copyCrossImageFace( cuT( "/Face/Top" ), *lines[0], 1u )
			, copyCrossImageFace( cuT( "/Face/Bottom" ), *lines[2], 1u )
			, copyCrossImageFace( cuT( "/Face/Front" ), *lines[1], 1u )
			, copyCrossImageFace( cuT( "/Face/Back" ), *lines[1], 3u ) };
	}
}

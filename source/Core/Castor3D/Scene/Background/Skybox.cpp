#include "Skybox.hpp"

#include "Engine.hpp"

#include "Render/RenderPipeline.hpp"
#include "RenderToTexture/EquirectangularToCube.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Scene/Background/Visitor.hpp"
#include "Shader/Program.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Ashes/Image/TextureView.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/Shader/ShaderModule.hpp>
#include <Ashes/Sync/Fence.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

#include "Shader/Shaders/GlslUtils.hpp"

using namespace castor;
using namespace sdw;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		ashes::ImageCreateInfo doGetImageCreate( ashes::Format format
			, Size const & dimensions
			, bool attachment
			, uint32_t mipLevel = 1u )
		{
			ashes::ImageCreateInfo result;
			result.flags = ashes::ImageCreateFlag::eCubeCompatible;
			result.arrayLayers = 6u;
			result.extent.width = dimensions.getWidth();
			result.extent.height = dimensions.getHeight();
			result.extent.depth = 1u;
			result.format = format;
			result.initialLayout = ashes::ImageLayout::eUndefined;
			result.imageType = ashes::TextureType::e2D;
			result.mipLevels = mipLevel;
			result.samples = ashes::SampleCountFlag::e1;
			result.sharingMode = ashes::SharingMode::eExclusive;
			result.tiling = ashes::ImageTiling::eOptimal;
			result.usage = ashes::ImageUsageFlag::eSampled
				| ashes::ImageUsageFlag::eTransferDst
				| ( attachment ? ashes::ImageUsageFlag::eColourAttachment : ashes::ImageUsageFlag( 0u ) );
			return result;
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

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}
		else if ( !obj.getCrossTexturePath().empty()
			&& castor::File::fileExists( obj.getCrossTexturePath() ) )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			Path subfolder{ cuT( "Textures" ) };
			String relative = Scene::TextWriter::copyFile( obj.getCrossTexturePath()
				, file.getFilePath()
				, subfolder );
			string::replace( relative, cuT( "\\" ), cuT( "/" ) );
			result = file.writeText( m_tabs + cuT( "\tcross" ) + cuT( " \"" ) + relative + cuT( "\"\n" ) ) > 0;
			castor::TextWriter< SkyboxBackground >::checkError( result, "Skybox cross-texture" );

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}
		else if ( castor::File::fileExists( Path{ obj.m_texture->getImage( 0u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 1u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 2u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 3u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 4u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 5u ).toString() } ) )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			Path subfolder{ cuT( "Textures" ) };

			for ( uint32_t i = 0; i < 6 && result; ++i )
			{
				String relative = Scene::TextWriter::copyFile( Path{ obj.m_texture->getImage( size_t( CubeMapFace( i ) ) ).toString() }
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
			, doGetImageCreate( ashes::Format::eR8G8B8A8_UNORM, { 16u, 16u }, false )
			, ashes::MemoryPropertyFlag::eDeviceLocal );
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
		getTexture().getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource( folder, relative, ImageComponents::eRGB );
		notifyChanged();
	}

	void SkyboxBackground::loadRightImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource( folder, relative, ImageComponents::eRGB );
		notifyChanged();
	}

	void SkyboxBackground::loadTopImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource( folder, relative, ImageComponents::eRGB );
		notifyChanged();
	}

	void SkyboxBackground::loadBottomImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource( folder, relative, ImageComponents::eRGB );
		notifyChanged();
	}

	void SkyboxBackground::loadFrontImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource( folder, relative, ImageComponents::eRGB );
		notifyChanged();
	}

	void SkyboxBackground::loadBackImage( castor::Path const & folder
		, castor::Path const & relative )
	{
		getTexture().getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource( folder, relative, ImageComponents::eRGB );
		notifyChanged();
	}

	void SkyboxBackground::loadEquiTexture( castor::Path const & folder
		, castor::Path const & relative
		, uint32_t size )
	{
		ashes::ImageCreateInfo image{};
		image.arrayLayers = 1u;
		image.extent.width = size;
		image.extent.height = size;
		image.extent.depth = 1u;
		image.imageType = ashes::TextureType::e2D;
		image.initialLayout = ashes::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = ashes::SampleCountFlag::e1;
		image.sharingMode = ashes::SharingMode::eExclusive;
		image.tiling = ashes::ImageTiling::eOptimal;
		image.usage = ashes::ImageUsageFlag::eSampled
			| ashes::ImageUsageFlag::eTransferDst;
		auto texture = std::make_shared< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, image
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		texture->getDefaultImage().initialiseSource( folder, relative, ImageComponents::eRGB );
		setEquiTexture( texture, size );
	}

	void SkyboxBackground::setEquiTexture( TextureLayoutSPtr texture
		, uint32_t size )
	{
		m_equiTexturePath = castor::Path( texture->getDefaultImage().toString() );
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
		ashes::ImageCreateInfo image{};
		image.arrayLayers = 1u;
		image.extent.depth = 1u;
		image.imageType = ashes::TextureType::e2D;
		image.initialLayout = ashes::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = ashes::SampleCountFlag::e1;
		image.sharingMode = ashes::SharingMode::eExclusive;
		image.tiling = ashes::ImageTiling::eOptimal;
		image.usage = ashes::ImageUsageFlag::eSampled
			| ashes::ImageUsageFlag::eTransferSrc
			| ashes::ImageUsageFlag::eTransferDst;
		auto texture = std::make_shared< TextureLayout >( *getScene().getEngine()->getRenderSystem()
			, image
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		texture->getDefaultImage().initialiseSource( folder, relative, ImageComponents::eRGB );
		setCrossTexture( texture );
	}

	void SkyboxBackground::setCrossTexture( TextureLayoutSPtr texture )
	{
		m_crossTexturePath = castor::Path( texture->getDefaultImage().toString() );
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
		auto node = camera.getParent();
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

		m_hdr = m_texture->getPixelFormat() == ashes::Format::eR32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR32G32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR32G32B32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR32G32B32A32_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16G16_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16G16B16_SFLOAT
			|| m_texture->getPixelFormat() == ashes::Format::eR16G16B16A16_SFLOAT;
		return m_texture->initialise();
	}

	void SkyboxBackground::doInitialiseEquiTexture()
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_equiTexture->initialise();

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != m_equiSize.getWidth()
			|| m_texture->getDimensions().height != m_equiSize.getHeight() )
		{
			m_texture = std::make_shared< TextureLayout >( renderSystem
				, doGetImageCreate( m_equiTexture->getPixelFormat(), m_equiSize, true )
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			m_texture->getImage( CubeMapFace::ePositiveX ).initialiseSource();
			m_texture->getImage( CubeMapFace::eNegativeX ).initialiseSource();
			m_texture->getImage( CubeMapFace::ePositiveY ).initialiseSource();
			m_texture->getImage( CubeMapFace::eNegativeY ).initialiseSource();
			m_texture->getImage( CubeMapFace::ePositiveZ ).initialiseSource();
			m_texture->getImage( CubeMapFace::eNegativeZ ).initialiseSource();
			m_texture->initialise();

			EquirectangularToCube equiToCube{ *m_equiTexture
				, renderSystem
				, *m_texture };
			equiToCube.render();

			if ( m_scene.getMaterialsType() != MaterialType::eLegacy )
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
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_texture->getImage( CubeMapFace::ePositiveX ).initialiseSource();
		m_texture->getImage( CubeMapFace::eNegativeX ).initialiseSource();
		m_texture->getImage( CubeMapFace::ePositiveY ).initialiseSource();
		m_texture->getImage( CubeMapFace::eNegativeY ).initialiseSource();
		m_texture->getImage( CubeMapFace::ePositiveZ ).initialiseSource();
		m_texture->getImage( CubeMapFace::eNegativeZ ).initialiseSource();
		m_texture->initialise();

		ashes::ImageSubresourceLayers srcSubresource
		{
			m_crossTexture->getDefaultView().getSubResourceRange().aspectMask,
			0,
			0,
			1,
		};
		ashes::ImageSubresourceLayers dstSubresource
		{
			m_texture->getDefaultView().getSubResourceRange().aspectMask,
			0,
			0,
			1,
		};
		ashes::ImageCopy copyInfos[6];
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

		auto & device = getCurrentDevice( renderSystem );
		auto commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		commandBuffer->begin();
		commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTopOfPipe
			, ashes::PipelineStageFlag::eTransfer
			, m_crossTexture->getDefaultView().makeTransferSource( ashes::ImageLayout::eUndefined, 0u ) );
		uint32_t index{ 0u };

		for ( auto & copyInfo : copyInfos )
		{
			commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTopOfPipe
				, ashes::PipelineStageFlag::eTransfer
				, m_texture->getImage( index ).getView().makeTransferDestination( ashes::ImageLayout::eUndefined, 0u ) );
			commandBuffer->copyImage( copyInfo
				, m_crossTexture->getTexture()
				, ashes::ImageLayout::eTransferSrcOptimal
				, m_texture->getTexture()
				, ashes::ImageLayout::eTransferDstOptimal );
			commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTransfer
				, ashes::PipelineStageFlag::eFragmentShader
				, m_texture->getImage( index ).getView().makeShaderInputResource( ashes::ImageLayout::eTransferDstOptimal
					, ashes::AccessFlag::eTransferWrite ) );
			++index;
		}

		commandBuffer->end();

		device.getGraphicsQueue().submit( *commandBuffer, nullptr );
		device.getGraphicsQueue().waitIdle();

		m_crossTexture->cleanup();
	}
}

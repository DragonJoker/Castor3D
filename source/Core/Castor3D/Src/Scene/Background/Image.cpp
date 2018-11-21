#include "Scene/Background/Image.hpp"

#include "Engine.hpp"

#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "RenderToTexture/TextureProjectionToCube.hpp"
#include "Shader/Program.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Miscellaneous/PushConstantRange.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Shader/ShaderModule.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

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

	ImageBackground::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< ImageBackground >{ tabs }
	{
	}

	bool ImageBackground::TextWriter::operator()( ImageBackground const & obj, TextFile & file )
	{
		castor::Path relative = Scene::TextWriter::copyFile( castor::Path{ obj.getTexture().getDefaultImage().toString() }
			, file.getFilePath()
			, castor::Path{ cuT( "Textures" ) } );
		auto result = file.writeText( m_tabs + cuT( "background_image \"" ) + relative + cuT( "\"\n" ) ) > 0;
		castor::TextWriter< Scene >::checkError( result, "Background image" );
		return result;
	}

	//************************************************************************************************

	ImageBackground::ImageBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eImage }
		, m_viewport{ engine }
	{
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate( ashes::Format::eR8G8B8A8_UNORM, { 16u, 16u }, false )
			, ashes::MemoryPropertyFlag::eDeviceLocal );
	}

	ImageBackground::~ImageBackground()
	{
	}

	bool ImageBackground::loadImage( Path const & folder, Path const & relative )
	{
		bool result = false;

		try
		{
			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.flags = 0u;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = ashes::ImageUsageFlag::eSampled
				| ashes::ImageUsageFlag::eTransferSrc
				| ashes::ImageUsageFlag::eTransferDst;
			auto texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			texture->setSource( folder, relative );
			m_2dTexture = texture;
			m_2dTexturePath = castor::Path( m_2dTexture->getDefaultImage().toString() );
			notifyChanged();
			result = true;
		}
		catch ( castor::Exception & p_exc )
		{
			Logger::logError( p_exc.what() );
		}

		return result;
	}

	void ImageBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	ashes::ShaderStageStateArray ImageBackground::doInitialiseShader()
	{
		using namespace glsl;
		auto & renderSystem = *getEngine()->getRenderSystem();

		glsl::Shader vtx;
		{
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, 0, 0 );
			UBO_MODEL_MATRIX( writer, 1, 0 );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			std::function< void() > main = [&]()
			{
				out.gl_Position() = writer.paren( c3d_projection * c3d_curView * c3d_curMtxModel * vec4( position, 1.0 ) ).xyww();
				vtx_texture = position;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( writer, 2, 0 );
			auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto c3d_mapSkybox = writer.declSampler< SamplerCube >( cuT( "c3d_mapSkybox" ), 3u, 0u );
			glsl::Utils utils{ writer };

			if ( !m_hdr )
			{
				utils.declareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

			std::function< void() > main = [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" )
					, texture( c3d_mapSkybox, vtx_texture ) );

				if ( !m_hdr )
				{
					pxl_FragColor = vec4( utils.removeGamma( c3d_gamma, colour.xyz() ), colour.w() );
				}
				else
				{
					pxl_FragColor = vec4( colour.xyz(), colour.w() );
				}
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			pxl = writer.finalise();
		}

		auto & device = getCurrentDevice( renderSystem );
		ashes::ShaderStageStateArray result;
		result.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		result.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		result[0].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, vtx.getSource() ) );
		result[1].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, pxl.getSource() ) );
		return result;
	}

	bool ImageBackground::doInitialise( ashes::RenderPass const & renderPass )
	{
		doInitialise2DTexture();
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

	void ImageBackground::doCleanup()
	{
	}

	void ImageBackground::doUpdate( Camera const & camera )
	{
		m_viewport.setOrtho( -1.0_r
			, 1.0_r
			, -m_ratio
			, m_ratio
			, 0.1_r
			, 2.0_r );
		m_viewport.update();
		auto node = camera.getParent();
		Matrix4x4f view;
		matrix::lookAt( view
			, node->getDerivedPosition()
			, node->getDerivedPosition() + Point3f{ 0.0f, 0.0f, 1.0f }
			, Point3f{ 0.0f, 1.0f, 0.0f } );
		m_matrixUbo.update( view
			, m_viewport.getProjection() );
	}

	void ImageBackground::doInitialise2DTexture()
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_2dTexture->initialise();
		ashes::Extent3D extent{ m_2dTexture->getWidth(), m_2dTexture->getHeight(), 1u };
		auto dim = std::max( m_2dTexture->getWidth(), m_2dTexture->getHeight() );

		// create the cube texture if needed.
		if ( m_texture->getDimensions().width != dim
			|| m_texture->getDimensions().height != dim )
		{
			m_ratio = float( m_2dTexture->getHeight() ) / m_2dTexture->getWidth();
			auto xOffset = ( dim - extent.width ) / 2u;
			auto yOffset = ( dim - extent.height ) / 2u;
			ashes::Offset3D const srcOffset{ 0, 0, 0 };
			ashes::Offset3D const dstOffset{ int32_t( xOffset ), int32_t( yOffset ), 0 };
			m_texture = std::make_shared< TextureLayout >( renderSystem
				, doGetImageCreate( m_2dTexture->getPixelFormat(), { dim, dim }, true )
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
				m_2dTexture->getDefaultView().getSubResourceRange().aspectMask,
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

			auto & device = getCurrentDevice( renderSystem );
			auto commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
			commandBuffer->begin();
			commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTopOfPipe
				, ashes::PipelineStageFlag::eTransfer
				, m_2dTexture->getDefaultView().makeTransferSource( ashes::ImageLayout::eUndefined, 0u ) );
			uint32_t index{ 0u };

			for ( auto & copyInfo : copyInfos )
			{
				commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eTopOfPipe
					, ashes::PipelineStageFlag::eTransfer
					, m_texture->getImage( index ).getView().makeTransferDestination( ashes::ImageLayout::eUndefined, 0u ) );
				commandBuffer->copyImage( copyInfo
					, m_2dTexture->getTexture()
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

			m_2dTexture->cleanup();
		}
	}

	//************************************************************************************************
}

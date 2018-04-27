#include "Scene/Background/Colour.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		static uint32_t constexpr Dim = 16u;

		renderer::ImageCreateInfo doGetImageCreate()
		{
			Size size{ Dim, Dim };
			renderer::ImageCreateInfo result;
			result.flags = renderer::ImageCreateFlag::eCubeCompatible;
			result.arrayLayers = 6u;
			result.extent.width = Dim;
			result.extent.height = Dim;
			result.extent.depth = 1u;
			result.format = renderer::Format::eR32G32B32A32_SFLOAT;
			result.imageType = renderer::TextureType::e2D;
			result.initialLayout = renderer::ImageLayout::eUndefined;
			result.mipLevels = 1u;
			result.samples = renderer::SampleCountFlag::e1;
			result.sharingMode = renderer::SharingMode::eExclusive;
			result.tiling = renderer::ImageTiling::eOptimal;
			result.usage = renderer::ImageUsageFlag::eSampled | renderer::ImageUsageFlag::eTransferDst;
			return result;
		}

		renderer::BufferImageCopyArray doInitialiseCopies()
		{
			renderer::BufferImageCopyArray result( 6u );
			renderer::BufferImageCopy copyRegion{};
			copyRegion.imageExtent.width = 16u;
			copyRegion.imageExtent.height = 16u;
			copyRegion.imageExtent.depth = 1u;
			copyRegion.imageSubresource.aspectMask = renderer::ImageAspectFlag::eColour;
			copyRegion.imageSubresource.layerCount = 1u;
			copyRegion.imageSubresource.mipLevel = 0u;

			for ( uint32_t i = 0; i < 6u; ++i )
			{
				copyRegion.imageSubresource.baseArrayLayer = i;
				result[i] = copyRegion;
			}

			return result;
		}
	}

	//************************************************************************************************

	ColourBackground::ColourBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eColour }
		, m_copyRegions{ doInitialiseCopies() }
		, m_viewport{ engine }
	{
		m_hdr = true;
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate()
			, renderer::MemoryPropertyFlag::eDeviceLocal );
	}

	ColourBackground::~ColourBackground()
	{
	}

	void ColourBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	bool ColourBackground::doInitialise( renderer::RenderPass const & renderPass )
	{
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( pow( value.red(), m_scene.getHdrConfig().getGamma() )
			, pow( value.green(), m_scene.getHdrConfig().getGamma() )
			, pow( value.blue(), m_scene.getHdrConfig().getGamma() ) );
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		m_stagingBuffer = renderer::makeBuffer< Point4f >( device
			, 256u
			, renderer::BufferTarget::eTransferSrc
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent );

		m_texture->getImage( 0u ).initialiseSource();
		m_texture->getImage( 1u ).initialiseSource();
		m_texture->getImage( 2u ).initialiseSource();
		m_texture->getImage( 3u ).initialiseSource();
		m_texture->getImage( 4u ).initialiseSource();
		m_texture->getImage( 5u ).initialiseSource();
		auto result = m_texture->initialise();
		m_colour.reset();

		if ( result )
		{
			m_cmdCopy = device.getGraphicsCommandPool().createCommandBuffer( true );
			result = m_cmdCopy->begin();

			if ( result )
			{
				m_cmdCopy->memoryBarrier( renderer::PipelineStageFlag::eTopOfPipe
					, renderer::PipelineStageFlag::eTransfer
					, m_texture->getDefaultView().makeTransferDestination( renderer::ImageLayout::eUndefined, 0u ) );
				m_cmdCopy->copyToImage( m_copyRegions
					, m_stagingBuffer->getBuffer()
					, m_texture->getTexture() );
				m_cmdCopy->memoryBarrier( renderer::PipelineStageFlag::eTransfer
					, renderer::PipelineStageFlag::eFragmentShader
					, m_texture->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eTransferDstOptimal, renderer::AccessFlag::eTransferWrite ) );
				result = m_cmdCopy->end();
			}
		}

		doUpdateColour();
		return result;
	}

	void ColourBackground::doCleanup()
	{
		m_stagingBuffer.reset();
		m_cmdCopy.reset();
	}

	void ColourBackground::doUpdate( Camera const & camera )
	{
		auto & value = m_scene.getBackgroundColour();
		m_colour = HdrRgbColour::fromComponents( pow( value.red(), m_scene.getHdrConfig().getGamma() )
			, pow( value.green(), m_scene.getHdrConfig().getGamma() )
			, pow( value.blue(), m_scene.getHdrConfig().getGamma() ) );

		if ( m_colour.isDirty() )
		{
			doUpdateColour();
		}

		auto node = camera.getParent();
		m_viewport.setPerspective( 45.0_degrees
			, camera.getViewport().getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.update( camera.getView()
			, m_viewport.getProjection() );
	}

	void ColourBackground::doUpdateColour()
	{
		if ( auto * buffer = m_stagingBuffer->lock( 0u, 256u, renderer::MemoryMapFlag::eWrite ) )
		{
			Point4f colour{ m_colour->red().value(), m_colour->green().value(), m_colour->blue().value(), 1.0f };

			for ( auto i = 0u; i < 256; ++i )
			{
				*buffer = colour;
				++buffer;
			}

			m_stagingBuffer->flush( 0u, 256u );
			m_stagingBuffer->unlock();
			auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
			device.getGraphicsQueue().submit( *m_cmdCopy, nullptr );
			device.getGraphicsQueue().waitIdle();
			m_colour.reset();
		}
	}

	//************************************************************************************************
}

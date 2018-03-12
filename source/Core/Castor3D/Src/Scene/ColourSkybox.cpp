#include "ColourSkybox.hpp"

#include "Engine.hpp"

#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace
	{
		renderer::BufferImageCopyArray doInitialiseCopies()
		{
			renderer::BufferImageCopyArray result( 6u );
			renderer::BufferImageCopy copyRegion{};
			copyRegion.imageExtent.width = 16u;
			copyRegion.imageExtent.height = 16u;
			copyRegion.imageExtent.depth = 16u;
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

	ColourSkybox::ColourSkybox( Engine & engine )
		: Skybox{ engine }
		, m_copyRegions{ doInitialiseCopies() }
	{
		m_hdr = true;
	}

	ColourSkybox::~ColourSkybox()
	{
	}

	bool ColourSkybox::initialise()
	{
		REQUIRE( m_scene );
		static uint32_t constexpr Dim = 16u;
		Size size{ Dim, Dim };
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		m_stagingBuffer = renderer::makeBuffer< Point3f >( device
			, 256u
			, renderer::BufferTarget::eTransferSrc
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent );
		doUpdateColour();

		renderer::ImageCreateInfo image{};
		image.flags = renderer::ImageCreateFlag::eCubeCompatible;
		image.arrayLayers = 6u;
		image.extent.width = Dim;
		image.extent.height = Dim;
		image.extent.depth = 1u;
		image.format = renderer::Format::eR8G8B8_UNORM;
		image.imageType = renderer::TextureType::e3D;
		image.initialLayout = renderer::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = renderer::SampleCountFlag::e1;
		image.sharingMode = renderer::SharingMode::eExclusive;
		image.tiling = renderer::ImageTiling::eOptimal;
		image.usage = renderer::ImageUsageFlag::eSampled | renderer::ImageUsageFlag::eTransferDst;
		m_texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
			, image
			, renderer::MemoryPropertyFlag::eHostVisible );

		m_texture->getImage( 0u ).initialiseSource();
		m_texture->getImage( 1u ).initialiseSource();
		m_texture->getImage( 2u ).initialiseSource();
		m_texture->getImage( 3u ).initialiseSource();
		m_texture->getImage( 4u ).initialiseSource();
		m_texture->getImage( 5u ).initialiseSource();
		m_colour.reset();

		m_cmdCopy = device.getGraphicsCommandPool().createCommandBuffer( true );
		m_cmdCopy->begin();
		m_cmdCopy->copyToImage( m_copyRegions
			, m_stagingBuffer->getBuffer()
			, m_texture->getTexture() );

		return Skybox::initialise();
	}

	void ColourSkybox::update()
	{
		if ( m_colour.isDirty() && m_ibl )
		{
			doUpdateColour();
			auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
			device.getGraphicsQueue().submit( *m_cmdCopy, nullptr );
			device.getGraphicsQueue().waitIdle();
			m_colour.reset();
		}
	}

	void ColourSkybox::setColour( castor::RgbColour const & value )
	{
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
	}

	void ColourSkybox::doUpdateColour()
	{
		if ( auto * buffer = m_stagingBuffer->lock( 0u, 256u, renderer::MemoryMapFlag::eWrite ) )
		{
			Point3f colour{ m_colour->red().value(), m_colour->green().value(), m_colour->blue().value() };

			for ( auto i = 0u; i < 256; ++i )
			{
				*buffer = colour;
				++buffer;
			}

			m_stagingBuffer->flush( 0u, 256u );
			m_stagingBuffer->unlock();
		}
	}
}

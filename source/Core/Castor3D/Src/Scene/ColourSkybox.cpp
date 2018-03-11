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
	ColourSkybox::ColourSkybox( Engine & engine )
		: Skybox{ engine }
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
		constexpr PixelFormat format{ PixelFormat::eRGB32F };
		Pixel< format > pixel{ true };
		auto pxData = pixel.ptr();
		std::memcpy( pxData, &m_colour->red().value(), sizeof( float ) );
		pxData += sizeof( float );
		std::memcpy( pxData, &m_colour->green().value(), sizeof( float ) );
		pxData += sizeof( float );
		std::memcpy( pxData, &m_colour->blue().value(), sizeof( float ) );

		auto buffer = PxBufferBase::create( size, format );
		auto data = buffer->ptr();

		for ( uint32_t i = 0u; i < 256; ++i )
		{
			std::memcpy( data, pixel.constPtr(), 3 * sizeof( float ) );
			data += 3 * sizeof( float );
		}

		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
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
		m_texture = device.createTexture( image, renderer::MemoryPropertyFlag::eDeviceLocal );

		m_texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
			, image
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_texture->getImage( 0u ).initialiseSource( buffer );
		m_texture->getImage( 1u ).initialiseSource( buffer );
		m_texture->getImage( 2u ).initialiseSource( buffer );
		m_texture->getImage( 3u ).initialiseSource( buffer );
		m_texture->getImage( 4u ).initialiseSource( buffer );
		m_texture->getImage( 5u ).initialiseSource( buffer );
		m_colour.reset();

		return Skybox::initialise();
	}

	void ColourSkybox::update()
	{
		if ( m_colour.isDirty()
			&& m_ibl )
		{
			constexpr PixelFormat format{ PixelFormat::eRGB32F };
			Pixel< format > pixel{ true };
			auto pxData = pixel.ptr();
			std::memcpy( pxData, &m_colour->red().value(), sizeof( float ) );
			pxData += sizeof( float );
			std::memcpy( pxData, &m_colour->green().value(), sizeof( float ) );
			pxData += sizeof( float );
			std::memcpy( pxData, &m_colour->blue().value(), sizeof( float ) );
			m_texture->bind( MinTextureIndex );
			doUpdateTextureImage( 0u, pixel );
			doUpdateTextureImage( 1u, pixel );
			doUpdateTextureImage( 2u, pixel );
			doUpdateTextureImage( 3u, pixel );
			doUpdateTextureImage( 4u, pixel );
			doUpdateTextureImage( 5u, pixel );
			m_texture->unbind( MinTextureIndex );
			m_ibl->update( *m_texture );
			m_colour.reset();
		}
	}

	void ColourSkybox::setColour( castor::RgbColour const & value )
	{
		m_colour = HdrRgbColour::fromComponents( value.red(), value.green(), value.blue() );
	}

	void ColourSkybox::doUpdateTextureImage( uint32_t index
		, Pixel< PixelFormat::eRGB32F > const & pixel )
	{
		auto buffer = m_texture->lock( AccessType::eWrite, index );

		if ( buffer )
		{
			for ( uint32_t i = 0u; i < 256; ++i )
			{
				std::memcpy( buffer, pixel.constPtr(), 3 * sizeof( float ) );
				buffer += 3 * sizeof( float );
			}

			m_texture->unlock( true, index );
		}
	}
}

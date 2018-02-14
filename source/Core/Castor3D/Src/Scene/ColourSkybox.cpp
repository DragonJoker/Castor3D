#include "ColourSkybox.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBufferAttachment.hpp"
#include "Buffer/BufferElementGroup.hpp"
#include "Buffer/GeometryBuffers.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

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
		Size size{ 16, 16 };
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

		m_texture = getEngine()->getRenderSystem()->createTexture( TextureType::eCube
			, AccessType::eWrite
			, AccessType::eRead );
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

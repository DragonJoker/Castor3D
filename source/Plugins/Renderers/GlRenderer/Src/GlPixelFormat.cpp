#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlInternal getInternal( renderer::PixelFormat const & format )noexcept
	{
		switch ( format )
		{
		case renderer::PixelFormat::eL8:
			return GL_INTERNAL_R8;

		case renderer::PixelFormat::eA8L8:
			return GL_INTERNAL_R8G8;

		case renderer::PixelFormat::eR8G8B8:
			return GL_INTERNAL_R8G8B8;

		case renderer::PixelFormat::eR5G6B5:
			return GL_INTERNAL_R5G6B5;

		case renderer::PixelFormat::eA8R8G8B8:
			return GL_INTERNAL_R8G8B8A8;

		case renderer::PixelFormat::eA8B8G8R8:
			return GL_INTERNAL_B8G8R8A8;

		case renderer::PixelFormat::eA1R5G5B5:
			return GL_INTERNAL_R5G5B5A1;

		case renderer::PixelFormat::eA4R4G4B4:
			return GL_INTERNAL_R4G4B4A4;

		case renderer::PixelFormat::eD16:
			return GL_INTERNAL_D16;

		case renderer::PixelFormat::eD24S8:
			return GL_INTERNAL_D24S8;

		case renderer::PixelFormat::eD32F:
			return GL_INTERNAL_D32F;

		case renderer::PixelFormat::eS8:
			return GL_INTERNAL_S8;

		default:
			assert( false && "Unsupported pixel format." );
			return GL_INTERNAL_R8G8B8A8;
		}
	}

	GlFormat getFormat( renderer::PixelFormat format )noexcept
	{
		switch ( format )
		{
		case renderer::PixelFormat::eL8:
			return GL_FORMAT_R;

		case renderer::PixelFormat::eA8L8:
			return GL_FORMAT_RG;

		case renderer::PixelFormat::eR8G8B8:
			return GL_FORMAT_RGB;

		case renderer::PixelFormat::eR5G6B5:
			return GL_FORMAT_RGB;

		case renderer::PixelFormat::eA8R8G8B8:
#if defined( _WIN32 )
			return GL_FORMAT_ABGR;
#else
			return GL_FORMAT_RGBA;
#endif

		case renderer::PixelFormat::eA8B8G8R8:
			return GL_FORMAT_ABGR;

		case renderer::PixelFormat::eA1R5G5B5:
			return GL_FORMAT_RGBA;

		case renderer::PixelFormat::eA4R4G4B4:
			return GL_FORMAT_RGBA;

		case renderer::PixelFormat::eD16:
		case renderer::PixelFormat::eD32F:
			return GL_FORMAT_D;

		case renderer::PixelFormat::eD24S8:
			return GL_FORMAT_DS;

		case renderer::PixelFormat::eS8:
			return GL_FORMAT_S;

		default:
			assert( false && "Unsupported pixel format." );
			return GL_FORMAT_RGBA;
		}
	}

	GlType getType( renderer::PixelFormat format )noexcept
	{
		switch ( format )
		{
		case renderer::PixelFormat::eL8:
			return GL_TYPE_UI8;

		case renderer::PixelFormat::eA8L8:
			return GL_TYPE_UI8;

		case renderer::PixelFormat::eR8G8B8:
			return GL_TYPE_UI8;

		case renderer::PixelFormat::eR5G6B5:
			return GL_TYPE_UI565;

		case renderer::PixelFormat::eA8R8G8B8:
			return GL_TYPE_UI8888;

		case renderer::PixelFormat::eA8B8G8R8:
			return GL_TYPE_UI8888;

		case renderer::PixelFormat::eA1R5G5B5:
			return GL_TYPE_US5551;

		case renderer::PixelFormat::eA4R4G4B4:
			return GL_TYPE_US4444;

		case renderer::PixelFormat::eD16:
			return GL_TYPE_UI16;

		case renderer::PixelFormat::eD24S8:
			return GL_TYPE_UI24_8;

		case renderer::PixelFormat::eD32F:
			return GL_TYPE_F;

		case renderer::PixelFormat::eS8:
			return GL_TYPE_UI8;

		default:
			assert( false && "Unsupported pixel format." );
			return GL_TYPE_UI8888;
		}
	}

	uint32_t getSize( renderer::PixelFormat format )noexcept
	{
		switch ( format )
		{
		case renderer::PixelFormat::eL8:
			return 1u;

		case renderer::PixelFormat::eA8L8:
			return 2u;

		case renderer::PixelFormat::eR8G8B8:
			return 3u;

		case renderer::PixelFormat::eR5G6B5:
			return 2u;

		case renderer::PixelFormat::eA8R8G8B8:
			return 4u;

		case renderer::PixelFormat::eA8B8G8R8:
			return 4u;

		case renderer::PixelFormat::eA1R5G5B5:
			return 2u;

		case renderer::PixelFormat::eA4R4G4B4:
			return 2u;

		case renderer::PixelFormat::eD16:
			return 2u;

		case renderer::PixelFormat::eD24S8:
			return 4u;

		case renderer::PixelFormat::eD32F:
			return 4u;

		case renderer::PixelFormat::eS8:
			return 1u;

		default:
			assert( false && "Unsupported pixel format." );
			return 0u;
		}
	}

	renderer::PixelFormat convert( GlInternal format )noexcept
	{
		switch ( format )
		{
		case GL_INTERNAL_R8:
			return renderer::PixelFormat::eL8;

		case GL_INTERNAL_R8G8:
			return renderer::PixelFormat::eA8L8;

		case GL_INTERNAL_R8G8B8:
			return renderer::PixelFormat::eR8G8B8;

		case GL_INTERNAL_R5G6B5:
			return renderer::PixelFormat::eR5G6B5;

		case GL_INTERNAL_R8G8B8A8:
			return renderer::PixelFormat::eA8R8G8B8;

		case GL_INTERNAL_B8G8R8A8:
			return renderer::PixelFormat::eA8B8G8R8;

		case GL_INTERNAL_R5G5B5A1:
			return renderer::PixelFormat::eA1R5G5B5;

		case GL_INTERNAL_R4G4B4A4:
			return renderer::PixelFormat::eA4R4G4B4;

		case GL_INTERNAL_D16:
			return renderer::PixelFormat::eD16;

		case GL_INTERNAL_D24S8:
			return renderer::PixelFormat::eD24S8;

		case GL_INTERNAL_D32F:
			return renderer::PixelFormat::eD32F;

		case GL_INTERNAL_S8:
			return renderer::PixelFormat::eS8;

		default:
			assert( false && "Unsupported pixel format." );
			return renderer::PixelFormat::eA8R8G8B8;
		}
	}

	renderer::ImageAspectFlags getAspectFlag( renderer::PixelFormat format )
	{
		switch ( format )
		{
		case renderer::PixelFormat::eL8:
		case renderer::PixelFormat::eA8L8:
		case renderer::PixelFormat::eR8G8B8:
		case renderer::PixelFormat::eR5G6B5:
		case renderer::PixelFormat::eA8R8G8B8:
		case renderer::PixelFormat::eA8B8G8R8:
		case renderer::PixelFormat::eA1R5G5B5:
		case renderer::PixelFormat::eA4R4G4B4:
			return renderer::ImageAspectFlag::eColour;

		case renderer::PixelFormat::eD16:
		case renderer::PixelFormat::eD32F:
			return renderer::ImageAspectFlag::eDepth;

		case renderer::PixelFormat::eD24S8:
			return renderer::ImageAspectFlag::eDepth | renderer::ImageAspectFlag::eStencil;

		case renderer::PixelFormat::eS8:
			return renderer::ImageAspectFlag::eStencil;

		default:
			assert( false && "Unsupported pixel format." );
			return renderer::ImageAspectFlag::eColour;
		}
	}}

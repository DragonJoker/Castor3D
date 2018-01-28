#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlInternal value )
	{
		switch ( value )
		{
		case gl_renderer::GL_INTERNAL_R8:
			return "GL_R8";

		case gl_renderer::GL_INTERNAL_R8G8:
			return "GL_RG8";

		case gl_renderer::GL_INTERNAL_R8G8B8:
			return "GL_RGB8";

		case gl_renderer::GL_INTERNAL_R5G6B5:
			return "GL_R5_G6_B5";

		case gl_renderer::GL_INTERNAL_R8G8B8A8:
			return "GL_RGBA8";

		case gl_renderer::GL_INTERNAL_B8G8R8A8:
			return "GL_BGRA8";

		case gl_renderer::GL_INTERNAL_R5G5B5A1:
			return "GL_RGB5_A1";

		case gl_renderer::GL_INTERNAL_R4G4B4A4:
			return "GL_RGBA4";

		case gl_renderer::GL_INTERNAL_D16:
			return "GL_DEPTH_COMPONENT16";

		case gl_renderer::GL_INTERNAL_D24S8:
			return "GL_DEPTH_COMPONENT24_S8";

		case gl_renderer::GL_INTERNAL_D32F:
			return "GL_DEPTH_COMPONENT32F";

		case gl_renderer::GL_INTERNAL_S8:
			return "GL_STENCIL_COMPONENT8";

		default:
			assert( false && "Unupported GlInternal" );
			return "GlInternal_UNKNOWN";
		}
	}

	std::string getName( GlFormat value )
	{
		switch ( value )
		{
		case GL_FORMAT_S:
			return "GL_STENCIL_INDEX";

		case GL_FORMAT_D:
			return "GL_DEPTH_COMPONENT";

		case GL_FORMAT_R:
			return "GL_RED";

		case GL_FORMAT_RGB:
			return "GL_RGB";

		case GL_FORMAT_RGBA:
			return "GL_RGBA";

		case GL_FORMAT_ABGR:
			return "GL_ABGR";

		case GL_FORMAT_BGR:
			return "GL_BGR";

		case GL_FORMAT_BGRA:
			return "GL_BGRA";

		case GL_FORMAT_RG:
			return "GL_RG";

		case GL_FORMAT_DS:
			return "GL_DEPTH_STENCIL";

		default:
			assert( false && "Unupported GlFormat" );
			return "GlFormat_UNKNOWN";
		}
	}

	std::string getName( GlType value )
	{
		switch ( value )
		{
		case GL_TYPE_UI8:
			return "GL_UNSIGNED_BYTE";

		case GL_TYPE_UI16:
			return "GL_UNSIGNED_SHORT";

		case GL_TYPE_F:
			return "GL_FLOAT";

		case GL_TYPE_US4444:
			return "GL_UNSIGNED_SHORT_4_4_4_4";

		case GL_TYPE_US5551:
			return "GL_UNSIGNED_SHORT_5_5_5_1";

		case GL_TYPE_UI8888:
			return "GL_UNSIGNED_INT_8_8_8_8";

		case GL_TYPE_UI565:
			return "GL_UNSIGNED_SHORT_5_6_5";

		case GL_TYPE_UI24_8:
			return "GL_UNSIGNED_INT_24_8";

		default:
			assert( false && "Unupported GlType" );
			return "GlType_UNKNOWN";
		}
	}

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

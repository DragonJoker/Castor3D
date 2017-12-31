#include "GlGetFunction.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include <Windows.h>
#elif defined( CASTOR_PLATFORM_LINUX )
#	include <X11/Xlib.h>
#	include <GL/glx.h>
#	define GLX_GLXEXT_PROTOTYPES
#	include <GL/glxext.h>
#endif
#include <GL/gl.h>

namespace GlRender
{
	namespace gl_api
	{
		GlProc getProcAddress( castor::String const & p_name )
		{
#if defined( CASTOR_PLATFORM_WINDOWS )
			return wglGetProcAddress( castor::string::stringCast< char >( p_name ).c_str() );
#else
			return glXGetProcAddressARB( reinterpret_cast< GLubyte const * >( castor::string::stringCast< char >( p_name ).c_str() ) );
#endif
		}
	}
}

/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#include "Miscellaneous/OpenGLLibrary.hpp"

#if RENDERLIB_WIN32
#elif RENDERLIB_XLIB
#	include <X11/Xlib.h>
#	include <GL/glx.h>
#else
#	include <X11/Xlib.h>
#	include <GL/glx.h>
#endif

#include <GL/gl.h>

#include <iostream>
#include <stdexcept>

namespace gl_renderer
{
	namespace
	{
#if RENDERLIB_WIN32
		template< typename FuncT >
		bool getFunction( char const * const name, FuncT & function )
		{
			function = reinterpret_cast< FuncT >( wglGetProcAddress( name ) );
			return function != nullptr;
		}
#elif RENDERLIB_XLIB
		template< typename FuncT >
		bool getFunction( char const * const name, FuncT & function )
		{
			function = reinterpret_cast< FuncT >( glXGetProcAddressARB( reinterpret_cast< GLubyte const * >( name ) ) );
			return function != nullptr;
		}
#else
		template< typename FuncT >
		bool getFunction( char const * const name, FuncT & function )
		{
			function = reinterpret_cast< FuncT >( glXGetProcAddressARB( reinterpret_cast< GLubyte const * >( name ) ) );
			return function != nullptr;
		}
#endif
	}

	OpenGLLibrary::OpenGLLibrary()
	{
		try
		{
#define GL_LIB_BASE_FUNCTION( fun )\
			gl::fun = ( PFN_gl##fun )&gl##fun;

#define GL_LIB_FUNCTION( fun )\
			if ( !( getFunction( "gl"#fun, gl::fun ) ) )\
			{\
				throw std::runtime_error{ std::string{ "Couldn't load function " } + "gl"#fun };\
			}
#include "OpenGLFunctionsList.inl"

#if RENDERLIB_WIN32
#	define WGL_LIB_BASE_FUNCTION( fun )\
			wgl::fun = &wgl##fun;
#	define WGL_LIB_FUNCTION( fun )\
			if ( !( getFunction( "wgl"#fun, wgl::fun ) ) )\
			{\
				throw std::runtime_error{ std::string{ "Couldn't load function " } + "wgl"#fun };\
			}
#	include "OpenGLFunctionsList.inl"
#elif RENDERLIB_XLIB
#else
#endif
		}
		catch ( std::exception & error )
		{
			std::cerr << error.what() << std::endl;
		}
	}
}

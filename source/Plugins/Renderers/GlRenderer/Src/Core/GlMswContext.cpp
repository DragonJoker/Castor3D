#if defined( _WIN32 )

#include "Core/GlMswContext.hpp"
#include "Miscellaneous/GlDebug.hpp"

#include <Core/PlatformWindowHandle.hpp>

#include <Windows.h>

namespace gl_renderer
{
	namespace
	{
#if !defined( NDEBUG )

		static const int GL_CONTEXT_CREATION_DEFAULT_FLAGS =  GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT | GL_CONTEXT_FLAG_DEBUG_BIT;
		static const int GL_CONTEXT_CREATION_DEFAULT_MASK = GL_CONTEXT_CORE_PROFILE_BIT;

#else

		static const int GL_CONTEXT_CREATION_DEFAULT_FLAGS = GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT;
		static const int GL_CONTEXT_CREATION_DEFAULT_MASK = GL_CONTEXT_CORE_PROFILE_BIT;

#endif
	}

	MswContext::MswContext( renderer::ConnectionPtr && connection )
		: Context{ std::move( connection ) }
		, m_hDC( nullptr )
		, m_hContext( nullptr )
		, m_hWnd( m_connection->getHandle().getInternal< renderer::IMswWindowHandle >().getHwnd() )
	{
		m_hDC = ::GetDC( m_hWnd );

		if ( doSelectPixelFormat() )
		{
			m_hContext = wglCreateContext( m_hDC );
			setCurrent();
			m_opengl = std::make_unique< OpenGLLibrary >();
			m_vendor = ( char const * )gl::GetString( GL_VENDOR );
			m_renderer = ( char const * )gl::GetString( GL_RENDERER );
			m_version = ( char const * )gl::GetString( GL_VERSION );
			loadDebugFunctions();
			endCurrent();

			double fversion{ 0u };
			std::stringstream stream( m_version );
			stream >> fversion;
			auto version = int( fversion * 10 );
			m_major = version / 10;
			m_minor = version % 10;

			if ( version >= 33 )
			{
				m_glslVersion = version * 10;
			}
			else if ( version >= 32 )
			{
				m_glslVersion = 150;
			}
			else if ( version >= 31 )
			{
				m_glslVersion = 140;
			}
			else if ( version >= 30 )
			{
				m_glslVersion = 130;
			}
			else if ( version >= 21 )
			{
				m_glslVersion = 120;
			}
			else if ( version >= 20 )
			{
				m_glslVersion = 110;
			}
			else
			{
				m_glslVersion = 100;
			}

			if ( !doCreateGl3Context() )
			{
				wgl::DeleteContext( m_hContext );
				throw std::runtime_error{ "The supported OpenGL version is insufficient." };
			}

			setCurrent();
			glLogCall( gl::ClipControl, GL_UPPER_LEFT, GL_ZERO_TO_ONE );
			initialiseDebugFunctions();
			wgl::SwapIntervalEXT( 0 );
			endCurrent();
		}
	}

	MswContext::~MswContext()
	{
		try
		{
			if ( m_hDC )
			{
				wglDeleteContext( m_hContext );
				::ReleaseDC( m_hWnd, m_hDC );
			}
		}
		catch ( ... )
		{
		}
	}

	void MswContext::setCurrent()const
	{
		wglMakeCurrent( m_hDC, m_hContext );
	}

	void MswContext::endCurrent()const
	{
		wglMakeCurrent( nullptr, nullptr );
	}

	void MswContext::swapBuffers()const
	{
		::SwapBuffers( m_hDC );
	}

	HGLRC MswContext::doCreateDummyContext()
	{
		HGLRC result = nullptr;

		if ( doSelectPixelFormat() )
		{
			result = wglCreateContext( m_hDC );
		}

		return result;
	}

	bool MswContext::doSelectPixelFormat()
	{
		bool result = false;
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.iLayerType = PFD_MAIN_PLANE;
		pfd.cColorBits = 24;
		pfd.cRedBits = 8;
		pfd.cGreenBits = 8;
		pfd.cBlueBits = 8;
		pfd.cDepthBits = 0;

		int pixelFormats = ::ChoosePixelFormat( m_hDC, &pfd );

		if ( pixelFormats )
		{
			result = ::SetPixelFormat( m_hDC, pixelFormats, &pfd ) != FALSE;
		}

		return result;
	}

	bool MswContext::doCreateGl3Context()
	{
		bool result = false;

		try
		{
			using PFNGLCREATECONTEXTATTRIBS = HGLRC(*)( HDC hDC, HGLRC hShareContext, int const * attribList );
			PFNGLCREATECONTEXTATTRIBS glCreateContextAttribs;
			HGLRC hContext = m_hContext;
			std::vector< int > attribList
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, m_major,
				WGL_CONTEXT_MINOR_VERSION_ARB, m_minor,
				WGL_CONTEXT_FLAGS_ARB, GL_CONTEXT_CREATION_DEFAULT_FLAGS,
				WGL_CONTEXT_PROFILE_MASK_ARB, GL_CONTEXT_CREATION_DEFAULT_MASK,
				0
			};

			setCurrent();
			gl::GetError();
			glCreateContextAttribs = ( PFNGLCREATECONTEXTATTRIBS )wglGetProcAddress( "wglCreateContextAttribsARB" );
			hContext = glCreateContextAttribs( m_hDC, nullptr, attribList.data() );
			endCurrent();
			wgl::DeleteContext( m_hContext );
			m_hContext = hContext;
			result = m_hContext != nullptr;

			if ( !result )
			{
				std::stringstream error;
				error << "Failed to create a " << m_major << "." << m_minor << " OpenGL context (0x" << std::hex << gl::GetError() << ").";
				throw std::runtime_error{ error.str() };
			}
		}
		catch ( ... )
		{
			result = false;
		}

		return result;
	}
}

#endif

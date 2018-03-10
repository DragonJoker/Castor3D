#include "Core/GlMswContext.hpp"

#if defined( _WIN32 )

#include "Core/GlPhysicalDevice.hpp"
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

	MswContext::MswContext( PhysicalDevice const & gpu
		, renderer::ConnectionPtr && connection )
		: Context{ gpu, std::move( connection ) }
		, m_hDC( nullptr )
		, m_hContext( nullptr )
		, m_hWnd( m_connection->getHandle().getInternal< renderer::IMswWindowHandle >().getHwnd() )
	{
		m_hDC = ::GetDC( m_hWnd );

		if ( doSelectFormat() )
		{
			m_hContext = wglCreateContext( m_hDC );
			setCurrent();
			m_opengl = std::make_unique< OpenGLLibrary >();
			loadDebugFunctions();
			endCurrent();

			double fversion{ 0u };

			if ( !doCreateGl3Context() )
			{
				wgl::DeleteContext( m_hContext );
				throw std::runtime_error{ "The supported OpenGL version is insufficient." };
			}

			setCurrent();
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

		if ( doSelectFormat() )
		{
			result = wglCreateContext( m_hDC );
		}

		return result;
	}

	bool MswContext::doSelectFormat()
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
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;

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
				WGL_CONTEXT_MAJOR_VERSION_ARB, m_gpu.getMajor(),
				WGL_CONTEXT_MINOR_VERSION_ARB, m_gpu.getMinor(),
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
				error << "Failed to create a " << m_gpu.getMajor() << "." << m_gpu.getMinor() << " OpenGL context (0x" << std::hex << gl::GetError() << ").";
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

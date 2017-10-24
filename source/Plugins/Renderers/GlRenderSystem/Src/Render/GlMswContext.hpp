/*
See LICENSE file in root folder
*/
#ifndef ___GL_MSW_CONTEXT_H___
#define ___GL_MSW_CONTEXT_H___

#ifdef CASTOR_PLATFORM_WINDOWS
#include "Common/GlHolder.hpp"
#include "Common/OpenGl.hpp"

#include <Miscellaneous/GpuInformations.hpp>

#ifndef _WINDEF_

#define DECLARE_WIN_HANDLE( Name ) struct Name##__{int unused;}; typedef struct Name##__ *Name
DECLARE_WIN_HANDLE( HDC );
DECLARE_WIN_HANDLE( HGLRC );
DECLARE_WIN_HANDLE( HWND );

#endif

namespace GlRender
{
	class GlContext;

	class GlContextImpl
		: public Holder
	{
	public:
		GlContextImpl( OpenGl & p_gl, GlContext * p_context );
		~GlContextImpl();

		bool initialise( castor3d::RenderWindow * p_window );
		void cleanup();
		void setCurrent();
		void endCurrent();
		void swapBuffers();
		void updateVSync( bool p_enable );

		inline HDC getHDC()const
		{
			return m_hDC;
		}

		inline HGLRC getContext()const
		{
			return m_hContext;
		}

		inline castor3d::GpuInformations && getGpuInformations()
		{
			return std::move( m_gpuInformations );
		}

	protected:
		void doInitialiseOpenGL( castor::PixelFormat p_colour, bool p_stereo );
		HGLRC doCreateDummyContext( castor::PixelFormat p_colour, bool p_stereo );
		bool doSelectPixelFormat( castor::PixelFormat p_colour, bool p_stereo );
		bool doSelectStereoPixelFormat( castor::PixelFormat p_colour );
		bool doCreateGl3Context();

	protected:
		HDC m_hDC;
		HGLRC m_hContext;
		HWND m_hWnd;
		GlContext * m_context;
		bool m_initialised;
		castor3d::GpuInformations m_gpuInformations;
	};
}

#undef DECLARE_WIN_HANDLE
#endif

#endif

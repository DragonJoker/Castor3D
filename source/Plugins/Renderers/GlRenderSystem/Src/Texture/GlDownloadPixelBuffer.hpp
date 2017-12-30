/*
See LICENSE file in root folder
*/
#ifndef ___GL_DOWNLOAD_PIXEL_BUFFER_H___
#define ___GL_DOWNLOAD_PIXEL_BUFFER_H___

#include "Buffer/GlGpuIoBuffer.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\brief		Pack (download to RAM) buffer implementation
	*/
	class GlDownloadPixelBuffer
		: public GlGpuIoBuffer
	{
	public:
		GlDownloadPixelBuffer( OpenGl & p_gl, GlRenderSystem * renderSystem, uint32_t p_pixelsSize );
		virtual ~GlDownloadPixelBuffer();

	private:
		bool doInitialise()override;
	};
}

#endif

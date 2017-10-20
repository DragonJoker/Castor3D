/*
See LICENSE file in root folder
*/
#ifndef ___GL_UPLOAD_PIXEL_BUFFER_H___
#define ___GL_UPLOAD_PIXEL_BUFFER_H___

#include "Buffer/GlGpuIoBuffer.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\brief		Unpack (upload to VRAM) buffer implementation
	*/
	class GlUploadPixelBuffer
		: public GlGpuIoBuffer
	{
	public:
		GlUploadPixelBuffer( OpenGl & p_gl, GlRenderSystem * renderSystem, uint32_t p_pixelsSize );
		virtual ~GlUploadPixelBuffer();

	private:
		bool doInitialise()override;
	};
}

#endif

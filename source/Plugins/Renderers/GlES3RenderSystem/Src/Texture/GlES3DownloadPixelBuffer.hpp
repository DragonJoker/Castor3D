/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_DOWNLOAD_PIXEL_BUFFER_H___
#define ___C3DGLES3_DOWNLOAD_PIXEL_BUFFER_H___

#include "Buffer/GlES3GpuIoBuffer.hpp"

namespace GlES3Render
{
	/*!
	\author		Sylvain DOREMUS
	\brief		Pack (download to RAM) buffer implementation
	*/
	class GlES3DownloadPixelBuffer
		: public GlES3GpuIoBuffer
	{
	public:
		GlES3DownloadPixelBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize );
		GlES3DownloadPixelBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, Castor::PxBufferBaseSPtr p_pixels );
		virtual ~GlES3DownloadPixelBuffer();

	private:
		bool DoInitialise()override;
	};
}

#endif

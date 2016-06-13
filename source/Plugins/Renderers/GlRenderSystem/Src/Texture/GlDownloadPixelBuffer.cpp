#include "Texture/GlDownloadPixelBuffer.hpp"

#include "Common/OpenGl.hpp"

#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDownloadPixelBuffer::GlDownloadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pixels, p_pixelsSize, eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_READ )
	{
	}

	GlDownloadPixelBuffer::GlDownloadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, PxBufferBaseSPtr p_pixels )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pixels->ptr(), p_pixels->size(), eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_READ )
	{
	}

	GlDownloadPixelBuffer::~GlDownloadPixelBuffer()
	{
	}

	bool GlDownloadPixelBuffer::Initialise()
	{
		return Fill( m_pixels, m_pixelsSize );
	}
}

#include "GlUploadPixelBuffer.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlUploadPixelBuffer::GlUploadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pixels, p_pixelsSize, eGL_BUFFER_TARGET_PIXEL_UNPACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW )
	{
	}

	GlUploadPixelBuffer::GlUploadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, PxBufferBaseSPtr p_pixels )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pixels->ptr(), p_pixels->size(), eGL_BUFFER_TARGET_PIXEL_UNPACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW )
	{
	}

	GlUploadPixelBuffer::~GlUploadPixelBuffer()
	{
	}

	bool GlUploadPixelBuffer::Initialise()
	{
		return Fill( NULL, m_pixelsSize );
	}
}

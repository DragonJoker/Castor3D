#include "GlDownloadPixelBuffer.hpp"
#include "OpenGl.hpp"

#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDownloadPixelBuffer::GlDownloadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pPixels, p_uiPixelsSize, eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_READ )
	{
	}

	GlDownloadPixelBuffer::GlDownloadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, PxBufferBaseSPtr p_pPixels )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pPixels->ptr(), p_pPixels->size(), eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_READ )
	{
	}

	GlDownloadPixelBuffer::~GlDownloadPixelBuffer()
	{
	}

	bool GlDownloadPixelBuffer::Initialise()
	{
		bool l_return = Create();

		if ( l_return )
		{
			Fill( m_pPixels, m_uiPixelsSize );
		}

		return l_return;
	}
}

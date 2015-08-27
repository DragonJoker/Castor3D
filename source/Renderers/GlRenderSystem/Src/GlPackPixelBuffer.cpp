#include "GlPackPixelBuffer.hpp"
#include "OpenGl.hpp"

#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPackPixelBuffer::GlPackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize )
		:	GlPixelBuffer( p_gl, p_pRenderSystem, p_pPixels, p_uiPixelsSize, eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_READ	)
	{
	}

	GlPackPixelBuffer::GlPackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, PxBufferBaseSPtr p_pPixels )
		:	GlPixelBuffer( p_gl, p_pRenderSystem, p_pPixels->ptr(), p_pPixels->size(), eGL_BUFFER_TARGET_PIXEL_PACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_READ	)
	{
	}

	GlPackPixelBuffer::~GlPackPixelBuffer()
	{
	}

	void GlPackPixelBuffer::Initialise()
	{
		Create();

		if ( Bind() )
		{
			Fill( m_pPixels, m_uiPixelsSize );
			Unbind();
		}
	}
}

#include "GlUnpackPixelBuffer.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlUnpackPixelBuffer::GlUnpackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize )
		:	GlPixelBuffer( p_gl, p_pRenderSystem, p_pPixels, p_uiPixelsSize, eGL_BUFFER_TARGET_PIXEL_UNPACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW	)
	{
	}

	GlUnpackPixelBuffer::GlUnpackPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, PxBufferBaseSPtr p_pPixels )
		:	GlPixelBuffer( p_gl, p_pRenderSystem, p_pPixels->ptr(), p_pPixels->size(), eGL_BUFFER_TARGET_PIXEL_UNPACK, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW	)
	{
	}

	GlUnpackPixelBuffer::~GlUnpackPixelBuffer()
	{
	}

	void GlUnpackPixelBuffer::Initialise()
	{
		Create();

		if ( Bind() )
		{
			Fill( NULL, m_uiPixelsSize );
			Unbind();
		}
	}
}

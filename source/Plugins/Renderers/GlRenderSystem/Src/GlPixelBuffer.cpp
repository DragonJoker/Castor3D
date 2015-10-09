#include "GlPixelBuffer.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPixelBuffer::GlPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize, eGL_BUFFER_TARGET p_ePackMode, eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_eNature )
		:	GlBufferBase< uint8_t >( p_gl, p_ePackMode )
		,	m_uiPixelsSize( p_uiPixelsSize )
		,	m_pPixels( p_pPixels )
		,	m_ePackMode( p_ePackMode )
		,	m_eAccessType( p_type )
		,	m_eAccessNature( p_eNature )
		,	m_renderSystem( p_renderSystem )
	{
	}

	GlPixelBuffer::~GlPixelBuffer()
	{
	}

	bool GlPixelBuffer::Activate()
	{
		return GlBufferBase::Bind();
	}

	void GlPixelBuffer::Deactivate()
	{
		GlBufferBase::Unbind();
	}

	bool GlPixelBuffer::Fill( uint8_t * p_pBuffer, ptrdiff_t p_iSize )
	{
		return GlBufferBase::Fill( p_pBuffer, p_iSize, m_eAccessType, m_eAccessNature );
	}
}

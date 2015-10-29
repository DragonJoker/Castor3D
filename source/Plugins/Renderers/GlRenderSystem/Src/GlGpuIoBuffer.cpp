#include "GlGpuIoBuffer.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGpuIoBuffer::GlGpuIoBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pPixels, uint32_t p_uiPixelsSize, eGL_BUFFER_TARGET p_ePackMode, eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_eNature )
		: GlBufferBase< uint8_t >( p_gl, p_ePackMode )
		, m_uiPixelsSize( p_uiPixelsSize )
		, m_pPixels( p_pPixels )
		, m_ePackMode( p_ePackMode )
		, m_eAccessType( p_type )
		, m_eAccessNature( p_eNature )
		, m_renderSystem( p_renderSystem )
	{
	}

	GlGpuIoBuffer::~GlGpuIoBuffer()
	{
	}

	bool GlGpuIoBuffer::Activate()
	{
		return GlBufferBase::Bind();
	}

	void GlGpuIoBuffer::Deactivate()
	{
		GlBufferBase::Unbind();
	}

	bool GlGpuIoBuffer::Fill( uint8_t * p_buffer, ptrdiff_t p_iSize )
	{
		return GlBufferBase::Fill( p_buffer, p_iSize, m_eAccessType, m_eAccessNature );
	}
}

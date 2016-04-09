#include "GlGpuIoBuffer.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGpuIoBuffer::GlGpuIoBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize, eGL_BUFFER_TARGET p_packMode, eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature )
		: GlBufferBase< uint8_t >( p_gl, p_packMode )
		, m_pixelsSize( p_pixelsSize )
		, m_pixels( p_pixels )
		, m_accessType( p_type )
		, m_accessNature( p_nature )
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

	bool GlGpuIoBuffer::Fill( uint8_t * p_buffer, ptrdiff_t p_size )
	{
		return GlBufferBase::Fill( p_buffer, p_size, m_accessType, m_accessNature );
	}
}

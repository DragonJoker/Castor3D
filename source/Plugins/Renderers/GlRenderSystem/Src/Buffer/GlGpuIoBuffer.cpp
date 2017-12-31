#include "Buffer/GlGpuIoBuffer.hpp"

#include "Common/OpenGl.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlGpuIoBuffer::GlGpuIoBuffer( OpenGl & p_gl, GlRenderSystem * renderSystem, uint32_t p_pixelsSize, GlBufferTarget p_packMode, BufferAccessType p_type, BufferAccessNature p_nature )
		: GlBufferBase< uint8_t >{ p_gl, p_packMode }
		, m_pixelsSize{ p_pixelsSize }
		, m_accessType{ p_type }
		, m_accessNature{ p_nature }
		, m_renderSystem{ renderSystem }
	{
	}

	GlGpuIoBuffer::~GlGpuIoBuffer()
	{
	}

	bool GlGpuIoBuffer::initialise()
	{
		initialiseStorage( m_pixelsSize, m_accessType, m_accessNature );
		return doInitialise();
	}
}

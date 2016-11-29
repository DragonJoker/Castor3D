#include "Buffer/GlGpuIoBuffer.hpp"

#include "Common/OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGpuIoBuffer::GlGpuIoBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize, GlBufferTarget p_packMode, BufferAccessType p_type, BufferAccessNature p_nature )
		: GlBufferBase< uint8_t >{ p_gl, p_packMode }
		, m_pixelsSize{ p_pixelsSize }
		, m_pixels{ p_pixels }
		, m_accessType{ p_type }
		, m_accessNature{ p_nature }
		, m_renderSystem{ p_renderSystem }
	{
	}

	GlGpuIoBuffer::~GlGpuIoBuffer()
	{
	}

	bool GlGpuIoBuffer::Initialise()
	{
		bool l_return = InitialiseStorage( m_pixelsSize, m_accessType, m_accessNature );

		if ( l_return )
		{
			l_return = DoInitialise();
		}

		return l_return;
	}
}

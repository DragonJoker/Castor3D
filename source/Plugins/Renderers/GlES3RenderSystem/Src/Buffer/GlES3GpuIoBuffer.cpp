#include "Buffer/GlES3GpuIoBuffer.hpp"

#include "Common/OpenGlES3.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3GpuIoBuffer::GlES3GpuIoBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize, GlES3BufferTarget p_packMode, BufferAccessType p_type, BufferAccessNature p_nature )
		: GlES3BufferBase< uint8_t >{ p_gl, p_packMode }
		, m_pixelsSize{ p_pixelsSize }
		, m_pixels{ p_pixels }
		, m_accessType{ p_type }
		, m_accessNature{ p_nature }
		, m_renderSystem{ p_renderSystem }
	{
	}

	GlES3GpuIoBuffer::~GlES3GpuIoBuffer()
	{
	}

	bool GlES3GpuIoBuffer::Initialise()
	{
		InitialiseStorage( m_pixelsSize, m_accessType, m_accessNature );
		return DoInitialise();
	}
}

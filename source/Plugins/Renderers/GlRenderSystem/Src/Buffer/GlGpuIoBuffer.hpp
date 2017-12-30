/*
See LICENSE file in root folder
*/
#ifndef ___GL_GPU_IO_BUFFER_H___
#define ___GL_GPU_IO_BUFFER_H___

#include "Buffer/GlBufferBase.hpp"

namespace GlRender
{
	class GlGpuIoBuffer
		: public GlBufferBase< uint8_t >
	{
	public:
		GlGpuIoBuffer( OpenGl & p_gl, GlRenderSystem * renderSystem, uint32_t p_pixelsSize, GlBufferTarget p_packMode, castor3d::BufferAccessType p_type, castor3d::BufferAccessNature p_nature );
		virtual ~GlGpuIoBuffer();
		bool initialise();

	private:
		virtual bool doInitialise() = 0;

	protected:
		castor3d::BufferAccessType m_accessType;
		castor3d::BufferAccessNature m_accessNature;
		uint32_t m_pixelsSize;
		GlRenderSystem * m_renderSystem;
	};
}

#endif

/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_GPU_IO_BUFFER_H___
#define ___C3DGLES3_GPU_IO_BUFFER_H___

#include "Buffer/GlES3BufferBase.hpp"

namespace GlES3Render
{
	class GlES3GpuIoBuffer
		: public GlES3BufferBase< uint8_t >
	{
	public:
		GlES3GpuIoBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize, GlES3BufferTarget p_packMode, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature );
		virtual ~GlES3GpuIoBuffer();
		bool Initialise();

	private:
		virtual bool DoInitialise() = 0;

	protected:
		Castor3D::BufferAccessType m_accessType;
		Castor3D::BufferAccessNature m_accessNature;
		uint8_t	* m_pixels;
		uint32_t m_pixelsSize;
		GlES3RenderSystem * m_renderSystem;
	};
}

#endif

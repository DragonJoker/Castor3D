#include "Texture/GlES3UploadPixelBuffer.hpp"

#include "Common/OpenGlES3.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3UploadPixelBuffer::GlES3UploadPixelBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize )
		: GlES3GpuIoBuffer( p_gl, p_renderSystem, p_pixels, p_pixelsSize, GlES3BufferTarget::ePixelUnpack, BufferAccessType::eStream, BufferAccessNature::eDraw )
	{
	}

	GlES3UploadPixelBuffer::GlES3UploadPixelBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, PxBufferBaseSPtr p_pixels )
		: GlES3GpuIoBuffer( p_gl, p_renderSystem, p_pixels->ptr(), p_pixels->size(), GlES3BufferTarget::ePixelUnpack, BufferAccessType::eStream, BufferAccessNature::eDraw )
	{
	}

	GlES3UploadPixelBuffer::~GlES3UploadPixelBuffer()
	{
	}

	bool GlES3UploadPixelBuffer::DoInitialise()
	{
		return true;
	}
}

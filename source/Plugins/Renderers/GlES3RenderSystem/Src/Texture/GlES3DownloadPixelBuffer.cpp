#include "Texture/GlES3DownloadPixelBuffer.hpp"

#include "Common/OpenGlES3.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3DownloadPixelBuffer::GlES3DownloadPixelBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, uint8_t * p_pixels, uint32_t p_pixelsSize )
		: GlES3GpuIoBuffer( p_gl, p_renderSystem, p_pixels, p_pixelsSize, GlES3BufferTarget::ePixelPack, BufferAccessType::eStream, BufferAccessNature::eRead )
	{
	}

	GlES3DownloadPixelBuffer::GlES3DownloadPixelBuffer( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, PxBufferBaseSPtr p_pixels )
		: GlES3GpuIoBuffer( p_gl, p_renderSystem, p_pixels->ptr(), p_pixels->size(), GlES3BufferTarget::ePixelPack, BufferAccessType::eStream, BufferAccessNature::eRead )
	{
	}

	GlES3DownloadPixelBuffer::~GlES3DownloadPixelBuffer()
	{
	}

	bool GlES3DownloadPixelBuffer::DoInitialise()
	{
		return true;
	}
}

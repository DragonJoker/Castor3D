#include "Texture/GlDownloadPixelBuffer.hpp"

#include "Common/OpenGl.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDownloadPixelBuffer::GlDownloadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint32_t p_pixelsSize )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pixelsSize, GlBufferTarget::ePixelPack, BufferAccessType::eStream, BufferAccessNature::eRead )
	{
	}

	GlDownloadPixelBuffer::~GlDownloadPixelBuffer()
	{
	}

	bool GlDownloadPixelBuffer::DoInitialise()
	{
		return true;
	}
}

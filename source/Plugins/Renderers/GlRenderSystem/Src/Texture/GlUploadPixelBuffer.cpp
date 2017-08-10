#include "Texture/GlUploadPixelBuffer.hpp"

#include "Common/OpenGl.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlUploadPixelBuffer::GlUploadPixelBuffer( OpenGl & p_gl, GlRenderSystem * renderSystem, uint32_t p_pixelsSize )
		: GlGpuIoBuffer( p_gl, renderSystem, p_pixelsSize, GlBufferTarget::ePixelUnpack, BufferAccessType::eStream, BufferAccessNature::eDraw )
	{
	}

	GlUploadPixelBuffer::~GlUploadPixelBuffer()
	{
	}

	bool GlUploadPixelBuffer::doInitialise()
	{
		return true;
	}
}

#include "Texture/GlUploadPixelBuffer.hpp"

#include "Common/OpenGl.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlUploadPixelBuffer::GlUploadPixelBuffer( OpenGl & p_gl, GlRenderSystem * p_renderSystem, uint32_t p_pixelsSize )
		: GlGpuIoBuffer( p_gl, p_renderSystem, p_pixelsSize, GlBufferTarget::ePixelUnpack, BufferAccessType::eStream, BufferAccessNature::eDraw )
	{
	}

	GlUploadPixelBuffer::~GlUploadPixelBuffer()
	{
	}

	bool GlUploadPixelBuffer::DoInitialise()
	{
		return true;
	}
}

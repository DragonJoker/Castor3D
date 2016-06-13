#include "FrameBuffer/TestRenderBufferAttachment.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestRenderBufferAttachment::TestRenderBufferAttachment( RenderBufferSPtr p_renderBuffer )
		: RenderBufferAttachment( p_renderBuffer )
	{
	}

	TestRenderBufferAttachment::~TestRenderBufferAttachment()
	{
	}

	bool TestRenderBufferAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		return true;
	}

	bool TestRenderBufferAttachment::DoAttach( FrameBufferSPtr p_frameBuffer )
	{
		return true;
	}

	void TestRenderBufferAttachment::DoDetach()
	{
	}
}

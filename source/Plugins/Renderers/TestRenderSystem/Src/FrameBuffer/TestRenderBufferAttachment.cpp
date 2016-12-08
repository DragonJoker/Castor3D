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

	void TestRenderBufferAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
	}

	void TestRenderBufferAttachment::DoAttach()
	{
	}

	void TestRenderBufferAttachment::DoDetach()
	{
	}
}

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

	void TestRenderBufferAttachment::DoAttach()
	{
	}

	void TestRenderBufferAttachment::DoDetach()
	{
	}
}

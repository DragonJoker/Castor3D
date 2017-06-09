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

	void TestRenderBufferAttachment::DoClear( Colour const & p_colour )const
	{
	}

	void TestRenderBufferAttachment::DoClear( float p_depth )const
	{
	}

	void TestRenderBufferAttachment::DoClear( int p_stencil )const
	{
	}

	void TestRenderBufferAttachment::DoClear( float p_depth, int p_stencil )const
	{
	}
}

#include "FrameBuffer/TestRenderBufferAttachment.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestRenderBufferAttachment::TestRenderBufferAttachment( RenderBufferSPtr p_renderBuffer )
		: RenderBufferAttachment( p_renderBuffer )
	{
	}

	void TestRenderBufferAttachment::doDownload( castor::Position const & p_offset
		, castor::PxBufferBase & p_buffer )const
	{
	}

	TestRenderBufferAttachment::~TestRenderBufferAttachment()
	{
	}

	void TestRenderBufferAttachment::doAttach()
	{
	}

	void TestRenderBufferAttachment::doDetach()
	{
	}

	void TestRenderBufferAttachment::doClear( Colour const & p_colour )const
	{
	}

	void TestRenderBufferAttachment::doClear( float p_depth )const
	{
	}

	void TestRenderBufferAttachment::doClear( int p_stencil )const
	{
	}

	void TestRenderBufferAttachment::doClear( float p_depth, int p_stencil )const
	{
	}
}

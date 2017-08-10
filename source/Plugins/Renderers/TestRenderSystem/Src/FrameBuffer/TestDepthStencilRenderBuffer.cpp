#include "FrameBuffer/TestDepthStencilRenderBuffer.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestDepthStencilRenderBuffer::TestDepthStencilRenderBuffer( PixelFormat p_format )
		: DepthStencilRenderBuffer( p_format )
	{
	}

	TestDepthStencilRenderBuffer::~TestDepthStencilRenderBuffer()
	{
	}

	bool TestDepthStencilRenderBuffer::create()
	{
		return true;
	}

	void TestDepthStencilRenderBuffer::destroy()
	{
	}

	bool TestDepthStencilRenderBuffer::initialise( castor::Size const & p_size )
	{
		m_size = p_size;
		return true;
	}

	void TestDepthStencilRenderBuffer::cleanup()
	{
	}

	void TestDepthStencilRenderBuffer::bind()
	{
	}

	void TestDepthStencilRenderBuffer::unbind()
	{
	}

	bool TestDepthStencilRenderBuffer::resize( castor::Size const & p_size )
	{
		m_size = p_size;
		return true;
	}
}

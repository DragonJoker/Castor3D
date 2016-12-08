#include "FrameBuffer/TestDepthStencilRenderBuffer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestDepthStencilRenderBuffer::TestDepthStencilRenderBuffer( PixelFormat p_format )
		: DepthStencilRenderBuffer( p_format )
	{
	}

	TestDepthStencilRenderBuffer::~TestDepthStencilRenderBuffer()
	{
	}

	bool TestDepthStencilRenderBuffer::Create()
	{
		return true;
	}

	void TestDepthStencilRenderBuffer::Destroy()
	{
	}

	bool TestDepthStencilRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		m_size = p_size;
		return true;
	}

	void TestDepthStencilRenderBuffer::Cleanup()
	{
	}

	void TestDepthStencilRenderBuffer::Bind()
	{
	}

	void TestDepthStencilRenderBuffer::Unbind()
	{
	}

	bool TestDepthStencilRenderBuffer::Resize( Castor::Size const & p_size )
	{
		m_size = p_size;
		return true;
	}
}

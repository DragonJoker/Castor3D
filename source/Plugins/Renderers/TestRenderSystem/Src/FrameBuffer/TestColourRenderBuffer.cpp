#include "FrameBuffer/TestColourRenderBuffer.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestColourRenderBuffer::TestColourRenderBuffer( PixelFormat p_format )
		: ColourRenderBuffer( p_format )
	{
	}

	TestColourRenderBuffer::~TestColourRenderBuffer()
	{
	}

	bool TestColourRenderBuffer::create()
	{
		return true;
	}

	void TestColourRenderBuffer::destroy()
	{
	}

	bool TestColourRenderBuffer::initialise( castor::Size const & p_size )
	{
		return true;
	}

	void TestColourRenderBuffer::cleanup()
	{
	}

	void TestColourRenderBuffer::bind()
	{
	}

	void TestColourRenderBuffer::unbind()
	{
	}

	bool TestColourRenderBuffer::resize( castor::Size const & p_size )
	{
		return true;
	}
}

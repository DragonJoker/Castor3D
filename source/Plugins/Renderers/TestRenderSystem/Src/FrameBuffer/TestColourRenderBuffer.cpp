#include "FrameBuffer/TestColourRenderBuffer.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestColourRenderBuffer::TestColourRenderBuffer( ePIXEL_FORMAT p_format )
		: ColourRenderBuffer( p_format )
	{
	}

	TestColourRenderBuffer::~TestColourRenderBuffer()
	{
	}

	bool TestColourRenderBuffer::Create()
	{
		return true;
	}

	void TestColourRenderBuffer::Destroy()
	{
	}

	bool TestColourRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		return true;
	}

	void TestColourRenderBuffer::Cleanup()
	{
	}

	bool TestColourRenderBuffer::Bind()
	{
		return true;
	}

	void TestColourRenderBuffer::Unbind()
	{
	}

	bool TestColourRenderBuffer::Resize( Castor::Size const & p_size )
	{
		return true;
	}
}

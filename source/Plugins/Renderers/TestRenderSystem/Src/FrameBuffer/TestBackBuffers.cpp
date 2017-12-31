#include "FrameBuffer/TestBackBuffers.hpp"

#include <Graphics/PixelBuffer.hpp>

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestBackBuffers::TestBackBuffers( Engine & engine )
		: BackBuffers( engine )
	{
	}

	TestBackBuffers::~TestBackBuffers()
	{
	}

	bool TestBackBuffers::initialise()
	{
		return true;
	}

	void TestBackBuffers::cleanup()
	{
	}

	void TestBackBuffers::bind( WindowBuffer p_buffer, FrameBufferTarget p_target )const
	{
	}

	void TestBackBuffers::setDrawBuffers( FrameBuffer::AttachArray const & p_attaches )const
	{
	}

	void TestBackBuffers::setReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
	}

	void TestBackBuffers::downloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )const
	{
	}

	void TestBackBuffers::doClear( castor3d::BufferComponents p_uiTargets )const
	{
	}

	void TestBackBuffers::doBlitInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
	}

	void TestBackBuffers::doStretchInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
	}
}

#include "FrameBuffer/TestBackBuffers.hpp"

#include <Graphics/PixelBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestBackBuffers::TestBackBuffers( Engine & p_engine )
		: BackBuffers( p_engine )
	{
	}

	TestBackBuffers::~TestBackBuffers()
	{
	}

	bool TestBackBuffers::Create()
	{
		return true;
	}

	void TestBackBuffers::Destroy()
	{
	}

	bool TestBackBuffers::Bind( eBUFFER p_buffer, FrameBufferTarget p_target )const
	{
		return true;
	}

	void TestBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )const
	{
	}

	void TestBackBuffers::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
	}

	bool TestBackBuffers::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		return true;
	}

	void TestBackBuffers::DoClear( uint32_t p_uiTargets )
	{
	}

	bool TestBackBuffers::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components )const
	{
		return false;
	}

	bool TestBackBuffers::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, InterpolationMode p_interpolation )const
	{
		return false;
	}
}

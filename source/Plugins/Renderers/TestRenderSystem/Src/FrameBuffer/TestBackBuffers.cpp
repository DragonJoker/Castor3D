#include "FrameBuffer/TestBackBuffers.hpp"

#include <PixelBufferBase.hpp>

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

	bool TestBackBuffers::Bind( eBUFFER p_buffer, eFRAMEBUFFER_TARGET p_target )
	{
		return true;
	}

	void TestBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )
	{
	}

	void TestBackBuffers::SetReadBuffer( eATTACHMENT_POINT p_eAttach, uint8_t p_index )
	{
	}

	bool TestBackBuffers::DownloadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		return true;
	}

	bool TestBackBuffers::DoInitialise( Size const & p_size )
	{
		return true;
	}

	void TestBackBuffers::DoCleanup()
	{
	}

	void TestBackBuffers::DoClear( uint32_t p_uiTargets )
	{
	}

	void TestBackBuffers::DoResize( Castor::Size const & p_size )
	{
	}

	bool TestBackBuffers::DoBlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components )
	{
		return false;
	}

	bool TestBackBuffers::DoStretchInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, InterpolationMode p_interpolation )
	{
		return false;
	}
}

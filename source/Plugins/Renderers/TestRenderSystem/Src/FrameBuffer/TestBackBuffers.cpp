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

	void TestBackBuffers::Bind( WindowBuffer p_buffer, FrameBufferTarget p_target )const
	{
	}

	void TestBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )const
	{
	}

	void TestBackBuffers::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
	}

	void TestBackBuffers::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
	}

	void TestBackBuffers::DoClear( Castor3D::BufferComponents p_uiTargets )
	{
	}

	void TestBackBuffers::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
	}

	void TestBackBuffers::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
	}
}

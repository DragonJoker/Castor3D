#include "FrameBuffer/TestFrameBuffer.hpp"

#include "FrameBuffer/TestColourRenderBuffer.hpp"
#include "FrameBuffer/TestDepthStencilRenderBuffer.hpp"
#include "FrameBuffer/TestRenderBufferAttachment.hpp"
#include "FrameBuffer/TestTextureAttachment.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestFrameBuffer::TestFrameBuffer( Engine & p_engine )
		: FrameBuffer( p_engine )
	{
	}

	TestFrameBuffer::~TestFrameBuffer()
	{
	}

	bool TestFrameBuffer::Create()
	{
		return true;
	}

	void TestFrameBuffer::Destroy()
	{
	}

	void TestFrameBuffer::SetDrawBuffers( AttachArray const & p_attaches )const
	{
	}

	void TestFrameBuffer::SetReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
	}

	bool TestFrameBuffer::IsComplete()const
	{
		return true;
	}

	bool TestFrameBuffer::DownloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		return true;
	}

	ColourRenderBufferSPtr TestFrameBuffer::CreateColourRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< TestColourRenderBuffer >( p_format );
	}

	DepthStencilRenderBufferSPtr TestFrameBuffer::CreateDepthStencilRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< TestDepthStencilRenderBuffer >( p_format );
	}

	RenderBufferAttachmentSPtr TestFrameBuffer::CreateAttachment( RenderBufferSPtr p_renderBuffer )
	{
		return std::make_shared< TestRenderBufferAttachment >( p_renderBuffer );
	}

	TextureAttachmentSPtr TestFrameBuffer::CreateAttachment( TextureLayoutSPtr p_texture )
	{
		return std::make_shared< TestTextureAttachment >( p_texture );
	}

	void TestFrameBuffer::DoClear( uint32_t p_uiTargets )
	{
	}

	bool TestFrameBuffer::DoBind( FrameBufferTarget p_target )const
	{
		return true;
	}

	void TestFrameBuffer::DoUnbind()const
	{
	}

	bool TestFrameBuffer::DoBlitInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components )const
	{
		return true;
	}

	bool TestFrameBuffer::DoStretchInto( FrameBuffer const & p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, InterpolationMode p_interpolation )const
	{
		return true;
	}
}

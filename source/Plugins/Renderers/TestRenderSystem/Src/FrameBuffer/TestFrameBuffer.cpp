#include "FrameBuffer/TestFrameBuffer.hpp"

#include "FrameBuffer/TestColourRenderBuffer.hpp"
#include "FrameBuffer/TestCubeTextureFaceAttachment.hpp"
#include "FrameBuffer/TestDepthStencilRenderBuffer.hpp"
#include "FrameBuffer/TestRenderBufferAttachment.hpp"
#include "FrameBuffer/TestTextureAttachment.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestFrameBuffer::TestFrameBuffer( Engine & engine )
		: FrameBuffer( engine )
	{
	}

	TestFrameBuffer::~TestFrameBuffer()
	{
	}

	bool TestFrameBuffer::initialise()
	{
		return true;
	}

	void TestFrameBuffer::cleanup()
	{
	}

	void TestFrameBuffer::setDrawBuffers( AttachArray const & p_attaches )const
	{
	}

	void TestFrameBuffer::setReadBuffer( AttachmentPoint p_eAttach, uint8_t p_index )const
	{
	}

	bool TestFrameBuffer::isComplete()const
	{
		return true;
	}

	void TestFrameBuffer::downloadBuffer( AttachmentPoint p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
	}

	ColourRenderBufferSPtr TestFrameBuffer::createColourRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< TestColourRenderBuffer >( p_format );
	}

	DepthStencilRenderBufferSPtr TestFrameBuffer::createDepthStencilRenderBuffer( PixelFormat p_format )
	{
		return std::make_shared< TestDepthStencilRenderBuffer >( p_format );
	}

	RenderBufferAttachmentSPtr TestFrameBuffer::createAttachment( RenderBufferSPtr p_renderBuffer )
	{
		return std::make_shared< TestRenderBufferAttachment >( p_renderBuffer );
	}

	TextureAttachmentSPtr TestFrameBuffer::createAttachment( TextureLayoutSPtr p_texture )
	{
		return std::make_shared< TestTextureAttachment >( p_texture );
	}

	TextureAttachmentSPtr TestFrameBuffer::createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face )
	{
		return std::make_shared< TestCubeTextureFaceAttachment >( p_texture, p_face );
	}

	TextureAttachmentSPtr TestFrameBuffer::createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face, uint32_t p_mipLevel )
	{
		return std::make_shared< TestCubeTextureFaceAttachment >( p_texture, p_face, p_mipLevel );
	}

	void TestFrameBuffer::doClear( castor3d::BufferComponents p_uiTargets )const
	{
	}

	void TestFrameBuffer::doBind( FrameBufferTarget p_target )const
	{
	}

	void TestFrameBuffer::doUnbind()const
	{
	}

	void TestFrameBuffer::doBlitInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, FlagCombination< BufferComponent > const & p_components )const
	{
	}

	void TestFrameBuffer::doStretchInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
	}
}

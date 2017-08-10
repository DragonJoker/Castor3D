#include "FrameBuffer/TestTextureAttachment.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestTextureAttachment::TestTextureAttachment( TextureLayoutSPtr p_texture )
		: TextureAttachment( p_texture )
	{
	}

	TestTextureAttachment::~TestTextureAttachment()
	{
	}

	void TestTextureAttachment::doDownload( castor::Position const & p_offset
		, castor::PxBufferBase & p_buffer )const
	{
	}

	void TestTextureAttachment::doAttach()
	{
	}

	void TestTextureAttachment::doDetach()
	{
	}

	void TestTextureAttachment::doClear( Colour const & p_colour )const
	{
	}

	void TestTextureAttachment::doClear( float p_depth )const
	{
	}

	void TestTextureAttachment::doClear( int p_stencil )const
	{
	}

	void TestTextureAttachment::doClear( float p_depth, int p_stencil )const
	{
	}
}

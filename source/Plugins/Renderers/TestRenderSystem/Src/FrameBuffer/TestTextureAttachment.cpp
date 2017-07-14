#include "FrameBuffer/TestTextureAttachment.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestTextureAttachment::TestTextureAttachment( TextureLayoutSPtr p_texture )
		: TextureAttachment( p_texture )
	{
	}

	TestTextureAttachment::~TestTextureAttachment()
	{
	}

	void TestTextureAttachment::DoDownload( Castor::Position const & p_offset
		, Castor::PxBufferBase & p_buffer )const
	{
	}

	void TestTextureAttachment::DoAttach()
	{
	}

	void TestTextureAttachment::DoDetach()
	{
	}

	void TestTextureAttachment::DoClear( Colour const & p_colour )const
	{
	}

	void TestTextureAttachment::DoClear( float p_depth )const
	{
	}

	void TestTextureAttachment::DoClear( int p_stencil )const
	{
	}

	void TestTextureAttachment::DoClear( float p_depth, int p_stencil )const
	{
	}
}

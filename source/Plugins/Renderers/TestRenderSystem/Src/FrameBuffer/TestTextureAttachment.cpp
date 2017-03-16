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

	void TestTextureAttachment::DoAttach()
	{
	}

	void TestTextureAttachment::DoDetach()
	{
	}

	void TestTextureAttachment::DoClear( BufferComponent p_component )const
	{	
	}
}

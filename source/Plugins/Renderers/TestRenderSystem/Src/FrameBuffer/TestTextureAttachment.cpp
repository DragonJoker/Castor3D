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

	bool TestTextureAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		return true;
	}

	bool TestTextureAttachment::DoAttach()
	{
		return true;
	}

	void TestTextureAttachment::DoDetach()
	{
	}
}

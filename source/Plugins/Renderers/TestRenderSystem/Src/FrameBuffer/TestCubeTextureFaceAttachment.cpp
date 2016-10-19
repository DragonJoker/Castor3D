#include "FrameBuffer/TestCubeTextureFaceAttachment.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestCubeTextureFaceAttachment::TestCubeTextureFaceAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face )
		: TextureAttachment( p_texture )
		, m_face( p_face )
	{
	}

	TestCubeTextureFaceAttachment::~TestCubeTextureFaceAttachment()
	{
	}

	bool TestCubeTextureFaceAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		return true;
	}

	bool TestCubeTextureFaceAttachment::DoAttach()
	{
		return true;
	}

	void TestCubeTextureFaceAttachment::DoDetach()
	{
	}
}

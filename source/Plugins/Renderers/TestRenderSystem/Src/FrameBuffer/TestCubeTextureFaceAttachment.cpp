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

	void TestCubeTextureFaceAttachment::DoAttach()
	{
	}

	void TestCubeTextureFaceAttachment::DoDetach()
	{
	}
}

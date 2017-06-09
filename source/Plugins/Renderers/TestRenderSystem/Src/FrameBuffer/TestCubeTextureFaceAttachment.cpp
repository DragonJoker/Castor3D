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

	void TestCubeTextureFaceAttachment::DoClear( Colour const & p_colour )const
	{
	}

	void TestCubeTextureFaceAttachment::DoClear( float p_depth )const
	{
	}

	void TestCubeTextureFaceAttachment::DoClear( int p_stencil )const
	{
	}

	void TestCubeTextureFaceAttachment::DoClear( float p_depth, int p_stencil )const
	{
	}
}

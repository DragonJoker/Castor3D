#include "FrameBuffer/TestCubeTextureFaceAttachment.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestCubeTextureFaceAttachment::TestCubeTextureFaceAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face, uint32_t p_mipLevel )
		: TextureAttachment( p_texture )
		, m_face( p_face )
		, m_mipLevel( p_mipLevel )
	{
	}

	TestCubeTextureFaceAttachment::~TestCubeTextureFaceAttachment()
	{
	}

	void TestCubeTextureFaceAttachment::DoDownload( Castor::Position const & p_offset
		, Castor::PxBufferBase & p_buffer )const
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

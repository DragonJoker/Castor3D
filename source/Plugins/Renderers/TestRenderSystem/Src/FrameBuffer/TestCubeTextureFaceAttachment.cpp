#include "FrameBuffer/TestCubeTextureFaceAttachment.hpp"

using namespace castor3d;
using namespace castor;

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

	void TestCubeTextureFaceAttachment::doDownload( castor::Position const & p_offset
		, castor::PxBufferBase & p_buffer )const
	{
	}

	void TestCubeTextureFaceAttachment::doAttach()
	{
	}

	void TestCubeTextureFaceAttachment::doDetach()
	{
	}

	void TestCubeTextureFaceAttachment::doClear( Colour const & p_colour )const
	{
	}

	void TestCubeTextureFaceAttachment::doClear( float p_depth )const
	{
	}

	void TestCubeTextureFaceAttachment::doClear( int p_stencil )const
	{
	}

	void TestCubeTextureFaceAttachment::doClear( float p_depth, int p_stencil )const
	{
	}
}

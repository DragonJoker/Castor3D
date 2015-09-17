#include "TextureAttachment.hpp"
#include "FrameBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureAttachment::TextureAttachment( DynamicTextureSPtr p_pTexture )
		:	FrameBufferAttachment( )
		,	m_pTexture( p_pTexture )
	{
	}

	TextureAttachment::~TextureAttachment()
	{
	}

	bool TextureAttachment::Attach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer, eTEXTURE_TARGET p_eTarget, int p_iLayer )
	{
		m_eAttachedTarget = p_eTarget;
		m_iAttachedLayer = p_iLayer;
		bool l_return = FrameBufferAttachment::Attach( p_eAttachment, p_pFrameBuffer );

		if ( l_return )
		{
			p_pFrameBuffer->Attach( this );
		}

		return l_return;
	}

	void TextureAttachment::Detach()
	{
		FrameBufferSPtr l_pFrameBuffer = GetFrameBuffer();

		if ( l_pFrameBuffer )
		{
			l_pFrameBuffer->Detach( this );
			FrameBufferAttachment::Detach();
		}
	}
}

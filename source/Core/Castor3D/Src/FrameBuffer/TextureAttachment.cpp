#include "TextureAttachment.hpp"

#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	TextureAttachment::TextureAttachment( TextureLayoutSPtr p_texture )
		: FrameBufferAttachment( AttachmentType::eTexture )
		, m_texture( p_texture )
		, m_target( TextureType::eCount )
		, m_layer( 0 )
	{
	}

	TextureAttachment::~TextureAttachment()
	{
	}

	PxBufferBaseSPtr TextureAttachment::getBuffer()const
	{
		return getTexture()->getImage().getBuffer();
	}
}

#include "TextureAttachment.hpp"

#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	TextureAttachment::TextureAttachment( TextureLayoutSPtr texture
		, uint32_t mipLevel )
		: FrameBufferAttachment( AttachmentType::eTexture )
		, m_texture( texture )
		, m_target( TextureType::eCount )
		, m_layer( 0 )
		, m_mipLevel{ mipLevel }
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

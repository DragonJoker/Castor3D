#include "TextureAttachment.hpp"

#include "FrameBuffer.hpp"

#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

using namespace Castor;

namespace Castor3D
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

	PxBufferBaseSPtr TextureAttachment::GetBuffer()const
	{
		return GetTexture()->GetImage().GetBuffer();
	}
}

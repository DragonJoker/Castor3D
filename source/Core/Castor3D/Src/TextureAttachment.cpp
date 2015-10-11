#include "TextureAttachment.hpp"
#include "FrameBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureAttachment::TextureAttachment( DynamicTextureSPtr p_texture )
		: FrameBufferAttachment( eATTACHMENT_TYPE_TEXTURE )
		, m_texture( p_texture )
		, m_target( eTEXTURE_TARGET_COUNT )
		, m_layer( 0 )
	{
	}

	TextureAttachment::~TextureAttachment()
	{
	}

	bool TextureAttachment::Attach( eATTACHMENT_POINT p_attachment, uint8_t p_index, FrameBufferSPtr p_frameBuffer, eTEXTURE_TARGET p_target, int p_layer )
	{
		m_layer = p_layer;
		m_target = p_target;
		return FrameBufferAttachment::Attach( p_attachment, p_index, p_frameBuffer );
	}

	bool TextureAttachment::Attach( eATTACHMENT_POINT p_attachment, FrameBufferSPtr p_frameBuffer, eTEXTURE_TARGET p_target, int p_layer )
	{
		return Attach( p_attachment, 0, p_frameBuffer, p_target, p_layer );
	}

	void TextureAttachment::Detach()
	{
		FrameBufferAttachment::Detach();
		m_target = eTEXTURE_TARGET_COUNT;
		m_layer = 0;
	}
}

#include "TextureLayout.hpp"

#include "Engine.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		size_t GetImagesCount( eTEXTURE_TYPE p_type )
		{
			size_t l_return = 1;

			if ( p_type == eTEXTURE_TYPE_CUBE )
			{
				l_return = 6;
			}

			return l_return;
		}
	}

	TextureLayout::TextureLayout( eTEXTURE_TYPE p_type, RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_initialised{ false }
		, m_type{ p_type }
		, m_images{ GetImagesCount( p_type ), nullptr }
	{
	}

	TextureLayout::~TextureLayout()
	{
	}

	bool TextureLayout::Bind( uint32_t p_index )const
	{
		bool l_return = false;

		if ( m_initialised )
		{
			l_return = DoBind( p_index );
		}

		return l_return;
	}

	void TextureLayout::Unbind( uint32_t p_index )const
	{
		DoUnbind( p_index );
	}
}

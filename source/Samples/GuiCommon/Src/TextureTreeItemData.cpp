#include "TextureTreeItemData.hpp"

namespace GuiCommon
{
	wxTextureTreeItemData::wxTextureTreeItemData( Castor3D::TextureUnitSPtr p_texture )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_TEXTURE )
		, m_texture( p_texture )
	{
	}

	wxTextureTreeItemData::~wxTextureTreeItemData()
	{
	}
}

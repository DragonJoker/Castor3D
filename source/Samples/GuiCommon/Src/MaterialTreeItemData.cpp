#include "MaterialTreeItemData.hpp"

namespace GuiCommon
{
	wxMaterialTreeItemData::wxMaterialTreeItemData( Castor3D::MaterialSPtr p_material )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_MATERIAL )
		, m_material( p_material )
	{
	}

	wxMaterialTreeItemData::~wxMaterialTreeItemData()
	{
	}
}

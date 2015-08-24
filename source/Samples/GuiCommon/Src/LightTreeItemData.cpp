#include "LightTreeItemData.hpp"

namespace GuiCommon
{
	wxLightTreeItemData::wxLightTreeItemData( Castor3D::LightSPtr p_light )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_LIGHT )
		, m_light( p_light )
	{
	}

	wxLightTreeItemData::~wxLightTreeItemData()
	{
	}
}

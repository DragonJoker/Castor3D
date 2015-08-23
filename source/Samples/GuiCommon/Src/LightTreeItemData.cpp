#include "LightTreeItemData.hpp"

namespace GuiCommon
{
	wxLightTreeItemData::wxLightTreeItemData( Castor3D::LightSPtr p_light )
		: wxTreeItemData()
		, m_light( p_light )
	{
	}

	wxLightTreeItemData::~wxLightTreeItemData()
	{
	}
}

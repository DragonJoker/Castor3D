#include "OverlayTreeItemData.hpp"

namespace GuiCommon
{
	wxOverlayTreeItemData::wxOverlayTreeItemData( Castor3D::OverlaySPtr p_overlay )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_OVERLAY )
		, m_overlay( p_overlay )
	{
	}

	wxOverlayTreeItemData::~wxOverlayTreeItemData()
	{
	}
}

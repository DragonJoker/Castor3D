#include "OverlayTreeItemData.hpp"

namespace GuiCommon
{
	wxOverlayTreeItemData::wxOverlayTreeItemData( Castor3D::OverlaySPtr p_overlay )
		: wxTreeItemData()
		, m_overlay( p_overlay )
	{
	}

	wxOverlayTreeItemData::~wxOverlayTreeItemData()
	{
	}
}

#include "TreeItemPropertyData.hpp"

namespace GuiCommon
{
	wxTreeItemPropertyData::wxTreeItemPropertyData( ePROPERTY_DATA_TYPE p_type )
		: wxTreeItemData()
		, m_type( p_type )
	{
	}

	wxTreeItemPropertyData::~wxTreeItemPropertyData()
	{
	}
}

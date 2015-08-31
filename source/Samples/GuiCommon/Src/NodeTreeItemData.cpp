#include "NodeTreeItemData.hpp"

namespace GuiCommon
{
	wxNodeTreeItemData::wxNodeTreeItemData( Castor3D::SceneNodeSPtr p_node )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_NODE )
		, m_node( p_node )
	{
	}

	wxNodeTreeItemData::~wxNodeTreeItemData()
	{
	}
}

#include "NodeTreeItemData.hpp"

namespace GuiCommon
{
	wxNodeTreeItemData::wxNodeTreeItemData( Castor3D::SceneNodeSPtr p_node )
		: wxTreeItemData()
		, m_node( p_node )
	{
	}

	wxNodeTreeItemData::~wxNodeTreeItemData()
	{
	}
}

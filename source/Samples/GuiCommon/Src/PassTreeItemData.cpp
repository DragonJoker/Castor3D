#include "PassTreeItemData.hpp"

namespace GuiCommon
{
	wxPassTreeItemData::wxPassTreeItemData( Castor3D::PassSPtr p_pass )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_PASS )
		, m_pass( p_pass )
	{
	}

	wxPassTreeItemData::~wxPassTreeItemData()
	{
	}
}

#include "SubmeshTreeItemData.hpp"

namespace GuiCommon
{
	wxSubmeshTreeItemData::wxSubmeshTreeItemData( Castor3D::GeometrySPtr p_pGeometry, Castor3D::SubmeshSPtr p_pSubmesh )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_SUBMESH )
		, m_pGeometry( p_pGeometry )
		, m_pSubmesh( p_pSubmesh )
	{
	}

	wxSubmeshTreeItemData::~wxSubmeshTreeItemData()
	{
	}
}

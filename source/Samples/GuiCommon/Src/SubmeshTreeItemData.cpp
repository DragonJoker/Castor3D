#include "SubmeshTreeItemData.hpp"

namespace GuiCommon
{
	wxSubmeshTreeItemData::wxSubmeshTreeItemData( Castor3D::GeometrySPtr p_pGeometry, Castor3D::SubmeshSPtr p_pSubmesh )
		:	wxTreeItemData()
		,	m_pGeometry( p_pGeometry )
		,	m_pSubmesh( p_pSubmesh )
	{
	}

	wxSubmeshTreeItemData::~wxSubmeshTreeItemData()
	{
	}
}

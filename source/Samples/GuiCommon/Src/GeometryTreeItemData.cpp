#include "GeometryTreeItemData.hpp"

namespace GuiCommon
{
	wxGeometryTreeItemData::wxGeometryTreeItemData( Castor3D::GeometrySPtr p_pGeometry )
		:	wxTreeItemData()
		,	m_pGeometry( p_pGeometry )
	{
	}

	wxGeometryTreeItemData::~wxGeometryTreeItemData()
	{
	}
}

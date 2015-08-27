#include "GeometryTreeItemData.hpp"

namespace GuiCommon
{
	wxGeometryTreeItemData::wxGeometryTreeItemData( Castor3D::GeometrySPtr p_pGeometry )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_GEOMETRY )
		, m_pGeometry( p_pGeometry )
	{
	}

	wxGeometryTreeItemData::~wxGeometryTreeItemData()
	{
	}
}

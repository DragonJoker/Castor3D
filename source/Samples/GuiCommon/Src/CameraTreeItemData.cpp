#include "CameraTreeItemData.hpp"

namespace GuiCommon
{
	wxCameraTreeItemData::wxCameraTreeItemData( Castor3D::CameraSPtr p_camera )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_CAMERA )
		, m_camera( p_camera )
	{
	}

	wxCameraTreeItemData::~wxCameraTreeItemData()
	{
	}
}

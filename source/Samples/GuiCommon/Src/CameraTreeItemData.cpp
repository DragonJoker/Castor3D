#include "CameraTreeItemData.hpp"

namespace GuiCommon
{
	wxCameraTreeItemData::wxCameraTreeItemData( Castor3D::CameraSPtr p_camera )
		: wxTreeItemData()
		, m_camera( p_camera )
	{
	}

	wxCameraTreeItemData::~wxCameraTreeItemData()
	{
	}
}

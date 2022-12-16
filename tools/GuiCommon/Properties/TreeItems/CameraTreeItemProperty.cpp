#include "GuiCommon/Properties/TreeItems/CameraTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	CameraTreeItemProperty::CameraTreeItemProperty( bool editable, castor3d::Camera & camera )
		: TreeItemProperty( camera.getScene()->getEngine(), editable )
		, m_camera( camera )
	{
		CreateTreeItemMenu();
	}

	void CameraTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_CAMERA = _( "Camera: " );

		addProperty( grid, PROPERTY_CATEGORY_CAMERA + wxString( m_camera.getName() ) );
	}
}

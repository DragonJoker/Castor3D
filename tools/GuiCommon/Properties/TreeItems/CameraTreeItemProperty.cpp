#include "GuiCommon/Properties/TreeItems/CameraTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	CameraTreeItemProperty::CameraTreeItemProperty( bool p_editable, castor3d::Camera & p_camera )
		: TreeItemProperty( p_camera.getScene()->getEngine(), p_editable )
		, m_camera( p_camera )
	{
		CreateTreeItemMenu();
	}

	CameraTreeItemProperty::~CameraTreeItemProperty()
	{
	}

	void CameraTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_CAMERA = _( "Camera: " );

		addProperty( grid, PROPERTY_CATEGORY_CAMERA + wxString( m_camera.getName() ) );
	}
}

#include "GuiCommon/Properties/TreeItems/CameraTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_CAMERA = _( "Camera: " );
		static wxString PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );
	}

	CameraTreeItemProperty::CameraTreeItemProperty( bool p_editable, castor3d::Camera & p_camera )
		: TreeItemProperty( p_camera.getScene()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_camera( p_camera )
	{
		PROPERTY_CATEGORY_CAMERA = _( "Camera: " );
		PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );

		CreateTreeItemMenu();
	}

	CameraTreeItemProperty::~CameraTreeItemProperty()
	{
	}

	void CameraTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_CAMERA + wxString( m_camera.getName() ) ) );
	}

	void CameraTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
	}
}

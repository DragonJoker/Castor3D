#include "CameraTreeItemProperty.hpp"

#include <Scene/Camera.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_CAMERA = _( "Camera: " );
		static wxString PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );
	}

	CameraTreeItemProperty::CameraTreeItemProperty( bool p_editable, Castor3D::CameraSPtr p_camera )
		: TreeItemProperty( p_camera->GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_camera( p_camera )
	{
		PROPERTY_CATEGORY_CAMERA = _( "Camera: " );
		PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );

		CreateTreeItemMenu();
	}

	CameraTreeItemProperty::~CameraTreeItemProperty()
	{
	}

	void CameraTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		CameraSPtr l_camera = GetCamera();

		if ( l_camera )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_CAMERA + wxString( l_camera->GetName() ) ) );
		}
	}

	void CameraTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
	}
}

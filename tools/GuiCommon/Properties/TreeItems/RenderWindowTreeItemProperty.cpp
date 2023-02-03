#include "GuiCommon/Properties/TreeItems/RenderWindowTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Render/RenderWindow.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	RenderWindowTreeItemProperty::RenderWindowTreeItemProperty( bool editable
		, castor3d::RenderWindow & window )
		: TreeItemProperty( window.getEngine(), editable )
		, m_window( window )
	{
		CreateTreeItemMenu();
	}

	void RenderWindowTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );

		addProperty( grid, PROPERTY_CATEGORY_RENDER_WINDOW + wxString( m_window.getName() ) );
	}
}

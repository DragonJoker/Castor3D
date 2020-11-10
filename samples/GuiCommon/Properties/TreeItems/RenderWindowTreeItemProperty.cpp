#include "GuiCommon/Properties/TreeItems/RenderWindowTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Render/RenderWindow.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	RenderWindowTreeItemProperty::RenderWindowTreeItemProperty( bool editable
		, RenderWindow & window )
		: TreeItemProperty( window.getEngine(), editable, ePROPERTY_DATA_TYPE_RENDER_WINDOW )
		, m_window( window )
	{
		CreateTreeItemMenu();
	}

	RenderWindowTreeItemProperty::~RenderWindowTreeItemProperty()
	{
	}

	void RenderWindowTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );
#if C3D_DebugQuads
		static wxString PROPERTY_RENDER_WINDOW_VIEW_PICKING = _( "View Picking" );
#endif

		addProperty( grid, PROPERTY_CATEGORY_RENDER_WINDOW + wxString( m_window.getName() ) );
#if C3D_DebugQuads
		addPropertyT( grid, PROPERTY_RENDER_WINDOW_VIEW_PICKING, m_window.isPickingDebugEnabled(), &m_window, &RenderWindow::enablePickingDebug );
#endif
	}
}

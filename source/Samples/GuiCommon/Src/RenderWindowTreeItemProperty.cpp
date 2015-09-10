#include "RenderWindowTreeItemProperty.hpp"

#include <RenderWindow.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );
	}

	RenderWindowTreeItemProperty::RenderWindowTreeItemProperty( bool p_editable, RenderWindowSPtr p_window )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_RENDER_WINDOW )
		, m_window( p_window )
	{
	}

	RenderWindowTreeItemProperty::~RenderWindowTreeItemProperty()
	{
	}

	void RenderWindowTreeItemProperty::CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		RenderWindowSPtr l_window = GetRenderWindow();

		if ( l_window )
		{
		}
	}

	void RenderWindowTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		RenderWindowSPtr l_window = GetRenderWindow();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_window )
		{
		}
	}
}

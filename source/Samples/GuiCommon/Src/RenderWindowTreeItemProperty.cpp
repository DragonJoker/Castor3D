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

	wxRenderWindowTreeItemProperty::wxRenderWindowTreeItemProperty( RenderWindowSPtr p_window )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_RENDER_WINDOW )
		, m_window( p_window )
	{
	}

	wxRenderWindowTreeItemProperty::~wxRenderWindowTreeItemProperty()
	{
	}

	void wxRenderWindowTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		RenderWindowSPtr l_window = GetRenderWindow();

		if ( l_window )
		{
		}
	}

	void wxRenderWindowTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		RenderWindowSPtr l_window = GetRenderWindow();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_window )
		{
		}
	}
}

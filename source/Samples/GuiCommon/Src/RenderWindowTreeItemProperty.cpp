#include "RenderWindowTreeItemProperty.hpp"

#include <Render/RenderWindow.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );
	}

	RenderWindowTreeItemProperty::RenderWindowTreeItemProperty( bool p_editable, RenderWindowSPtr p_window )
		: TreeItemProperty( p_window->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_RENDER_WINDOW )
		, m_window( p_window )
	{
		PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );

		CreateTreeItemMenu();
	}

	RenderWindowTreeItemProperty::~RenderWindowTreeItemProperty()
	{
	}

	void RenderWindowTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		RenderWindowSPtr l_window = GetRenderWindow();

		if ( l_window )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_WINDOW + wxString( l_window->GetName() ) ) );
		}
	}

	void RenderWindowTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		RenderWindowSPtr l_window = GetRenderWindow();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_window )
		{
		}
	}
}

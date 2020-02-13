#include "GuiCommon/Properties/TreeItems/RenderWindowTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Render/RenderWindow.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );
	}

	RenderWindowTreeItemProperty::RenderWindowTreeItemProperty( bool p_editable, RenderWindowSPtr p_window )
		: TreeItemProperty( p_window->getEngine(), p_editable, ePROPERTY_DATA_TYPE_RENDER_WINDOW )
		, m_window( p_window )
	{
		PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );

		CreateTreeItemMenu();
	}

	RenderWindowTreeItemProperty::~RenderWindowTreeItemProperty()
	{
	}

	void RenderWindowTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		RenderWindowSPtr window = getRenderWindow();

		if ( window )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_RENDER_WINDOW + wxString( window->getName() ) ) );
		}
	}

	void RenderWindowTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		RenderWindowSPtr window = getRenderWindow();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && window )
		{
		}
	}
}

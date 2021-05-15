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
		: TreeItemProperty( window.getEngine(), editable )
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

		addProperty( grid, PROPERTY_CATEGORY_RENDER_WINDOW + wxString( m_window.getName() ) );

#if C3D_DebugQuads
		static wxString PROPERTY_RENDER_WINDOW_DEBUG_VIEW = _( "Debug View" );

		auto intermediates = m_window.listIntermediateViews();
		auto & debugConfig = m_window.getDebugConfig();
		wxArrayString debugChoices;

		for ( auto & intermediate : intermediates )
		{
			debugChoices.Add( make_wxString( intermediate.name ) );
		}

		addPropertyET( grid, PROPERTY_RENDER_WINDOW_DEBUG_VIEW, debugChoices, &debugConfig.debugIndex );
#endif
	}
}

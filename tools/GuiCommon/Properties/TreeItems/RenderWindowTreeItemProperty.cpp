#include "GuiCommon/Properties/TreeItems/RenderWindowTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
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

	void RenderWindowTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_RENDER_WINDOW = _( "Render Window: " );

		addProperty( grid, PROPERTY_CATEGORY_RENDER_WINDOW + wxString( m_window.getName() ) );

		doCreateDebugProperties( grid );
	}

	void RenderWindowTreeItemProperty::doCreateDebugProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_DEBUG = _( "Debug" );
		static wxString PROPERTY_SCENE_DEBUG_OVERLAYS = _( "Debug overlays" );

		addProperty( grid
			, PROPERTY_CATEGORY_DEBUG );
		addPropertyT( grid
			, PROPERTY_SCENE_DEBUG_OVERLAYS
			, m_window.getEngine()->getRenderLoop().hasDebugOverlays()
			, &m_window.getEngine()->getRenderLoop()
			, &castor3d::RenderLoop::showDebugOverlays );
	}
}

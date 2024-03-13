#include "GuiCommon/Properties/TreeItems/ViewportTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/SizeProperties.hpp"

#include <Castor3D/Render/Viewport.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	ViewportTreeItemProperty::ViewportTreeItemProperty( bool editable
		, castor3d::Engine & engine
		, castor3d::Viewport & viewport )
		: TreeItemProperty( &engine, editable )
		, m_viewport( viewport )
	{
		CreateTreeItemMenu();
	}

	void ViewportTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );
		static wxString PROPERTY_VIEWPORT_TYPE = _( "Type" );
		static wxString PROPERTY_VIEWPORT_TYPE_PERSPECTIVE = _( "Perspective" );
		static wxString PROPERTY_VIEWPORT_TYPE_FRUSTUM = _( "Frustum" );
		static wxString PROPERTY_VIEWPORT_TYPE_ORTHO = _( "Ortho" );
		static wxString PROPERTY_VIEWPORT_SIZE = _( "Size" );
		static wxString PROPERTY_VIEWPORT_TOP = _( "Top" );
		static wxString PROPERTY_VIEWPORT_BOTTOM = _( "Bottom" );
		static wxString PROPERTY_VIEWPORT_LEFT = _( "Left" );
		static wxString PROPERTY_VIEWPORT_RIGHT = _( "Right" );
		static wxString PROPERTY_VIEWPORT_NEAR = _( "Near" );
		static wxString PROPERTY_VIEWPORT_FAR = _( "Far" );
		static wxString PROPERTY_VIEWPORT_FOVY = _( "FOV Y" );
		static wxString PROPERTY_VIEWPORT_RATIO = _( "Ratio" );

		wxArrayString choices;
		choices.Add( PROPERTY_VIEWPORT_TYPE_PERSPECTIVE );
		choices.Add( PROPERTY_VIEWPORT_TYPE_FRUSTUM );
		choices.Add( PROPERTY_VIEWPORT_TYPE_ORTHO );

		addProperty( grid, PROPERTY_CATEGORY_VIEWPORT );
		addPropertyET( grid, PROPERTY_VIEWPORT_TYPE, choices, m_viewport.getType(), &m_viewport, &castor3d::Viewport::updateType );
		addPropertyT( grid, PROPERTY_VIEWPORT_SIZE, m_viewport.getSize(), &m_viewport, &castor3d::Viewport::resize );
		addPropertyT( grid, PROPERTY_VIEWPORT_TOP, m_viewport.getTop(), &m_viewport, &castor3d::Viewport::updateTop );
		addPropertyT( grid, PROPERTY_VIEWPORT_BOTTOM, m_viewport.getBottom(), &m_viewport, &castor3d::Viewport::updateBottom );
		addPropertyT( grid, PROPERTY_VIEWPORT_LEFT, m_viewport.getLeft(), &m_viewport, &castor3d::Viewport::updateLeft );
		addPropertyT( grid, PROPERTY_VIEWPORT_RIGHT, m_viewport.getRight(), &m_viewport, &castor3d::Viewport::updateRight );
		addPropertyT( grid, PROPERTY_VIEWPORT_NEAR, m_viewport.getNear(), &m_viewport, &castor3d::Viewport::updateNear );
		addPropertyT( grid, PROPERTY_VIEWPORT_FAR, m_viewport.getFar(), &m_viewport, &castor3d::Viewport::updateFar );
		addPropertyT( grid, PROPERTY_VIEWPORT_FOVY, m_viewport.getFovY(), &m_viewport, &castor3d::Viewport::updateFovY );
		addPropertyT( grid, PROPERTY_VIEWPORT_RATIO, m_viewport.getRatio(), &m_viewport, &castor3d::Viewport::updateRatio );
	}
}

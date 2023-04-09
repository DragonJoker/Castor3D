#include "GuiCommon/Properties/TreeItems/AnimatedObjectTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	AnimatedObjectTreeItemProperty::AnimatedObjectTreeItemProperty( castor3d::Engine * engine
		, bool editable
		, castor3d::AnimatedObjectRPtr object )
		: TreeItemProperty( engine, editable )
		, m_object( object )
	{
		CreateTreeItemMenu();
	}

	void AnimatedObjectTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_ANIMATED_OBJECT = _( "Animated Object: " );
		auto object = getObject();

		if ( object )
		{
			addProperty( grid, PROPERTY_CATEGORY_ANIMATED_OBJECT + wxString( object->getName() ) );
		}
	}
}

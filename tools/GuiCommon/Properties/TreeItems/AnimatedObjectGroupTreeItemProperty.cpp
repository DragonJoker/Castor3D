#include "GuiCommon/Properties/TreeItems/AnimatedObjectGroupTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );
	}

	AnimatedObjectGroupTreeItemProperty::AnimatedObjectGroupTreeItemProperty( bool editable
		, castor3d::AnimatedObjectGroup & group )
		: TreeItemProperty( group.getScene()->getEngine(), editable )
		, m_group( group )
	{
		PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );

		CreateTreeItemMenu();
	}

	void AnimatedObjectGroupTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		auto group = getGroup();

		if ( group )
		{
			addProperty( grid, PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP + wxString( group->getName() ) );
		}
	}
}

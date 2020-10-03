#include "GuiCommon/Properties/TreeItems/AnimatedObjectGroupTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );
	}

	AnimatedObjectGroupTreeItemProperty::AnimatedObjectGroupTreeItemProperty( bool editable
		, AnimatedObjectGroup & group )
		: TreeItemProperty( group.getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_ANIMATED_OBJECT_GROUP )
		, m_group( group )
	{
		PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );

		CreateTreeItemMenu();
	}

	AnimatedObjectGroupTreeItemProperty::~AnimatedObjectGroupTreeItemProperty()
	{
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

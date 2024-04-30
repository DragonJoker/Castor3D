#include "GuiCommon/Properties/TreeItems/AnimatedObjectGroupTreeItemProperty.hpp"

#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

namespace GuiCommon
{
	AnimatedObjectGroupTreeItemProperty::AnimatedObjectGroupTreeItemProperty( bool editable
		, castor3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void AnimatedObjectGroupTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );

		if ( m_group )
		{
			addProperty( grid, PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP + wxString( m_group->getName() ) );
		}
	}
}

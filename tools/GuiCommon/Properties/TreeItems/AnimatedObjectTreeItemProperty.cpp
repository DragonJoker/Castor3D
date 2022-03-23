#include "GuiCommon/Properties/TreeItems/AnimatedObjectTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATED_OBJECT = _( "Animated Object: " );
	}

	AnimatedObjectTreeItemProperty::AnimatedObjectTreeItemProperty( castor3d::Engine * engine, bool p_editable, castor3d::AnimatedObjectSPtr p_object )
		: TreeItemProperty( engine, p_editable )
		, m_object( p_object )
	{
		PROPERTY_CATEGORY_ANIMATED_OBJECT = _( "Animated Object: " );

		CreateTreeItemMenu();
	}

	void AnimatedObjectTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		auto object = getObject();

		if ( object )
		{
			addProperty( grid, PROPERTY_CATEGORY_ANIMATED_OBJECT + wxString( object->getName() ) );
		}
	}
}

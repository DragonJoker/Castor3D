#include "GuiCommon/Properties/TreeItems/AnimatedObjectTreeItemProperty.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATED_OBJECT = _( "Animated Object: " );
	}

	AnimatedObjectTreeItemProperty::AnimatedObjectTreeItemProperty( Engine * engine, bool p_editable, AnimatedObjectSPtr p_object )
		: TreeItemProperty( engine, p_editable )
		, m_object( p_object )
	{
		PROPERTY_CATEGORY_ANIMATED_OBJECT = _( "Animated Object: " );

		CreateTreeItemMenu();
	}

	AnimatedObjectTreeItemProperty::~AnimatedObjectTreeItemProperty()
	{
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

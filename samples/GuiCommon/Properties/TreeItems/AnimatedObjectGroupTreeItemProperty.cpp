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

	AnimatedObjectGroupTreeItemProperty::AnimatedObjectGroupTreeItemProperty( bool p_editable, AnimatedObjectGroupSPtr p_group )
		: TreeItemProperty( p_group->getScene()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_ANIMATED_OBJECT_GROUP )
		, m_group( p_group )
	{
		PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );

		CreateTreeItemMenu();
	}

	AnimatedObjectGroupTreeItemProperty::~AnimatedObjectGroupTreeItemProperty()
	{
	}

	void AnimatedObjectGroupTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		auto group = getGroup();

		if ( group )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP + wxString( group->getName() ) ) );
		}
	}

	void AnimatedObjectGroupTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
	}
}

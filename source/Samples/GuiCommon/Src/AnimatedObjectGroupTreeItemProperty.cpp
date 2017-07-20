#include "AnimatedObjectGroupTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );
	}

	AnimatedObjectGroupTreeItemProperty::AnimatedObjectGroupTreeItemProperty( bool p_editable, AnimatedObjectGroupSPtr p_group )
		: TreeItemProperty( p_group->GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_ANIMATED_OBJECT_GROUP )
		, m_group( p_group )
	{
		PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP = _( "Animated Objects Group: " );

		CreateTreeItemMenu();
	}

	AnimatedObjectGroupTreeItemProperty::~AnimatedObjectGroupTreeItemProperty()
	{
	}

	void AnimatedObjectGroupTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		auto group = GetGroup();

		if ( group )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP + wxString( group->GetName() ) ) );
		}
	}

	void AnimatedObjectGroupTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		auto group = GetGroup();
		wxPGProperty * property = p_event.GetProperty();

		if ( group )
		{
		}
	}
}

#include "AnimatedObjectGroupTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Animation/AnimatedObjectGroup.hpp>

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
		auto l_group = GetGroup();

		if ( l_group )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_ANIMATED_OBJECT_GROUP + wxString( l_group->GetName() ) ) );
		}
	}

	void AnimatedObjectGroupTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		auto l_group = GetGroup();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_group )
		{
		}
	}
}

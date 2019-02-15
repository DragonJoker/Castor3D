#include "AnimatedObjectTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Scene/Animation/AnimatedObject.hpp>

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
		: TreeItemProperty( engine, p_editable, ePROPERTY_DATA_TYPE_ANIMATED_OBJECT )
		, m_object( p_object )
	{
		PROPERTY_CATEGORY_ANIMATED_OBJECT = _( "Animated Object: " );

		CreateTreeItemMenu();
	}

	AnimatedObjectTreeItemProperty::~AnimatedObjectTreeItemProperty()
	{
	}

	void AnimatedObjectTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		auto object = getObject();

		if ( object )
		{
		}
	}

	void AnimatedObjectTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		auto object = getObject();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && object )
		{
		}
	}
}

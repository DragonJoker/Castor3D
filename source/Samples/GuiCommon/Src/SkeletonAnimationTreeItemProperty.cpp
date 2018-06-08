#include "SkeletonAnimationTreeItemProperty.hpp"

#include <Animation/Animable.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Scene/Scene.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_SKELETON_ANIMATION = _( "Skeleton Animation: " );
	}

	SkeletonAnimationTreeItemProperty::SkeletonAnimationTreeItemProperty( bool editable, SkeletonAnimation & animation )
		: TreeItemProperty( animation.getOwner()->getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_SKELETON_ANIMATION )
		, m_animation( animation )
	{
		PROPERTY_CATEGORY_SKELETON_ANIMATION = _( "Skeleton Animation: " );

		CreateTreeItemMenu();
	}

	SkeletonAnimationTreeItemProperty::~SkeletonAnimationTreeItemProperty()
	{
	}

	void SkeletonAnimationTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SKELETON_ANIMATION + wxString( m_animation.getName() ) ) );
	}

	void SkeletonAnimationTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
		}
	}
}

#include "GuiCommon/Properties/TreeItems/SkeletonAnimationTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Animation/Animable.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	SkeletonAnimationTreeItemProperty::SkeletonAnimationTreeItemProperty( bool editable, SkeletonAnimation & animation )
		: TreeItemProperty( animation.getOwner()->getEngine(), editable, ePROPERTY_DATA_TYPE_SKELETON_ANIMATION )
		, m_animation( animation )
	{
		CreateTreeItemMenu();
	}

	SkeletonAnimationTreeItemProperty::~SkeletonAnimationTreeItemProperty()
	{
	}

	void SkeletonAnimationTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SKELETON_ANIMATION = _( "Skeleton Animation: " );

		addProperty( grid, PROPERTY_CATEGORY_SKELETON_ANIMATION + wxString( m_animation.getName() ) );
	}
}

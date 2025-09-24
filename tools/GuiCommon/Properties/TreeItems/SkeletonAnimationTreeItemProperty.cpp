#include "GuiCommon/Properties/TreeItems/SkeletonAnimationTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Animation/Animable.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	SkeletonAnimationTreeItemProperty::SkeletonAnimationTreeItemProperty( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine )
		: TreeItemProperty{ engine, imagesLoader, editable }
	{
		CreateTreeItemMenu();
	}

	void SkeletonAnimationTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SKELETON_ANIMATION = _( "Skeleton Animation: " );

		if ( !m_animation )
		{
			return;
		}

		addProperty( grid, PROPERTY_CATEGORY_SKELETON_ANIMATION + wxString( m_animation->getName() ) );
	}
}

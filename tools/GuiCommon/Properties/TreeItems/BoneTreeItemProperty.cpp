#include "GuiCommon/Properties/TreeItems/BoneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	BoneTreeItemProperty::BoneTreeItemProperty( bool editable, castor3d::BoneNode & bone )
		: TreeItemProperty( bone.getSkeleton().getScene()->getEngine(), editable )
		, m_bone( bone )
	{
		CreateTreeItemMenu();
	}

	void BoneTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BONE = _( "Bone: " );

		addProperty( grid, PROPERTY_CATEGORY_BONE + wxString( m_bone.getName() ) );
	}
}

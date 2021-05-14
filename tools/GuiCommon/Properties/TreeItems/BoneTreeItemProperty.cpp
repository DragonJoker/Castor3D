#include "GuiCommon/Properties/TreeItems/BoneTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Model/Skeleton/Bone.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	BoneTreeItemProperty::BoneTreeItemProperty( bool editable, Bone & bone )
		: TreeItemProperty( bone.getSkeleton().getScene()->getEngine(), editable )
		, m_bone( bone )
	{
		CreateTreeItemMenu();
	}

	BoneTreeItemProperty::~BoneTreeItemProperty()
	{
	}

	void BoneTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BONE = _( "Bone: " );

		addProperty( grid, PROPERTY_CATEGORY_BONE + wxString( m_bone.getName() ) );
	}
}

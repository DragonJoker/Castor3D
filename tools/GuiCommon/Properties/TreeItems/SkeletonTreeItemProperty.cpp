#include "GuiCommon/Properties/TreeItems/SkeletonTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	SkeletonTreeItemProperty::SkeletonTreeItemProperty( bool editable, Skeleton & skeleton )
		: TreeItemProperty( skeleton.getScene()->getEngine(), editable )
	{
		CreateTreeItemMenu();
	}

	void SkeletonTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SKELETON = _( "Skeleton: " );

		addProperty( grid, PROPERTY_CATEGORY_SKELETON );
	}
}

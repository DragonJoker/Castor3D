#include "GuiCommon/Properties/TreeItems/SkeletonNodeTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Model/Skeleton/SkeletonNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	SkeletonNodeTreeItemProperty::SkeletonNodeTreeItemProperty( bool editable, castor3d::SkeletonNode & node )
		: TreeItemProperty( node.getSkeleton().getScene()->getEngine(), editable )
		, m_node( node )
	{
		CreateTreeItemMenu();
	}

	void SkeletonNodeTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_BONE = _( "Node: " );

		addProperty( grid, PROPERTY_CATEGORY_BONE + wxString( m_node.getName() ) );
	}
}

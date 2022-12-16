#include "GuiCommon/Properties/TreeItems/NodeTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/Math/QuaternionProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	NodeTreeItemProperty::NodeTreeItemProperty( bool editable
		, castor3d::Engine * engine
		, castor3d::SceneNode & node )
		: TreeItemProperty( node.getScene()->getEngine(), editable )
		, m_node( node )
	{
		CreateTreeItemMenu();
	}

	void NodeTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_NODE = _( "Scene Node: " );
		static wxString PROPERTY_NODE_POSITION = _( "Position" );
		static wxString PROPERTY_NODE_SCALE = _( "Scale" );
		static wxString PROPERTY_NODE_ORIENTATION = _( "Orientation" );
		static wxString PROPERTY_NODE_VISIBLE = _( "Visible" );

		castor3d::SceneNode & node = getNode();
		addProperty( grid, PROPERTY_CATEGORY_NODE + wxString( node.getName() ) );
		addPropertyT( grid, PROPERTY_NODE_VISIBLE, node.isVisible(), &node, &castor3d::SceneNode::setVisible );
		addPropertyT( grid, PROPERTY_NODE_POSITION, node.getPosition(), &node, &castor3d::SceneNode::setPosition );
		addPropertyT( grid, PROPERTY_NODE_SCALE, node.getScale(), &node, &castor3d::SceneNode::setScale );
		addPropertyT( grid, PROPERTY_NODE_ORIENTATION, node.getOrientation(), &node, &castor3d::SceneNode::setOrientation );
	}
}

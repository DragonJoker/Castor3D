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

	void NodeTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_NODE_VISIBLE = _( "Visible" );

		wxString PROPERTY_CATEGORY_NODE = _( "Scene Node: " );

		if ( m_node.isStatic() )
		{
			PROPERTY_CATEGORY_NODE = _( "Static Scene Node: " );
		}

		addProperty( grid, PROPERTY_CATEGORY_NODE + wxString( m_node.getName() ) );
		addPropertyT( grid, PROPERTY_NODE_VISIBLE, m_node.isVisible(), &m_node, &castor3d::SceneNode::setVisible );

		if ( !m_node.isStatic() )
		{
			static wxString PROPERTY_NODE_POSITION = _( "Position" );
			static wxString PROPERTY_NODE_SCALE = _( "Scale" );
			static wxString PROPERTY_NODE_ORIENTATION = _( "Orientation" );
			addPropertyT( grid, PROPERTY_NODE_POSITION, m_node.getPosition(), &m_node, &castor3d::SceneNode::setPosition );
			addPropertyT( grid, PROPERTY_NODE_SCALE, m_node.getScale(), &m_node, &castor3d::SceneNode::setScale );
			addPropertyT( grid, PROPERTY_NODE_ORIENTATION, m_node.getOrientation(), &m_node, &castor3d::SceneNode::setOrientation );
		}
	}
}

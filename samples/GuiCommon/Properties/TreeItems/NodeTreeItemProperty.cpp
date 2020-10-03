#include "GuiCommon/Properties/TreeItems/NodeTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/Math/QuaternionProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	NodeTreeItemProperty::NodeTreeItemProperty( bool p_editable, Engine * engine, SceneNodeSPtr p_node )
		: TreeItemProperty( p_node->getScene()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_NODE )
		, m_node( p_node )
		, m_engine( engine )
	{
		CreateTreeItemMenu();
	}

	NodeTreeItemProperty::~NodeTreeItemProperty()
	{
	}

	void NodeTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_NODE = _( "Scene Node: " );
		static wxString PROPERTY_NODE_POSITION = _( "Position" );
		static wxString PROPERTY_NODE_SCALE = _( "Scale" );
		static wxString PROPERTY_NODE_ORIENTATION = _( "Orientation" );
		static wxString PROPERTY_NODE_VISIBLE = _( "Visible" );
		SceneNodeSPtr node = getNode();

		if ( node )
		{
			addProperty( grid, PROPERTY_CATEGORY_NODE + wxString( node->getName() ) );
			addPropertyT( grid, PROPERTY_NODE_VISIBLE, node->isVisible(), node.get(), &SceneNode::setVisible );
			addPropertyT( grid, PROPERTY_NODE_POSITION, node->getPosition(), node.get(), &SceneNode::setPosition );
			addPropertyT( grid, PROPERTY_NODE_SCALE, node->getScale(), node.get(), &SceneNode::setScale );
			addPropertyT( grid, PROPERTY_NODE_ORIENTATION, node->getOrientation(), node.get(), &SceneNode::setOrientation );
		}
	}
}

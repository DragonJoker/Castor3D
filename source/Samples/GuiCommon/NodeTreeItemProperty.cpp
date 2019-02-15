#include "NodeTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>

#include "PointProperties.hpp"
#include "QuaternionProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_NODE = _( "Scene Node: " );
		static wxString PROPERTY_NODE_POSITION = _( "Position" );
		static wxString PROPERTY_NODE_SCALE = _( "Scale" );
		static wxString PROPERTY_NODE_ORIENTATION = _( "Orientation" );
		static wxString PROPERTY_NODE_VISIBLE = _( "Visible" );
	}

	NodeTreeItemProperty::NodeTreeItemProperty( bool p_editable, Engine * engine, SceneNodeSPtr p_node )
		: TreeItemProperty( p_node->getScene()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_NODE )
		, m_node( p_node )
		, m_engine( engine )
	{
		PROPERTY_CATEGORY_NODE = _( "Scene Node: " );
		PROPERTY_NODE_POSITION = _( "Position" );
		PROPERTY_NODE_SCALE = _( "Scale" );
		PROPERTY_NODE_ORIENTATION = _( "Orientation" );
		PROPERTY_NODE_VISIBLE = _( "Visible" );

		CreateTreeItemMenu();
	}

	NodeTreeItemProperty::~NodeTreeItemProperty()
	{
	}

	void NodeTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		SceneNodeSPtr node = getNode();

		if ( node )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_NODE + wxString( node->getName() ) ) );
			p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_NODE_POSITION ) )->SetValue( WXVARIANT( node->getPosition() ) );
			p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_NODE_SCALE ) )->SetValue( WXVARIANT( node->getScale() ) );
			p_grid->Append( new QuaternionProperty( PROPERTY_NODE_ORIENTATION ) )->SetValue( WXVARIANT( node->getOrientation() ) );
			p_grid->Append( CreateProperty( PROPERTY_NODE_VISIBLE, node->isVisible(), true ) );
		}
	}

	void NodeTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneNodeSPtr node = getNode();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && node )
		{
			if ( property->GetName() == PROPERTY_NODE_POSITION )
			{
				OnPositionChange( PointRefFromVariant< real, 3 >( p_event.GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_NODE_SCALE )
			{
				OnScaleChange( PointRefFromVariant< real, 3 >( p_event.GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_NODE_ORIENTATION )
			{
				OnOrientationChange( QuaternionRefFromVariant( p_event.GetValue() ) );
			}
			else if ( property->GetName() == PROPERTY_NODE_VISIBLE )
			{
				OnVisibilityChange( p_event.GetValue() );
			}
		}
	}

	void NodeTreeItemProperty::OnPositionChange( Point3r const & p_value )
	{
		SceneNodeSPtr node = getNode();
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

		doApplyChange( [x, y, z, node]()
		{
			node->setPosition( Point3r( x, y, z ) );
		} );
	}

	void NodeTreeItemProperty::OnScaleChange( Point3r const & p_value )
	{
		SceneNodeSPtr node = getNode();
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

		doApplyChange( [x, y, z, node]()
		{
			node->setScale( Point3r( x, y, z ) );
		} );
	}

	void NodeTreeItemProperty::OnOrientationChange( Quaternion const & p_value )
	{
		SceneNodeSPtr node = getNode();

		doApplyChange( [p_value, node]()
		{
			node->setOrientation( p_value );
		} );
	}

	void NodeTreeItemProperty::OnVisibilityChange( bool p_value )
	{
		SceneNodeSPtr node = getNode();

		doApplyChange( [p_value, node]()
		{
			node->setVisible( p_value );
		} );
	}
}

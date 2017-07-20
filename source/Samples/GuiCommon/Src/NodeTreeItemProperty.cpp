#include "NodeTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>

#include "PointProperties.hpp"
#include "QuaternionProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

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

	NodeTreeItemProperty::NodeTreeItemProperty( bool p_editable, Engine * p_engine, SceneNodeSPtr p_node )
		: TreeItemProperty( p_node->GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_NODE )
		, m_node( p_node )
		, m_engine( p_engine )
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

	void NodeTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		SceneNodeSPtr node = GetNode();

		if ( node )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_NODE + wxString( node->GetName() ) ) );
			p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_NODE_POSITION ) )->SetValue( WXVARIANT( node->GetPosition() ) );
			p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_NODE_SCALE ) )->SetValue( WXVARIANT( node->GetScale() ) );
			p_grid->Append( new QuaternionProperty( PROPERTY_NODE_ORIENTATION ) )->SetValue( WXVARIANT( node->GetOrientation() ) );
			p_grid->Append( CreateProperty( PROPERTY_NODE_VISIBLE, node->IsVisible(), true ) );
		}
	}

	void NodeTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneNodeSPtr node = GetNode();
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
		SceneNodeSPtr node = GetNode();
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

		DoApplyChange( [x, y, z, node]()
		{
			node->SetPosition( Point3r( x, y, z ) );
		} );
	}

	void NodeTreeItemProperty::OnScaleChange( Point3r const & p_value )
	{
		SceneNodeSPtr node = GetNode();
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

		DoApplyChange( [x, y, z, node]()
		{
			node->SetScale( Point3r( x, y, z ) );
		} );
	}

	void NodeTreeItemProperty::OnOrientationChange( Quaternion const & p_value )
	{
		SceneNodeSPtr node = GetNode();

		DoApplyChange( [p_value, node]()
		{
			node->SetOrientation( p_value );
		} );
	}

	void NodeTreeItemProperty::OnVisibilityChange( bool p_value )
	{
		SceneNodeSPtr node = GetNode();

		DoApplyChange( [p_value, node]()
		{
			node->SetVisible( p_value );
		} );
	}
}

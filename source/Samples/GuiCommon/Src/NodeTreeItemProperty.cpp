#include "NodeTreeItemProperty.hpp"

#include <SceneNode.hpp>
#include <Engine.hpp>
#include <FunctorEvent.hpp>

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
		: TreeItemProperty( p_node->GetOwner()->GetOwner(), p_editable, ePROPERTY_DATA_TYPE_NODE )
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
		SceneNodeSPtr l_node = GetNode();

		if ( l_node )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_NODE + wxString( l_node->GetName() ) ) );
			p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_NODE_POSITION ) )->SetValue( WXVARIANT( l_node->GetPosition() ) );
			p_grid->Append( new Point3rProperty( GC_POINT_XYZ, PROPERTY_NODE_SCALE ) )->SetValue( WXVARIANT( l_node->GetScale() ) );
			p_grid->Append( new QuaternionProperty( PROPERTY_NODE_ORIENTATION ) )->SetValue( WXVARIANT( l_node->GetOrientation() ) );
			p_grid->Append( CreateProperty( PROPERTY_NODE_VISIBLE, l_node->IsVisible(), true ) );
		}
	}

	void NodeTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneNodeSPtr l_node = GetNode();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_node )
		{
			if ( l_property->GetName() == PROPERTY_NODE_POSITION )
			{
				OnPositionChange( PointRefFromVariant< real, 3 >( p_event.GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_NODE_SCALE )
			{
				OnScaleChange( PointRefFromVariant< real, 3 >( p_event.GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_NODE_ORIENTATION )
			{
				OnOrientationChange( QuaternionRefFromVariant( p_event.GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_NODE_VISIBLE )
			{
				OnVisibilityChange( p_event.GetValue() );
			}
		}
	}

	void NodeTreeItemProperty::OnPositionChange( Point3r const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

		DoApplyChange( [x, y, z, l_node]()
		{
			l_node->SetPosition( Point3r( x, y, z ) );
		} );
	}

	void NodeTreeItemProperty::OnScaleChange( Point3r const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();
		float x = p_value[0];
		float y = p_value[1];
		float z = p_value[2];

		DoApplyChange( [x, y, z, l_node]()
		{
			l_node->SetScale( Point3r( x, y, z ) );
		} );
	}

	void NodeTreeItemProperty::OnOrientationChange( Quaternion const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		DoApplyChange( [p_value, l_node]()
		{
			l_node->SetOrientation( p_value );
		} );
	}

	void NodeTreeItemProperty::OnVisibilityChange( bool p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		DoApplyChange( [p_value, l_node]()
		{
			l_node->SetVisible( p_value );
		} );
	}
}

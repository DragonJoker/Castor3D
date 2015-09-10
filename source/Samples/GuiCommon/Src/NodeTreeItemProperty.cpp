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
		static const wxString PROPERTY_CATEGORY_NODE = _( "Scene Node: " );
		static const wxString PROPERTY_NODE_POSITION = _( "Position" );
		static const wxString PROPERTY_NODE_SCALE = _( "Scale" );
		static const wxString PROPERTY_NODE_ORIENTATION = _( "Orientation" );
	}

	NodeTreeItemProperty::NodeTreeItemProperty( bool p_editable, Engine * p_engine, SceneNodeSPtr p_node )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_NODE )
		, m_node( p_node )
		, m_engine( p_engine )
	{
	}

	NodeTreeItemProperty::~NodeTreeItemProperty()
	{
	}

	void NodeTreeItemProperty::CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		SceneNodeSPtr l_node = GetNode();

		if ( l_node )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_NODE + wxString( l_node->GetName() ) ) );
			p_grid->Append( new Point3rProperty( PROPERTY_NODE_POSITION ) )->SetValue( wxVariant( l_node->GetPosition() ) );
			p_grid->Append( new Point3rProperty( PROPERTY_NODE_SCALE ) )->SetValue( wxVariant( l_node->GetScale() ) );
			p_grid->Append( new QuaternionProperty( PROPERTY_NODE_ORIENTATION ) )->SetValue( wxVariant( l_node->GetOrientation() ) );
		}
	}

	void NodeTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
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
		}
	}

	void NodeTreeItemProperty::OnPositionChange( Castor::Point3r const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		m_engine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_node]()
		{
			l_node->SetPosition( p_value );
		} ) );
	}

	void NodeTreeItemProperty::OnScaleChange( Castor::Point3r const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		m_engine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_node]()
		{
			l_node->SetScale( p_value );
		} ) );
	}

	void NodeTreeItemProperty::OnOrientationChange( Castor::Quaternion const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		m_engine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_node]()
		{
			l_node->SetOrientation( p_value );
		} ) );
	}
}

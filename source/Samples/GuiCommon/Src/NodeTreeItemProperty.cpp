#include "NodeTreeItemProperty.hpp"

#include <SceneNode.hpp>
#include <Engine.hpp>
#include <FunctorEvent.hpp>

#include "AdditionalProperties.hpp"
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

	wxNodeTreeItemProperty::wxNodeTreeItemProperty( Engine * p_engine, SceneNodeSPtr p_node )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_NODE )
		, m_node( p_node )
		, m_engine( p_engine )
	{
	}

	wxNodeTreeItemProperty::~wxNodeTreeItemProperty()
	{
	}

	void wxNodeTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
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

	void wxNodeTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		SceneNodeSPtr l_node = GetNode();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_node )
		{
			if ( l_property->GetName() == PROPERTY_NODE_POSITION )
			{
				OnPositionChange( Point3rRefFromVariant( p_event.GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_NODE_SCALE )
			{
				OnScaleChange( Point3rRefFromVariant( p_event.GetValue() ) );
			}
			else if ( l_property->GetName() == PROPERTY_NODE_ORIENTATION )
			{
				OnOrientationChange( QuaternionRefFromVariant( p_event.GetValue() ) );
			}
		}
	}

	void wxNodeTreeItemProperty::OnPositionChange( Castor::Point3r const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		m_engine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_node]()
		{
			l_node->SetPosition( p_value );
		} ) );
	}

	void wxNodeTreeItemProperty::OnScaleChange( Castor::Point3r const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		m_engine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_node]()
		{
			l_node->SetScale( p_value );
		} ) );
	}

	void wxNodeTreeItemProperty::OnOrientationChange( Castor::Quaternion const & p_value )
	{
		SceneNodeSPtr l_node = GetNode();

		m_engine->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_node]()
		{
			l_node->SetOrientation( p_value );
		} ) );
	}
}

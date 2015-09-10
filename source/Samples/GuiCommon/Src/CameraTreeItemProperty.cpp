#include "CameraTreeItemProperty.hpp"

#include <Camera.hpp>
#include <FunctorEvent.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_CAMERA = _( "Camera: " );
		static const wxString PROPERTY_CATEGORY_VIEWPORT = _( "Viewport" );
		static const wxString PROPERTY_TOPOLOGY = _( "Topology" );
		static const wxString PROPERTY_TOPOLOGY_POINTS = _( "Points" );
		static const wxString PROPERTY_TOPOLOGY_LINES = _( "Lines" );
		static const wxString PROPERTY_TOPOLOGY_LINE_LOOP = _( "Line Loop" );
		static const wxString PROPERTY_TOPOLOGY_LINE_STRIP = _( "Line Strip" );
		static const wxString PROPERTY_TOPOLOGY_TRIANGLES = _( "Triangles" );
		static const wxString PROPERTY_TOPOLOGY_TRIANGLE_STRIP = _( "Triangle Strip" );
		static const wxString PROPERTY_TOPOLOGY_TRIANGLE_FAN = _( "Triangle Fan" );
		static const wxString PROPERTY_TOPOLOGY_QUADS = _( "Quads" );
		static const wxString PROPERTY_TOPOLOGY_QUAD_STRIP = _( "Quad Strip" );
		static const wxString PROPERTY_TOPOLOGY_POLYGON = _( "Polygon" );
	}

	CameraTreeItemProperty::CameraTreeItemProperty( bool p_editable, Castor3D::CameraSPtr p_camera )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_camera( p_camera )
	{
	}

	CameraTreeItemProperty::~CameraTreeItemProperty()
	{
	}

	void CameraTreeItemProperty::CreateProperties( wxPropertyGrid * p_grid )
	{
		CameraSPtr l_camera = GetCamera();

		if ( l_camera )
		{
			wxPGChoices l_choices;
			l_choices.Add( PROPERTY_TOPOLOGY_POINTS );
			l_choices.Add( PROPERTY_TOPOLOGY_LINES );
			l_choices.Add( PROPERTY_TOPOLOGY_LINE_LOOP );
			l_choices.Add( PROPERTY_TOPOLOGY_LINE_STRIP );
			l_choices.Add( PROPERTY_TOPOLOGY_TRIANGLES );
			l_choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_STRIP );
			l_choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_FAN );
			l_choices.Add( PROPERTY_TOPOLOGY_QUADS );
			l_choices.Add( PROPERTY_TOPOLOGY_QUAD_STRIP );
			l_choices.Add( PROPERTY_TOPOLOGY_POLYGON );
			wxString l_selected;

			switch ( l_camera->GetPrimitiveType() )
			{
			case eTOPOLOGY_POINTS:
				l_selected = PROPERTY_TOPOLOGY_POINTS;
				break;

			case eTOPOLOGY_LINES:
				l_selected = PROPERTY_TOPOLOGY_LINES;
				break;

			case eTOPOLOGY_LINE_LOOP:
				l_selected = PROPERTY_TOPOLOGY_LINE_LOOP;
				break;

			case eTOPOLOGY_LINE_STRIP:
				l_selected = PROPERTY_TOPOLOGY_LINE_STRIP;
				break;

			case eTOPOLOGY_TRIANGLES:
				l_selected = PROPERTY_TOPOLOGY_TRIANGLES;
				break;

			case eTOPOLOGY_TRIANGLE_STRIPS:
				l_selected = PROPERTY_TOPOLOGY_TRIANGLE_STRIP;
				break;

			case eTOPOLOGY_TRIANGLE_FAN:
				l_selected = PROPERTY_TOPOLOGY_TRIANGLE_FAN;
				break;

			case eTOPOLOGY_QUADS:
				l_selected = PROPERTY_TOPOLOGY_QUADS;
				break;

			case eTOPOLOGY_QUAD_STRIPS:
				l_selected = PROPERTY_TOPOLOGY_QUAD_STRIP;
				break;

			case eTOPOLOGY_POLYGON:
				l_selected = PROPERTY_TOPOLOGY_POLYGON;
				break;
			}

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_CAMERA ) );
			p_grid->Append( new wxEnumProperty( PROPERTY_TOPOLOGY, PROPERTY_TOPOLOGY, l_choices ) )->SetValue( l_selected );
		}
	}

	void CameraTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		CameraSPtr l_camera = GetCamera();

		if ( l_property && l_camera )
		{
			if ( l_property->GetName() == PROPERTY_TOPOLOGY )
			{
				if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_POINTS )
				{
					OnTopologyChange( eTOPOLOGY_POINTS );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINES )
				{
					OnTopologyChange( eTOPOLOGY_LINES );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_LOOP )
				{
					OnTopologyChange( eTOPOLOGY_LINE_LOOP );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_STRIP )
				{
					OnTopologyChange( eTOPOLOGY_LINE_STRIP );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLES )
				{
					OnTopologyChange( eTOPOLOGY_TRIANGLES );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_STRIP )
				{
					OnTopologyChange( eTOPOLOGY_TRIANGLE_STRIPS );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_FAN )
				{
					OnTopologyChange( eTOPOLOGY_TRIANGLE_FAN );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_QUADS )
				{
					OnTopologyChange( eTOPOLOGY_QUADS );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_QUAD_STRIP )
				{
					OnTopologyChange( eTOPOLOGY_QUAD_STRIPS );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_POLYGON )
				{
					OnTopologyChange( eTOPOLOGY_POLYGON );
				}
			}
		}
	}

	void CameraTreeItemProperty::OnTopologyChange( eTOPOLOGY p_value )
	{
		CameraSPtr l_camera = GetCamera();

		l_camera->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_camera]()
		{
			l_camera->SetPrimitiveType( p_value );
		} ) );
	}
}

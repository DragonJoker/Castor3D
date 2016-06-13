#include "SubmeshTreeItemProperty.hpp"

#include <MaterialManager.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>

#include "AdditionalProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		static wxString PROPERTY_SUBMESH_MATERIAL = _( "Material" );
		static wxString PROPERTY_TOPOLOGY = _( "Topology" );
		static wxString PROPERTY_TOPOLOGY_POINTS = _( "Points" );
		static wxString PROPERTY_TOPOLOGY_LINES = _( "Lines" );
		static wxString PROPERTY_TOPOLOGY_LINE_LOOP = _( "Line Loop" );
		static wxString PROPERTY_TOPOLOGY_LINE_STRIP = _( "Line Strip" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLES = _( "Triangles" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLE_STRIP = _( "Triangle Strip" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLE_FAN = _( "Triangle Fan" );
		static wxString PROPERTY_TOPOLOGY_QUADS = _( "Quads" );
		static wxString PROPERTY_TOPOLOGY_QUAD_STRIP = _( "Quad Strip" );
		static wxString PROPERTY_TOPOLOGY_POLYGON = _( "Polygon" );
	}

	SubmeshTreeItemProperty::SubmeshTreeItemProperty( bool p_editable, GeometrySPtr p_pGeometry, SubmeshSPtr p_pSubmesh )
		: TreeItemProperty( p_pSubmesh->GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_SUBMESH )
		, m_pGeometry( p_pGeometry )
		, m_pSubmesh( p_pSubmesh )
	{
		PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		PROPERTY_SUBMESH_MATERIAL = _( "Material" );
		PROPERTY_TOPOLOGY = _( "Topology" );
		PROPERTY_TOPOLOGY_POINTS = _( "Points" );
		PROPERTY_TOPOLOGY_LINES = _( "Lines" );
		PROPERTY_TOPOLOGY_LINE_LOOP = _( "Line Loop" );
		PROPERTY_TOPOLOGY_LINE_STRIP = _( "Line Strip" );
		PROPERTY_TOPOLOGY_TRIANGLES = _( "Triangles" );
		PROPERTY_TOPOLOGY_TRIANGLE_STRIP = _( "Triangle Strip" );
		PROPERTY_TOPOLOGY_TRIANGLE_FAN = _( "Triangle Fan" );
		PROPERTY_TOPOLOGY_QUADS = _( "Quads" );
		PROPERTY_TOPOLOGY_QUAD_STRIP = _( "Quad Strip" );
		PROPERTY_TOPOLOGY_POLYGON = _( "Polygon" );

		CreateTreeItemMenu();
	}

	SubmeshTreeItemProperty::~SubmeshTreeItemProperty()
	{
	}

	void SubmeshTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		GeometrySPtr l_geometry = GetGeometry();
		SubmeshSPtr l_submesh = GetSubmesh();

		if ( l_geometry && l_submesh )
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

			switch ( l_submesh->GetTopology() )
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

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SUBMESH + wxString( l_geometry->GetName() ) ) );
			p_grid->Append( new wxEnumProperty( PROPERTY_TOPOLOGY, PROPERTY_TOPOLOGY, l_choices ) )->SetValue( l_selected );
			p_grid->Append( DoCreateMaterialProperty( PROPERTY_SUBMESH_MATERIAL ) )->SetValue( wxVariant( make_wxString( l_geometry->GetMaterial( l_submesh )->GetName() ) ) );
		}
	}

	void SubmeshTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		GeometrySPtr l_geometry = GetGeometry();
		SubmeshSPtr l_submesh = GetSubmesh();

		if ( l_property )
		{
			if ( l_property->GetName() == PROPERTY_SUBMESH_MATERIAL )
			{
				OnMaterialChange( String( l_property->GetValueAsString().c_str() ) );
			}
			else if ( l_property->GetName() == PROPERTY_TOPOLOGY )
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

	void SubmeshTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		SubmeshSPtr l_submesh = GetSubmesh();
		GeometrySPtr l_geometry = GetGeometry();

		DoApplyChange( [p_name, l_geometry, l_submesh]()
		{
			MaterialManager & l_manager = l_submesh->GetScene()->GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.Find( p_name );

			if ( l_material )
			{
				l_geometry->SetMaterial( l_submesh, l_material );
				l_geometry->GetScene()->SetChanged();
			}
		} );
	}

	void SubmeshTreeItemProperty::OnTopologyChange( eTOPOLOGY p_value )
	{
		SubmeshSPtr l_submesh = GetSubmesh();
		GeometrySPtr l_geometry = GetGeometry();

		DoApplyChange( [p_value, l_geometry, l_submesh]()
		{
			l_submesh->SetTopology( p_value );
		} );
	}
}

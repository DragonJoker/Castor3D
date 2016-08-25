#include "SubmeshTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
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

	SubmeshTreeItemProperty::SubmeshTreeItemProperty( bool p_editable, GeometrySPtr p_pGeometry, SubmeshSPtr p_submesh )
		: TreeItemProperty( p_submesh->GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_SUBMESH )
		, m_pGeometry( p_pGeometry )
		, m_pSubmesh( p_submesh )
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
			case Topology::Points:
				l_selected = PROPERTY_TOPOLOGY_POINTS;
				break;

			case Topology::Lines:
				l_selected = PROPERTY_TOPOLOGY_LINES;
				break;

			case Topology::LineLoop:
				l_selected = PROPERTY_TOPOLOGY_LINE_LOOP;
				break;

			case Topology::LineStrip:
				l_selected = PROPERTY_TOPOLOGY_LINE_STRIP;
				break;

			case Topology::Triangles:
				l_selected = PROPERTY_TOPOLOGY_TRIANGLES;
				break;

			case Topology::TriangleStrips:
				l_selected = PROPERTY_TOPOLOGY_TRIANGLE_STRIP;
				break;

			case Topology::TriangleFan:
				l_selected = PROPERTY_TOPOLOGY_TRIANGLE_FAN;
				break;

			case Topology::Quads:
				l_selected = PROPERTY_TOPOLOGY_QUADS;
				break;

			case Topology::QuadStrips:
				l_selected = PROPERTY_TOPOLOGY_QUAD_STRIP;
				break;

			case Topology::Polygon:
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
					OnTopologyChange( Topology::Points );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINES )
				{
					OnTopologyChange( Topology::Lines );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_LOOP )
				{
					OnTopologyChange( Topology::LineLoop );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_STRIP )
				{
					OnTopologyChange( Topology::LineStrip );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLES )
				{
					OnTopologyChange( Topology::Triangles );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_STRIP )
				{
					OnTopologyChange( Topology::TriangleStrips );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_FAN )
				{
					OnTopologyChange( Topology::TriangleFan );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_QUADS )
				{
					OnTopologyChange( Topology::Quads );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_QUAD_STRIP )
				{
					OnTopologyChange( Topology::QuadStrips );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_POLYGON )
				{
					OnTopologyChange( Topology::Polygon );
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
			auto & l_cache = l_submesh->GetScene()->GetEngine()->GetMaterialCache();
			MaterialSPtr l_material = l_cache.Find( p_name );

			if ( l_material )
			{
				l_geometry->SetMaterial( l_submesh, l_material );
				l_geometry->GetScene()->SetChanged();
			}
		} );
	}

	void SubmeshTreeItemProperty::OnTopologyChange( Topology p_value )
	{
		SubmeshSPtr l_submesh = GetSubmesh();
		GeometrySPtr l_geometry = GetGeometry();

		DoApplyChange( [p_value, l_geometry, l_submesh]()
		{
			l_submesh->SetTopology( p_value );
		} );
	}
}

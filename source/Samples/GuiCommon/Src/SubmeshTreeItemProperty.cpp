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

	SubmeshTreeItemProperty::SubmeshTreeItemProperty( bool p_editable, Geometry & p_geometry, Submesh & p_submesh )
		: TreeItemProperty( p_submesh.GetScene()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_SUBMESH )
		, m_geometry( p_geometry )
		, m_submesh( p_submesh )
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

		switch ( m_submesh.GetTopology() )
		{
		case Topology::ePoints:
			l_selected = PROPERTY_TOPOLOGY_POINTS;
			break;

		case Topology::eLines:
			l_selected = PROPERTY_TOPOLOGY_LINES;
			break;

		case Topology::eLineLoop:
			l_selected = PROPERTY_TOPOLOGY_LINE_LOOP;
			break;

		case Topology::eLineStrip:
			l_selected = PROPERTY_TOPOLOGY_LINE_STRIP;
			break;

		case Topology::eTriangles:
			l_selected = PROPERTY_TOPOLOGY_TRIANGLES;
			break;

		case Topology::eTriangleStrips:
			l_selected = PROPERTY_TOPOLOGY_TRIANGLE_STRIP;
			break;

		case Topology::eTriangleFan:
			l_selected = PROPERTY_TOPOLOGY_TRIANGLE_FAN;
			break;

		case Topology::eQuads:
			l_selected = PROPERTY_TOPOLOGY_QUADS;
			break;

		case Topology::eQuadStrips:
			l_selected = PROPERTY_TOPOLOGY_QUAD_STRIP;
			break;

		case Topology::ePolygon:
			l_selected = PROPERTY_TOPOLOGY_POLYGON;
			break;
		}

		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SUBMESH + wxString( m_geometry.GetName() ) ) );
		p_grid->Append( new wxEnumProperty( PROPERTY_TOPOLOGY, PROPERTY_TOPOLOGY, l_choices ) )->SetValue( l_selected );
		p_grid->Append( DoCreateMaterialProperty( PROPERTY_SUBMESH_MATERIAL ) )->SetValue( wxVariant( make_wxString( m_geometry.GetMaterial( m_submesh )->GetName() ) ) );
	}

	void SubmeshTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

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
					OnTopologyChange( Topology::ePoints );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINES )
				{
					OnTopologyChange( Topology::eLines );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_LOOP )
				{
					OnTopologyChange( Topology::eLineLoop );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_STRIP )
				{
					OnTopologyChange( Topology::eLineStrip );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLES )
				{
					OnTopologyChange( Topology::eTriangles );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_STRIP )
				{
					OnTopologyChange( Topology::eTriangleStrips );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_FAN )
				{
					OnTopologyChange( Topology::eTriangleFan );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_QUADS )
				{
					OnTopologyChange( Topology::eQuads );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_QUAD_STRIP )
				{
					OnTopologyChange( Topology::eQuadStrips );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TOPOLOGY_POLYGON )
				{
					OnTopologyChange( Topology::ePolygon );
				}
			}
		}
	}

	void SubmeshTreeItemProperty::OnMaterialChange( Castor::String const & p_name )
	{
		DoApplyChange( [p_name, this]()
		{
			auto & l_cache = m_submesh.GetScene()->GetEngine()->GetMaterialCache();
			MaterialSPtr l_material = l_cache.Find( p_name );

			if ( l_material )
			{
				m_geometry.SetMaterial( m_submesh, l_material );
				m_geometry.GetScene()->SetChanged();
			}
		} );
	}

	void SubmeshTreeItemProperty::OnTopologyChange( Topology p_value )
	{
		DoApplyChange( [p_value, this]()
		{
			m_submesh.SetTopology( p_value );
		} );
	}
}

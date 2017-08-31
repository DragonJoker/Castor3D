#include "SubmeshTreeItemProperty.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>

#include "AdditionalProperties.hpp"
#include "CubeBoxProperties.hpp"
#include "PointProperties.hpp"
#include "SphereBoxProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		static wxString PROPERTY_SUBMESH_MATERIAL = _( "Material" );
		static wxString PROPERTY_SUBMESH_CUBE_BOX = _( "Cube box" );
		static wxString PROPERTY_SUBMESH_SPHERE_BOX = _( "Sphere box" );
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
		: TreeItemProperty( p_submesh.getScene()->getEngine(), p_editable, ePROPERTY_DATA_TYPE_SUBMESH )
		, m_geometry( p_geometry )
		, m_submesh( p_submesh )
	{
		PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		PROPERTY_SUBMESH_MATERIAL = _( "Material" );
		PROPERTY_SUBMESH_CUBE_BOX = _( "Cube box" );
		PROPERTY_SUBMESH_SPHERE_BOX = _( "Sphere box" );
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

	void SubmeshTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		wxPGChoices choices;
		choices.Add( PROPERTY_TOPOLOGY_POINTS );
		choices.Add( PROPERTY_TOPOLOGY_LINES );
		choices.Add( PROPERTY_TOPOLOGY_LINE_LOOP );
		choices.Add( PROPERTY_TOPOLOGY_LINE_STRIP );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLES );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_STRIP );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_FAN );
		choices.Add( PROPERTY_TOPOLOGY_QUADS );
		choices.Add( PROPERTY_TOPOLOGY_QUAD_STRIP );
		choices.Add( PROPERTY_TOPOLOGY_POLYGON );
		wxString selected;

		switch ( m_submesh.getTopology() )
		{
		case Topology::ePoints:
			selected = PROPERTY_TOPOLOGY_POINTS;
			break;

		case Topology::eLines:
			selected = PROPERTY_TOPOLOGY_LINES;
			break;

		case Topology::eLineLoop:
			selected = PROPERTY_TOPOLOGY_LINE_LOOP;
			break;

		case Topology::eLineStrip:
			selected = PROPERTY_TOPOLOGY_LINE_STRIP;
			break;

		case Topology::eTriangles:
			selected = PROPERTY_TOPOLOGY_TRIANGLES;
			break;

		case Topology::eTriangleStrips:
			selected = PROPERTY_TOPOLOGY_TRIANGLE_STRIP;
			break;

		case Topology::eTriangleFan:
			selected = PROPERTY_TOPOLOGY_TRIANGLE_FAN;
			break;

		case Topology::eQuads:
			selected = PROPERTY_TOPOLOGY_QUADS;
			break;

		case Topology::eQuadStrips:
			selected = PROPERTY_TOPOLOGY_QUAD_STRIP;
			break;

		case Topology::ePolygon:
			selected = PROPERTY_TOPOLOGY_POLYGON;
			break;
		}

		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SUBMESH + wxString( m_geometry.getName() ) ) );
		p_grid->Append( new wxEnumProperty( PROPERTY_TOPOLOGY
			, PROPERTY_TOPOLOGY
			, choices ) )->SetValue( selected );
		p_grid->Append( new SphereBoxProperty( PROPERTY_SUBMESH_SPHERE_BOX
			, PROPERTY_SUBMESH_SPHERE_BOX
			, m_submesh.getCollisionSphere() ) )->Enable( false );
		p_grid->Append( new CubeBoxProperty( PROPERTY_SUBMESH_CUBE_BOX
			, PROPERTY_SUBMESH_CUBE_BOX
			, m_submesh.getCollisionBox() ) )->Enable( false );
		p_grid->Append( doCreateMaterialProperty( PROPERTY_SUBMESH_MATERIAL ) )->SetValue( wxVariant( make_wxString( m_geometry.getMaterial( m_submesh )->getName() ) ) );
	}

	void SubmeshTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_SUBMESH_MATERIAL )
			{
				OnMaterialChange( String( property->GetValueAsString().c_str() ) );
			}
			else if ( property->GetName() == PROPERTY_TOPOLOGY )
			{
				if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_POINTS )
				{
					OnTopologyChange( Topology::ePoints );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_LINES )
				{
					OnTopologyChange( Topology::eLines );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_LOOP )
				{
					OnTopologyChange( Topology::eLineLoop );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_STRIP )
				{
					OnTopologyChange( Topology::eLineStrip );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLES )
				{
					OnTopologyChange( Topology::eTriangles );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_STRIP )
				{
					OnTopologyChange( Topology::eTriangleStrips );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_FAN )
				{
					OnTopologyChange( Topology::eTriangleFan );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_QUADS )
				{
					OnTopologyChange( Topology::eQuads );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_QUAD_STRIP )
				{
					OnTopologyChange( Topology::eQuadStrips );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_POLYGON )
				{
					OnTopologyChange( Topology::ePolygon );
				}
			}
		}
	}

	void SubmeshTreeItemProperty::OnMaterialChange( castor::String const & p_name )
	{
		doApplyChange( [p_name, this]()
		{
			auto & cache = m_submesh.getScene()->getEngine()->getMaterialCache();
			MaterialSPtr material = cache.find( p_name );

			if ( material )
			{
				m_geometry.setMaterial( m_submesh, material );
				m_geometry.getScene()->setChanged();
			}
		} );
	}

	void SubmeshTreeItemProperty::OnTopologyChange( Topology p_value )
	{
		doApplyChange( [p_value, this]()
		{
			m_submesh.setTopology( p_value );
		} );
	}
}

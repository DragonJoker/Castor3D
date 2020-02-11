#include "GuiCommon/SubmeshTreeItemProperty.hpp"

#include "GuiCommon/AdditionalProperties.hpp"
#include "GuiCommon/CubeBoxProperties.hpp"
#include "GuiCommon/PointProperties.hpp"
#include "GuiCommon/SphereBoxProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

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
		static wxString PROPERTY_TOPOLOGY_POINT_LIST = _( "Point List" );
		static wxString PROPERTY_TOPOLOGY_LINE_LIST = _( "Line List" );
		static wxString PROPERTY_TOPOLOGY_LINE_STRIP = _( "Line Strip" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLE_LIST = _( "Triangle List" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLE_STRIP = _( "Triangle Strip" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLE_FAN = _( "Triangle Fan" );
		static wxString PROPERTY_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = _( "Line List With Adjacency" );
		static wxString PROPERTY_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = _( "Line Strip With Adjacency" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = _( "Triangle List With Adjacency" );
		static wxString PROPERTY_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = _( "Triangle Strip With Adjacency" );
		static wxString PROPERTY_TOPOLOGY_PATCH_LIST = _( "Patch List" );
	}

	SubmeshTreeItemProperty::SubmeshTreeItemProperty( bool editable, Geometry & geometry, Submesh & submesh )
		: TreeItemProperty( submesh.getOwner()->getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_SUBMESH )
		, m_geometry( geometry )
		, m_submesh( submesh )
	{
		PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		PROPERTY_SUBMESH_MATERIAL = _( "Material" );
		PROPERTY_SUBMESH_CUBE_BOX = _( "Cube box" );
		PROPERTY_SUBMESH_SPHERE_BOX = _( "Sphere box" );
		PROPERTY_TOPOLOGY = _( "Topology" );
		PROPERTY_TOPOLOGY_POINT_LIST = _( "Points" );
		PROPERTY_TOPOLOGY_LINE_LIST = _( "Lines" );
		PROPERTY_TOPOLOGY_LINE_STRIP = _( "Line Strip" );
		PROPERTY_TOPOLOGY_TRIANGLE_LIST = _( "Triangle List" );
		PROPERTY_TOPOLOGY_TRIANGLE_STRIP = _( "Triangle Strip" );
		PROPERTY_TOPOLOGY_TRIANGLE_FAN = _( "Triangle Fan" );
		PROPERTY_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = _( "Line List With Adjacency" );
		PROPERTY_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = _( "Line Strip With Adjacency" );
		PROPERTY_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = _( "Triangle List With Adjacency" );
		PROPERTY_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = _( "Triangle Strip With Adjacency" );
		PROPERTY_TOPOLOGY_PATCH_LIST = _( "Patch List" );

		CreateTreeItemMenu();
	}

	SubmeshTreeItemProperty::~SubmeshTreeItemProperty()
	{
	}

	void SubmeshTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * p_grid )
	{
		wxPGChoices choices;
		choices.Add( PROPERTY_TOPOLOGY_POINT_LIST );
		choices.Add( PROPERTY_TOPOLOGY_LINE_LIST );
		choices.Add( PROPERTY_TOPOLOGY_LINE_STRIP );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_LIST );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_STRIP );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_FAN );
		choices.Add( PROPERTY_TOPOLOGY_LINE_LIST_WITH_ADJACENCY );
		choices.Add( PROPERTY_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY );
		choices.Add( PROPERTY_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY );
		choices.Add( PROPERTY_TOPOLOGY_PATCH_LIST );
		wxString selected;

		switch ( m_submesh.getTopology() )
		{
		case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
			selected = PROPERTY_TOPOLOGY_POINT_LIST;
			break;

		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
			selected = PROPERTY_TOPOLOGY_LINE_LIST;
			break;

		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			selected = PROPERTY_TOPOLOGY_LINE_STRIP;
			break;

		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
			selected = PROPERTY_TOPOLOGY_TRIANGLE_LIST;
			break;

		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
			selected = PROPERTY_TOPOLOGY_TRIANGLE_STRIP;
			break;

		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			selected = PROPERTY_TOPOLOGY_TRIANGLE_FAN;
			break;

		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
			selected = PROPERTY_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
			break;

		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
			selected = PROPERTY_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
			break;

		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY:
			selected = PROPERTY_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
			break;

		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
			selected = PROPERTY_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
			break;

		case VK_PRIMITIVE_TOPOLOGY_PATCH_LIST:
			selected = PROPERTY_TOPOLOGY_PATCH_LIST;
			break;
		}

		p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_SUBMESH + wxString( m_geometry.getName() ) ) );
		p_grid->Append( new wxEnumProperty( PROPERTY_TOPOLOGY
			, PROPERTY_TOPOLOGY
			, choices ) )->SetValue( selected );
		p_grid->Append( new BoundingSphereProperty( PROPERTY_SUBMESH_SPHERE_BOX
			, PROPERTY_SUBMESH_SPHERE_BOX
			, m_submesh.getBoundingSphere() ) )->Enable( false );
		p_grid->Append( new BoundingBoxProperty( PROPERTY_SUBMESH_CUBE_BOX
			, PROPERTY_SUBMESH_CUBE_BOX
			, m_submesh.getBoundingBox() ) )->Enable( false );
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
				if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_POINT_LIST )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_LIST )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_STRIP )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_LIST )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_STRIP )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_FAN )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_LIST_WITH_ADJACENCY )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY );
				}
				else if ( property->GetValueAsString() == PROPERTY_TOPOLOGY_PATCH_LIST )
				{
					OnTopologyChange( VK_PRIMITIVE_TOPOLOGY_PATCH_LIST );
				}
			}
		}
	}

	void SubmeshTreeItemProperty::OnMaterialChange( castor::String const & p_name )
	{
		doApplyChange( [p_name, this]()
		{
			auto & cache = m_submesh.getOwner()->getScene()->getEngine()->getMaterialCache();
			MaterialSPtr material = cache.find( p_name );

			if ( material )
			{
				m_geometry.setMaterial( m_submesh, material );
				m_geometry.getScene()->setChanged();
			}
		} );
	}

	void SubmeshTreeItemProperty::OnTopologyChange( VkPrimitiveTopology value )
	{
		doApplyChange( [value, this]()
		{
			m_submesh.setTopology( value );
		} );
	}
}

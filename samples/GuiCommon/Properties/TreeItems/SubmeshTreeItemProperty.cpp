#include "GuiCommon/Properties/TreeItems/SubmeshTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/CubeBoxProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/Math/SphereBoxProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	SubmeshTreeItemProperty::SubmeshTreeItemProperty( bool editable, Geometry & geometry, Submesh & submesh )
		: TreeItemProperty( submesh.getOwner()->getScene()->getEngine(), editable, ePROPERTY_DATA_TYPE_SUBMESH )
		, m_geometry( geometry )
		, m_submesh( submesh )
	{
		CreateTreeItemMenu();
	}

	SubmeshTreeItemProperty::~SubmeshTreeItemProperty()
	{
	}

	void SubmeshTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
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

		wxArrayString choices;
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

		addProperty( grid, PROPERTY_CATEGORY_SUBMESH + wxString( m_geometry.getName() ) );
		addPropertyET( grid, PROPERTY_TOPOLOGY, choices, m_submesh.getTopology(), &m_submesh, &Submesh::setTopology );
		addProperty( grid, PROPERTY_SUBMESH_MATERIAL, getMaterialsList(), m_geometry.getMaterial( m_submesh )->getName()
			, [this]( wxVariant const & var )
			{
				auto name = make_String( var.GetString() );
				auto & cache = m_submesh.getOwner()->getScene()->getEngine()->getMaterialCache();
				MaterialSPtr material = cache.find( name );

				if ( material )
				{
					m_geometry.setMaterial( m_submesh, material );
					m_geometry.getScene()->setChanged();
				}
			} );
		addProperty( grid, PROPERTY_SUBMESH_SPHERE_BOX, m_submesh.getBoundingSphere(), EmptyHandler );
		addProperty( grid, PROPERTY_SUBMESH_SPHERE_BOX, m_submesh.getBoundingBox(), EmptyHandler );
	}
}

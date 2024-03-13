#include "GuiCommon/Properties/TreeItems/SubmeshTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/CubeBoxProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/Math/SphereBoxProperties.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <wx/propgrid/advprops.h>

namespace GuiCommon
{
	SubmeshTreeItemProperty::SubmeshTreeItemProperty( bool editable
		, castor3d::Geometry & geometry
		, castor3d::Submesh & submesh )
		: TreeItemProperty( submesh.getOwner()->getScene()->getEngine(), editable )
		, m_geometry( geometry )
		, m_submesh( submesh )
	{
		CreateTreeItemMenu();
	}

	void SubmeshTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_SUBMESH = _( "Submesh: " );
		static wxString PROPERTY_SUBMESH_MATERIAL = _( "Material" );

		m_materials = getMaterialsList();
		auto & engine = *m_geometry.getEngine();

		addProperty( grid, PROPERTY_CATEGORY_SUBMESH + wxString( m_geometry.getName() ) );
		addMaterial( grid, engine, PROPERTY_SUBMESH_MATERIAL, m_materials, m_geometry.getMaterial( m_submesh )
			, [this]( castor3d::MaterialObs material ) { m_geometry.setMaterial( m_submesh, material ); } );
		TreeItemConfigurationBuilder::submit( grid, *this, m_submesh );
	}
}

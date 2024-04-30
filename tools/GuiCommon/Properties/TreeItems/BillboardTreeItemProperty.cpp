#include "GuiCommon/Properties/TreeItems/BillboardTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Scene/BillboardList.hpp>
#include <Castor3D/Scene/Scene.hpp>

namespace GuiCommon
{
	BillboardTreeItemProperty::BillboardTreeItemProperty( bool editable
			, castor3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void BillboardTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY = _( "Billboard: " );
		static wxString PROPERTY_MATERIAL = _( "Material" );
		static wxString PROPERTY_SIZE = _( "Size" );

		if ( !m_billboard )
		{
			return;
		}

		m_materials = getMaterialsList();
		auto & engine = *m_billboard->getEngine();

		addProperty( grid, PROPERTY_CATEGORY + wxString( m_billboard->getName() ) );
		addPropertyT( grid, PROPERTY_SIZE, m_billboard->getDimensions(), m_billboard, &castor3d::BillboardList::setDimensions );
		addMaterial( grid, engine, PROPERTY_MATERIAL, m_materials, m_billboard->getMaterial()
			, [this]( castor3d::MaterialObs material ) { m_billboard->setMaterial( material ); } );
	}
}

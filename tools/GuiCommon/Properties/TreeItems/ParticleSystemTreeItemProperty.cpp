#include "GuiCommon/Properties/TreeItems/ParticleSystemTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleSystem.hpp>
#include <Castor3D/Scene/Scene.hpp>

namespace GuiCommon
{
	ParticleSystemTreeItemProperty::ParticleSystemTreeItemProperty( bool editable
			, castor3d::Engine * engine )
		: TreeItemProperty{ engine, editable }
	{
		CreateTreeItemMenu();
	}

	void ParticleSystemTreeItemProperty::doCreateProperties( wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY = _( "Particle System: " );
		static wxString PROPERTY_MATERIAL = _( "Material" );
		static wxString PROPERTY_SIZE = _( "Size" );

		if ( !m_particleSystem )
		{
			return;
		}

		m_materials = getMaterialsList();
		auto & engine = *m_particleSystem->getEngine();

		addProperty( grid, PROPERTY_CATEGORY + wxString( m_particleSystem->getName() ) );
		addPropertyT( grid, PROPERTY_SIZE, m_particleSystem->getDimensions(), m_particleSystem, &castor3d::ParticleSystem::setDimensions );
		addMaterial( grid, engine, PROPERTY_MATERIAL, m_materials, m_particleSystem->getMaterial()
			, [this]( castor3d::MaterialObs material ) { m_particleSystem->setMaterial( material ); } );
	}
}

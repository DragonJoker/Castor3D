/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_UNIT_H__
#define __COMC3D_COM_TEXTURE_UNIT_H__

#include "ComCastor3D/Castor3D/ComLightCategory.hpp"

#include <Castor3D/Scene/Light/PointLight.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, PointLight );
	/*!
	\~english
	\brief		This class defines a CPointLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CPointLight accessible depuis COM.
	*/
	class CPointLight
		: public CComAtlObject< PointLight, castor3d::PointLight >
	{
	public:
		COMEX_PROPERTY( Colour, IVector3D *, m_internal, &castor3d::LightCategory::getColour, &castor3d::LightCategory::setColour );
		COMEX_PROPERTY( DiffuseIntensity, float, m_internal, &castor3d::LightCategory::getDiffuseIntensity, &castor3d::LightCategory::setDiffuseIntensity );
		COMEX_PROPERTY( SpecularIntensity, float, m_internal, &castor3d::LightCategory::getSpecularIntensity, &castor3d::LightCategory::setSpecularIntensity );
		COMEX_PROPERTY( AttenuationRange, float, m_internal, &castor3d::PointLight::getRange, &castor3d::PointLight::setRange );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PointLight ), CPointLight );
}

#endif

/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SPOT_OLIGHT_H__
#define __COMC3D_COM_SPOT_OLIGHT_H__

#include "ComCastor3D/Castor3D/ComLightCategory.hpp"
#include "ComCastor3D/CastorUtils/ComAngle.hpp"

#include <Castor3D/Scene/Light/SpotLight.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, SpotLight );
	/*!
	\~english
	\brief		This class defines a CSpotLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CSpotLight accessible depuis COM.
	*/
	class CSpotLight
		: public CComAtlObject< SpotLight, castor3d::SpotLight >
	{
	public:
		COMEX_PROPERTY( Colour, IVector3D *, m_internal, &castor3d::SpotLight::getColour, &castor3d::SpotLight::setColour );
		COMEX_PROPERTY( DiffuseIntensity, float, m_internal, &castor3d::SpotLight::getDiffuseIntensity, &castor3d::SpotLight::setDiffuseIntensity );
		COMEX_PROPERTY( SpecularIntensity, float, m_internal, &castor3d::SpotLight::getSpecularIntensity, &castor3d::SpotLight::setSpecularIntensity );
		COMEX_PROPERTY( Attenuation, IVector3D *, m_internal, &castor3d::SpotLight::getAttenuation, &castor3d::SpotLight::setAttenuation );
		COMEX_PROPERTY( Exponent, float, m_internal, &castor3d::SpotLight::getExponent, &castor3d::SpotLight::setExponent );
		COMEX_PROPERTY( InnerCutOff, IAngle *, m_internal, &castor3d::SpotLight::getInnerCutOff, &castor3d::SpotLight::setInnerCutOff );
		COMEX_PROPERTY( OuterCutOff, IAngle *, m_internal, &castor3d::SpotLight::getOuterCutOff, &castor3d::SpotLight::setOuterCutOff );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SpotLight ), CSpotLight );
}

#endif

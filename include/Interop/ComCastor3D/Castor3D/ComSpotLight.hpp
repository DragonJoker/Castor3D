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
		COM_PROPERTY( Colour, IVector3D *, makeGetter( m_internal, &castor3d::LightCategory::getColour ), makePutter( m_internal, &castor3d::LightCategory::setColour ) );
		COM_PROPERTY( DiffuseIntensity, float, makeGetter( m_internal, &castor3d::LightCategory::getDiffuseIntensity ), makePutter( m_internal, &castor3d::LightCategory::setDiffuseIntensity ) );
		COM_PROPERTY( SpecularIntensity, float, makeGetter( m_internal, &castor3d::LightCategory::getSpecularIntensity ), makePutter( m_internal, &castor3d::LightCategory::setSpecularIntensity ) );
		COM_PROPERTY( Attenuation, IVector3D *, makeGetter( m_internal, &castor3d::SpotLight::getAttenuation ), makePutter( m_internal, &castor3d::SpotLight::setAttenuation ) );
		COM_PROPERTY( Exponent, float, makeGetter( m_internal, &castor3d::SpotLight::getExponent ), makePutter( m_internal, &castor3d::SpotLight::setExponent ) );
		COM_PROPERTY( InnerCutOff, IAngle *, makeGetter( m_internal, &castor3d::SpotLight::getInnerCutOff ), makePutter( m_internal, &castor3d::SpotLight::setInnerCutOff ) );
		COM_PROPERTY( OuterCutOff, IAngle *, makeGetter( m_internal, &castor3d::SpotLight::getOuterCutOff ), makePutter( m_internal, &castor3d::SpotLight::setOuterCutOff ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SpotLight ), CSpotLight );
}

#endif

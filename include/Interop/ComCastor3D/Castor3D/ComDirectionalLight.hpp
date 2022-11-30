/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_DIRECTIONAL_LIGHT_H__
#define __COMC3D_COM_DIRECTIONAL_LIGHT_H__

#include "ComCastor3D/Castor3D/ComLightCategory.hpp"

#include <Castor3D/Scene/Light/DirectionalLight.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, DirectionalLight );
	/*!
	\~english
	\brief		This class defines a CDirectionalLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CDirectionalLight accessible depuis COM.
	*/
	class CDirectionalLight
		: public CComAtlObject< DirectionalLight, castor3d::DirectionalLight >
	{
	public:
		COM_PROPERTY( Colour, IVector3D *, makeGetter( m_internal, &castor3d::LightCategory::getColour ), makePutter( m_internal, &castor3d::LightCategory::setColour ) );
		COM_PROPERTY( DiffuseIntensity, float, makeGetter( m_internal, &castor3d::LightCategory::getDiffuseIntensity ), makePutter( m_internal, &castor3d::LightCategory::setDiffuseIntensity ) );
		COM_PROPERTY( SpecularIntensity, float, makeGetter( m_internal, &castor3d::LightCategory::getSpecularIntensity ), makePutter( m_internal, &castor3d::LightCategory::setSpecularIntensity ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( DirectionalLight ), CDirectionalLight );
}

#endif

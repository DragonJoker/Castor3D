/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LIGHTGROUP_H__
#define __COMC3D_COM_LIGHTGROUP_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComDirectionalLight.hpp"
#include "ComCastor3D/Castor3D/ComPointLight.hpp"
#include "ComCastor3D/Castor3D/ComSpotLight.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"

#include <Castor3D/Scene/Light/LightGroup.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, LightGroup );
	/*!
	\~english
	\brief		This class defines a CLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CLight accessible depuis COM.
	*/
	class CLightGroup
		: public CComAtlObject< LightGroup, castor3d::LightGroup >
	{
	public:
		COMEX_PROPERTY_GET( Name, BSTR, m_internal, &castor3d::LightGroup::getName );
		COMEX_PROPERTY_GET( Scene, IScene *, m_internal, &castor3d::LightGroup::getScene );
		COMEX_PROPERTY_GET( LightType, eLIGHT_TYPE, m_internal, &castor3d::LightGroup::getLightType );
		COMEX_PROPERTY_GET( DirectionalLight, IDirectionalLight *, m_internal, &castor3d::LightGroup::getDirectionalLight );
		COMEX_PROPERTY_GET( PointLight, IPointLight *, m_internal, &castor3d::LightGroup::getPointLight );
		COMEX_PROPERTY_GET( SpotLight, ISpotLight *, m_internal, &castor3d::LightGroup::getSpotLight );

		STDMETHOD( AddLight )( /* [in] */ ISceneNode * val );
		STDMETHOD( RemoveLight )(/* [in] */ ISceneNode * val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LightGroup ), CLightGroup );
}

#endif

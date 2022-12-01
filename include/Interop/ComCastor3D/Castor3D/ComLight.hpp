/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LIGHT_H__
#define __COMC3D_COM_LIGHT_H__

#include "ComCastor3D/Castor3D/ComDirectionalLight.hpp"
#include "ComCastor3D/Castor3D/ComPointLight.hpp"
#include "ComCastor3D/Castor3D/ComSpotLight.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"

#include <Castor3D/Scene/Light/Light.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Light );
	/*!
	\~english
	\brief		This class defines a CLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CLight accessible depuis COM.
	*/
	class CLight
		: public CComAtlObject< Light, castor3d::Light >
	{
	public:
		COMEX_PROPERTY_GET( Name, BSTR, m_internal, &castor3d::MovableObject::getName );
		COMEX_PROPERTY_GET( Type, eMOVABLE_TYPE, m_internal, &castor3d::MovableObject::getType );
		COMEX_PROPERTY_GET( Scene, IScene *, m_internal, &castor3d::MovableObject::getScene );
		COMEX_PROPERTY_GET( Node, ISceneNode *, m_internal, &castor3d::MovableObject::getParent );
		COMEX_PROPERTY_GET( LightType, eLIGHT_TYPE, m_internal, &castor3d::Light::getLightType );
		COMEX_PROPERTY_GET( DirectionalLight, IDirectionalLight *, m_internal, &castor3d::Light::getDirectionalLight );
		COMEX_PROPERTY_GET( PointLight, IPointLight *, m_internal, &castor3d::Light::getPointLight );
		COMEX_PROPERTY_GET( SpotLight, ISpotLight *, m_internal, &castor3d::Light::getSpotLight );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Light ), CLight );
}

#endif

/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LIGHTGROUP_H__
#define __COMC3D_COM_LIGHTGROUP_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComDirectionalLight.hpp"
#include "ComCastor3D/Castor3D/ComPointLight.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComSpotLight.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( LightGroup, C3DLightGroup );
	/*!
	\~english
	\brief		This class defines a CLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CLight accessible depuis COM.
	*/
	class CLightGroup
		: public CComAtlObjectT< LightGroup, C3DLightGroup >
	{
	public:
		COM_PROPERTY_GET_EX( Name, BSTR, C3DString, c3dLightGroup_getName );
		COM_PROPERTY_GET_MPTR( Scene, IScene, c3dLightGroup_getScene );
		COM_PROPERTY_GET_EX( LightType, eLIGHT_TYPE, C3D_LIGHT_TYPE, c3dLightGroup_getLightType );
		COM_PROPERTY_GET_MPTR( DirectionalLight, IDirectionalLight, c3dLightGroup_getDirectionalLight );
		COM_PROPERTY_GET_MPTR( PointLight, IPointLight, c3dLightGroup_getPointLight );
		COM_PROPERTY_GET_MPTR( SpotLight, ISpotLight, c3dLightGroup_getSpotLight );

		COM_DESTROY( CLightGroup, c3dLightGroup_delete );
		STDMETHOD( Create )( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ eLIGHT_TYPE type )override;
		STDMETHOD( AddLight )( /*[in]*/ ISceneNode * val )override;
		STDMETHOD( RemoveLight )(/*[in]*/ ISceneNode * val )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LightGroup ), CLightGroup );
}

#endif

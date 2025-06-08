/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LIGHT_H__
#define __COMC3D_COM_LIGHT_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComDirectionalLight.hpp"
#include "ComCastor3D/Castor3D/ComPointLight.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"
#include "ComCastor3D/Castor3D/ComSpotLight.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Light, C3DLight );
	/*!
	\~english
	\brief		This class defines a CLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CLight accessible depuis COM.
	*/
	class CLight
		: public CComAtlObjectT< Light, C3DLight >
	{
	public:
		COM_PROPERTY_GET_EX( Name, BSTR, C3DString, c3dLight_getName );
		COM_PROPERTY_GET_MPTR( Scene, IScene, c3dLight_getScene );
		COM_PROPERTY_GET_MPTR( Node, ISceneNode, c3dLight_getNode );
		COM_PROPERTY_GET_EX( LightType, eLIGHT_TYPE, C3D_LIGHT_TYPE, c3dLight_getLightType );
		COM_PROPERTY_GET_MPTR( DirectionalLight, IDirectionalLight, c3dLight_getDirectionalLight );
		COM_PROPERTY_GET_MPTR( PointLight, IPointLight, c3dLight_getPointLight );
		COM_PROPERTY_GET_MPTR( SpotLight, ISpotLight, c3dLight_getSpotLight );

		COM_DESTROY( CLight, c3dLight_delete );
		STDMETHOD( Create )( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ eLIGHT_TYPE type )override;
		STDMETHOD( AttachTo )( /*[in]*/ ISceneNode * val );
		STDMETHOD( Detach )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Light ), CLight );
}

#endif

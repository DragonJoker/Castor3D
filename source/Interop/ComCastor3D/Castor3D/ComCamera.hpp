#ifndef __COMC3D_COM_CAMERA_H__
#define __COMC3D_COM_CAMERA_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Camera, C3DCamera );
	/*!
	\~english
	\brief		This class defines a CCamera object accessible from COM.
	\~french
	\brief		Cette classe définit un CCamera accessible depuis COM.
	*/
	class CCamera
		: public CComAtlObjectT< Camera, C3DCamera >
	{
	public:
		COM_PROPERTY_GET_EX( Name, BSTR, C3DString, c3dCamera_getName );
		COM_PROPERTY_GET_MPTR( Scene, IScene, c3dCamera_getScene );
		COM_PROPERTY_GET_MPTR( Node, ISceneNode, c3dCamera_getNode );
		COM_PROPERTY_EX( ViewportType, eVIEWPORT_TYPE, C3D_VIEWPORT_TYPE, c3dCamera_getViewportType, c3dCamera_setViewportType );
		COM_PROPERTY_GET( Width, UINT, c3dCamera_getWidth );
		COM_PROPERTY_GET( Height, UINT, c3dCamera_getHeight );

		COM_DESTROY( CCamera, c3dCamera_delete );
		STDMETHOD( Create )( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ UINT width, /*[in]*/ UINT height )override;
		STDMETHOD( AttachTo )( /*[in]*/ ISceneNode * val )override;
		STDMETHOD( Detach )()override;
		STDMETHOD( Resize )( /*[in]*/ UINT width, /*[in]*/ UINT height )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Camera ), CCamera );
}

#endif

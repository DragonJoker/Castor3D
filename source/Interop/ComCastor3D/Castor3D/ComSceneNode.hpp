/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SCENE_NODE_H__
#define __COMC3D_COM_SCENE_NODE_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"
#include "ComCastor3D/CastorUtils/ComQuaternion.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( SceneNode, C3DSceneNode );
	/*!
	\~english
	\brief		This class defines a CSceneNode object accessible from COM.
	\~french
	\brief		Cette classe définit un CSceneNode accessible depuis COM.
	*/
	class CSceneNode
		: public CComAtlObjectT< SceneNode, C3DSceneNode >
	{
	public:
		COM_PROPERTY_SPTR( Position, IVector3D, c3dSceneNode_getPosition, c3dSceneNode_setPosition );
		COM_PROPERTY_SPTR( Orientation, IQuaternion, c3dSceneNode_getOrientation, c3dSceneNode_setOrientation );
		COM_PROPERTY_SPTR( Scaling, IVector3D, c3dSceneNode_getScaling, c3dSceneNode_setScaling );
		COM_PROPERTY_GET_MPTR( Parent, ISceneNode, c3dSceneNode_getParent );

		COM_DESTROY( CSceneNode, c3dSceneNode_delete );
		STDMETHOD( AttachTo )( /*[in]*/ ISceneNode * val )override;
		STDMETHOD( Detach )()override;
		STDMETHOD( Yaw )( /*[in]*/ FLOAT val )override;
		STDMETHOD( Pitch )( /*[in]*/ FLOAT val )override;
		STDMETHOD( Roll )( /*[in]*/ FLOAT val )override;
		STDMETHOD( Rotate )( /*[in]*/ IQuaternion * val )override;
		STDMETHOD( Translate )( /*[in]*/ IVector3D * val )override;
		STDMETHOD( Scale )( /*[in]*/ IVector3D * val )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SceneNode ), CSceneNode );
}

#endif

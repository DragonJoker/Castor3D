/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SCENE_NODE_H__
#define __COMC3D_COM_SCENE_NODE_H__

#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"
#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"
#include "ComCastor3D/CastorUtils/ComQuaternion.hpp"

#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, SceneNode );
	/*!
	\~english
	\brief		This class defines a CSceneNode object accessible from COM.
	\~french
	\brief		Cette classe définit un CSceneNode accessible depuis COM.
	*/
	class CSceneNode
		: public CComAtlObject< SceneNode, castor3d::SceneNode >
	{
	public:
		COMEX_PROPERTY( Position, IVector3D *, m_internal, &castor3d::SceneNode::getPosition, &castor3d::SceneNode::setPosition );
		COMEX_PROPERTY( Orientation, IQuaternion *, m_internal, &castor3d::SceneNode::getOrientation, &castor3d::SceneNode::setOrientation );
		COMEX_PROPERTY( Scaling, IVector3D *, m_internal, &castor3d::SceneNode::getScale, &castor3d::SceneNode::setScale );

		STDMETHOD( AttachObject )( /* [in] */ IMovableObject * val );
		STDMETHOD( DetachObject )( /* [in] */ IMovableObject * val );
		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
		STDMETHOD( Yaw )( /* [in] */ IAngle * val );
		STDMETHOD( Pitch )( /* [in] */ IAngle * val );
		STDMETHOD( Roll )( /* [in] */ IAngle * val );
		STDMETHOD( Rotate )( /* [in] */ IQuaternion * val );
		STDMETHOD( Translate )( /* [in] */ IVector3D * val );
		STDMETHOD( Scale )( /* [in] */ IVector3D * val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SceneNode ), CSceneNode );
}

#endif

#ifndef __COMC3D_COM_CAMERA_H__
#define __COMC3D_COM_CAMERA_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"

#include <Castor3D/Scene/Camera.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Camera );
	/*!
	\~english
	\brief		This class defines a CCamera object accessible from COM.
	\~french
	\brief		Cette classe définit un CCamera accessible depuis COM.
	*/
	class CCamera
		: public CComAtlObject< Camera, castor3d::Camera >
	{
	public:
		COMEX_PROPERTY( ViewportType, eVIEWPORT_TYPE, m_internal, &castor3d::Camera::getViewportType, &castor3d::Camera::setViewportType );

		COMEX_PROPERTY_GET( Name, BSTR, m_internal, &castor3d::MovableObject::getName );
		COMEX_PROPERTY_GET( Type, eMOVABLE_TYPE, m_internal, &castor3d::MovableObject::getType );
		COMEX_PROPERTY_GET( Scene, IScene *, m_internal, &castor3d::MovableObject::getScene );
		COMEX_PROPERTY_GET( Node, ISceneNode *, m_internal, &castor3d::MovableObject::getParent );
		COMEX_PROPERTY_GET( Width, unsigned int, m_internal, &castor3d::Camera::getWidth );
		COMEX_PROPERTY_GET( Height, unsigned int, m_internal, &castor3d::Camera::getHeight );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
		STDMETHOD( Resize )( /* [in] */ unsigned int width, /* [in] */ unsigned int height );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Camera ), CCamera );
}

#endif

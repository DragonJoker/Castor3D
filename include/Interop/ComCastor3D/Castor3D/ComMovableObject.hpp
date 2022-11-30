/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MOVABLE_OBJECT_H__
#define __COMC3D_COM_MOVABLE_OBJECT_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include "ComCastor3D/Castor3D/ComScene.hpp"

#include <Castor3D/Scene/MovableObject.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, MovableObject );
	/*!
	\~english
	\brief		This class defines a CMovableObject object accessible from COM.
	\~french
	\brief		Cette classe définit un CMovableObject accessible depuis COM.
	*/
	class CMovableObject
		: public CComAtlObject< MovableObject, castor3d::MovableObject >
	{
	public:
		COM_PROPERTY_GET( Name, BSTR, makeGetter( m_internal, &castor3d::MovableObject::getName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, makeGetter( m_internal, &castor3d::MovableObject::getType ) );
		COM_PROPERTY_GET( Scene, IScene *, makeGetter( m_internal, &castor3d::MovableObject::getScene ) );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( MovableObject ), CMovableObject );
}

#endif

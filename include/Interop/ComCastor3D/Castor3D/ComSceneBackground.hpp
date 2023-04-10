/* See LICENSE file in root folder */
#ifndef __COMC3D_ComSceneBackground_H__
#define __COMC3D_ComSceneBackground_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Scene/Background/Background.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, SceneBackground );
	/*!
	\~english
	\brief		This class defines a CSceneBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CSceneBackground accessible depuis COM.
	*/
	class CSceneBackground
		: public CComAtlObject< SceneBackground, castor3d::SceneBackground >
	{
	public:
		COMEX_PROPERTY_GET( Type, BSTR, m_internal, &castor3d::SceneBackground::getType );
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( SceneBackground ), CSceneBackground );
}

#endif

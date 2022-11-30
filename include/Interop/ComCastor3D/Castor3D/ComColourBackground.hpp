/* See LICENSE file in root folder */
#ifndef __COMC3D_ComColourBackground_H__
#define __COMC3D_ComColourBackground_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Scene/Background/Colour.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, ColourBackground );
	/*!
	\~english
	\brief		This class defines a CColourBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CColourBackground accessible depuis COM.
	*/
	class CColourBackground
		: public CComAtlObject< ColourBackground, castor3d::ColourBackground >
	{
	public:
		COMEX_PROPERTY_GET( Type, BSTR, m_internal, &castor3d::SceneBackground::getType );
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( ColourBackground ), CColourBackground );
}

#endif

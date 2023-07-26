/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_UNIT_H__
#define __COMC3D_COM_TEXTURE_UNIT_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Material/Texture/TextureUnit.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, TextureUnit );
	/*!
	\~english
	\brief		This class defines a CTextureUnit object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextureUnit accessible depuis COM.
	*/
	class CTextureUnit
		: public CComAtlObject< TextureUnit, castor3d::TextureUnit >
	{
	public:
		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureUnit ), CTextureUnit );
}

#endif

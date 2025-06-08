/* See LICENSE file in root folder */
#ifndef __COMC3D_TextOverlay_H__
#define __COMC3D_TextOverlay_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComMaterial.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( TextOverlay, C3DTextOverlay );
	/*!
	\~english
	\brief		This class defines a CTextOverlay object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextOverlay accessible depuis COM.
	*/
	class CTextOverlay
		: public CComAtlObjectT< TextOverlay, C3DTextOverlay >
	{
	public:
		COM_PROPERTY_SPTR( Position, IPosition, c3dTextOverlay_getPosition, c3dTextOverlay_setPosition );
		COM_PROPERTY_SPTR( Size, ISize, c3dTextOverlay_getSize, c3dTextOverlay_setSize );
		COM_PROPERTY_EX( Visible, boolean, bool, c3dTextOverlay_getVisible, c3dTextOverlay_setVisible );
		COM_PROPERTY_MPTR( Material, IMaterial, c3dTextOverlay_getMaterial, c3dTextOverlay_setMaterial );

		COM_PROPERTY_EX( Caption, BSTR, C3DString, c3dTextOverlay_getCaption, c3dTextOverlay_setCaption );
		COM_PROPERTY_EX( Font, BSTR, C3DString, c3dTextOverlay_getFont, c3dTextOverlay_setFont );

		COM_DESTROY( CTextOverlay, c3dTextOverlay_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextOverlay ), CTextOverlay );
}

#endif

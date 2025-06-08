/* See LICENSE file in root folder */
#ifndef __COMC3D_ComTriFaceMapping_H__
#define __COMC3D_ComTriFaceMapping_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( TriFaceMapping, C3DTriFaceMapping );
	/*!
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class CTriFaceMapping
		: public CComAtlObjectT< TriFaceMapping, C3DTriFaceMapping >
	{
	public:
		COM_PROPERTY_GET( FacesCount, UINT, c3dTriFaceMapping_getFacesCount );

		COM_DESTROY( CTriFaceMapping, c3dTriFaceMapping_delete );
		STDMETHOD( AddFace )( /*[in]*/ UINT x, /*[in]*/ UINT y, /*[in]*/ UINT z )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TriFaceMapping ), CTriFaceMapping );
}

#endif

/* See LICENSE file in root folder */
#ifndef __COMC3D_ComTriFaceMapping_H__
#define __COMC3D_ComTriFaceMapping_H__

#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"

#include <Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, TriFaceMapping );
	/*!
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class CTriFaceMapping
		: public CComAtlObject< TriFaceMapping, castor3d::TriFaceMapping >
	{
	public:
		COM_PROPERTY_GET( FacesCount, unsigned int, makeGetter( m_internal, &castor3d::TriFaceMapping::getCount ) );

		STDMETHOD( get_Type )( /* [retval][out] */ eINDEX_MAPPING_TYPE * value );
		STDMETHOD( AddFace )( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TriFaceMapping ), CTriFaceMapping );
}

#endif

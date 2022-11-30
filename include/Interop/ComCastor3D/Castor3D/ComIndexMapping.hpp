/* See LICENSE file in root folder */
#ifndef __COMC3D_ComIndexMapping_H__
#define __COMC3D_ComIndexMapping_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_SPTR( castor3d, IndexMapping );
	/*!
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class ATL_NO_VTABLE CIndexMapping
		: public CComAtlObject< IndexMapping, castor3d::IndexMapping >
	{
	public:
		STDMETHOD( get_Type )( /* [retval][out] */ eINDEX_MAPPING_TYPE * value );
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( IndexMapping ), CIndexMapping );
}

#endif

/* See LICENSE file in root folder */
#ifndef __COMC3D_ComLinesMapping_H__
#define __COMC3D_ComLinesMapping_H__

#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"

#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, LinesMapping );
	/*!
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class CLinesMapping
		: public CComAtlObject< LinesMapping, castor3d::LinesMapping >
	{
	public:
		COM_PROPERTY_GET( LinesCount, UINT, makeGetter( m_internal, &castor3d::LinesMapping::getCount ) );

		STDMETHOD( get_Type )( /* [retval][out] */ eINDEX_MAPPING_TYPE * value );
		STDMETHOD( AddLine )( /* [in] */ unsigned int x, /* [in] */ unsigned int y );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LinesMapping ), CLinesMapping );
}

#endif

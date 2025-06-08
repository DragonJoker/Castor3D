/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SUBMESH_H__
#define __COMC3D_COM_SUBMESH_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComLineMapping.hpp"
#include "ComCastor3D/Castor3D/ComTriFaceMapping.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Submesh, C3DSubmesh );
	/*!
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class CSubmesh
		: public CComAtlObjectT< Submesh, C3DSubmesh >
	{
	public:
		COM_PROPERTY_GET( PointsCount, UINT, c3dSubmesh_getPointsCount );
		COM_PROPERTY_GET_EX( IndexMappingType, eINDEX_MAPPING_TYPE, C3D_INDEX_MAPPING_TYPE, c3dSubmesh_getIndexMappingType );
		COM_PROPERTY_GET_MPTR( TriFaceMapping, ITriFaceMapping, c3dSubmesh_getTriFaceMapping );
		COM_PROPERTY_GET_MPTR( LineMapping, ILineMapping, c3dSubmesh_getLineMapping );

		COM_DESTROY( CSubmesh, c3dSubmesh_delete );
		STDMETHOD( AddPoint )( /*[in]*/ IVector3D * val )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Submesh ), CSubmesh );
}

#endif

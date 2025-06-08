/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MESH_H__
#define __COMC3D_COM_MESH_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Mesh, C3DMesh );
	/*!
	\~english
	\brief		This class defines a CMesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CMesh accessible depuis COM.
	*/
	class CMesh
		: public CComAtlObjectT< Mesh, C3DMesh >
	{
	public:
		COM_PROPERTY_GET( SubmeshCount, UINT, c3dMesh_getSubmeshCount );

		COM_DESTROY( CMesh, c3dMesh_delete );
		STDMETHOD( GetSubmesh )( /*[in]*/ UINT val, /*[out, retval]*/ ISubmesh ** pVal )override;
		STDMETHOD( CreateSubmesh )( /*[out, retval]*/ ISubmesh ** pVal )override;
		STDMETHOD( RemoveSubmesh )( /*[in]*/ ISubmesh * val )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Mesh ), CMesh );
}

#endif

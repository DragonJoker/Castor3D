/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MESH_H__
#define __COMC3D_COM_MESH_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Model/Mesh/Mesh.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_RES( castor3d, Mesh, castor::String );
	/*!
	\~english
	\brief		This class defines a CMesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CMesh accessible depuis COM.
	*/
	class CMesh
		: public CComAtlObject< Mesh, castor3d::Mesh >
	{
	public:
		COMEX_PROPERTY_GET( SubmeshCount, unsigned int, getMesh(), &castor3d::Mesh::getSubmeshCount );
		COMEX_PROPERTY_GET( FaceCount, unsigned int, getMesh(), &castor3d::Mesh::getFaceCount );
		COMEX_PROPERTY_GET( VertexCount, unsigned int, getMesh(), &castor3d::Mesh::getVertexCount );

		STDMETHOD( GetSubmesh )( /* [in] */ unsigned int val, /* [out, retval] */ ISubmesh ** pVal );
		STDMETHOD( CreateSubmesh )( /* [out, retval] */ ISubmesh ** pVal );
		STDMETHOD( DeleteSubmesh )( /* [in] */ ISubmesh * val );

	private:
		castor3d::MeshRPtr getMesh()const
		{
			return getInternal().lock().get();
		}
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Mesh ), CMesh );
}

#endif

/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SUBMESH_H__
#define __COMC3D_COM_SUBMESH_H__

#include "ComCastor3D/Castor3D/ComIndexMapping.hpp"

#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Submesh );
	/*!
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class CSubmesh
		: public CComAtlObject< Submesh, castor3d::Submesh >
	{
	public:
		COMEX_PROPERTY_GET( IndexMapping, IIndexMapping *, m_internal, &castor3d::Submesh::getIndexMapping );
		COMEX_PROPERTY_GET( PointsCount, unsigned int, m_internal, &castor3d::Submesh::getPointsCount );

		STDMETHOD( AddPoint )( /* [in] */ IVector3D * val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Submesh ), CSubmesh );
}

#endif

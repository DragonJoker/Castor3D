/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_GEOMETRY_H__
#define __COMC3D_COM_GEOMETRY_H__

#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComSubmesh.hpp"

#include <Castor3D/Scene/Geometry.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Geometry );
	/*!
	\~english
	\brief		This class defines a CGeometry object accessible from COM.
	\~french
	\brief		Cette classe définit un CGeometry accessible depuis COM.
	*/
	class ATL_NO_VTABLE CGeometry
		: public CComAtlObject< Geometry, castor3d::Geometry >
	{
	public:
		COM_PROPERTY_GET( Mesh, IMesh *, makeGetter( m_internal, &castor3d::Geometry::getMesh ) );
		COM_PROPERTY_PUT( Mesh, IMesh *, makePutter( m_internal, &castor3d::Geometry::setMesh ) );
		//COMEX_PROPERTY( Mesh, IMesh *, m_internal, &castor3d::Geometry::getMesh, &castor3d::Geometry::setMesh );

		COMEX_PROPERTY_GET( Name, BSTR, m_internal, &castor3d::MovableObject::getName );
		COMEX_PROPERTY_GET( Type, eMOVABLE_TYPE, m_internal, &castor3d::MovableObject::getType );
		COMEX_PROPERTY_GET( Scene, IScene *, m_internal, &castor3d::MovableObject::getScene );
		COMEX_PROPERTY_GET( Node, ISceneNode *, m_internal, &castor3d::MovableObject::getParent );

		STDMETHOD( GetMaterial )( /* [in] */ ISubmesh * submesh, /* [out, retval] */ IMaterial ** pVal );
		STDMETHOD( SetMaterial )( /* [in] */ ISubmesh * submesh, /* [in] */ IMaterial * val );
		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Geometry ), CGeometry );
}

#endif

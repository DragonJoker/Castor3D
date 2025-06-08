/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_GEOMETRY_H__
#define __COMC3D_COM_GEOMETRY_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComMesh.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Geometry, C3DGeometry );
	/*!
	\~english
	\brief		This class defines a CGeometry object accessible from COM.
	\~french
	\brief		Cette classe définit un CGeometry accessible depuis COM.
	*/
	class ATL_NO_VTABLE CGeometry
		: public CComAtlObjectT< Geometry, C3DGeometry >
	{
	public:
		COM_PROPERTY_GET_EX( Name, BSTR, C3DString, c3dGeometry_getName );
		COM_PROPERTY_GET_MPTR( Scene, IScene, c3dGeometry_getScene );
		COM_PROPERTY_GET_MPTR( Node, ISceneNode, c3dGeometry_getNode );
		COM_PROPERTY_MPTR( Mesh, IMesh, c3dGeometry_getMesh, c3dGeometry_setMesh );

		COM_DESTROY( CGeometry, c3dGeometry_delete );
		STDMETHOD( Create )( /*[in]*/ IScene * scene, /*[in]*/ BSTR name, /*[in]*/ ISceneNode * parent, /*[in]*/ IMesh * mesh )override;
		STDMETHOD( GetMaterial )( /*[in]*/ ISubmesh * submesh, /*[out, retval]*/ IMaterial ** pVal )override;
		STDMETHOD( SetMaterial )( /*[in]*/ ISubmesh * submesh, /*[in]*/ IMaterial * val )override;
		STDMETHOD( AttachTo )( /*[in]*/ ISceneNode * val )override;
		STDMETHOD( Detach )()override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Geometry ), CGeometry );
}

#endif

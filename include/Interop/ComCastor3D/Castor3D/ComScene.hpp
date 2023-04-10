/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SCENE_H__
#define __COMC3D_COM_SCENE_H__

#include "ComCastor3D/Castor3D/ComSceneBackground.hpp"
#include "ComCastor3D/Castor3D/ComSceneNode.hpp"
#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"

#include <Castor3D/Scene/Scene.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Scene );
	/*!
	\~english
	\brief		This class defines a CScene object accessible from COM.
	\~french
	\brief		Cette classe définit un CScene accessible depuis COM.
	*/
	class CScene
		: public CComAtlObject< Scene, castor3d::Scene >
	{
	public:
		COMEX_PROPERTY( BackgroundColour, IRgbColour *, m_internal, &castor3d::Scene::getBackgroundColour, &castor3d::Scene::setBackgroundColour );
		COMEX_PROPERTY( AmbientLight, IRgbColour *, m_internal, &castor3d::Scene::getAmbientLight, &castor3d::Scene::setAmbientLight );

		COMEX_PROPERTY_GET( Background, ISceneBackground *, m_internal, &castor3d::Scene::getBackground );
		COMEX_PROPERTY_GET( Name, BSTR, m_internal, &castor3d::Scene::getName );
		COMEX_PROPERTY_GET( RootNode, ISceneNode *, m_internal, &castor3d::Scene::getRootNode );
		COMEX_PROPERTY_GET( ObjectRootNode, ISceneNode *, m_internal, &castor3d::Scene::getObjectRootNode );
		COMEX_PROPERTY_GET( CameraRootNode, ISceneNode *, m_internal, &castor3d::Scene::getCameraRootNode );

		STDMETHOD( CreateNode )( /* [in] */ BSTR name, /* [in] */ ISceneNode * parent, /* [out, retval] */ ISceneNode ** pVal );
		STDMETHOD( CreateGeometry )( /* [in] */ BSTR name, /*[in] */ ISceneNode * parent, /* [out, retval] */ IGeometry ** pVal );
		STDMETHOD( CreateCamera )( /* [in] */ BSTR name, /* [in] */ int ww, /* [in] */ int wh, /* [in] */ ISceneNode * node, /* [out, retval] */ ICamera ** pVal );
		STDMETHOD( CreateLight )( /* [in] */ BSTR name, /* [in] */ ISceneNode * node, /* [in] */ eLIGHT_TYPE type, /* [out, retval] */ ILight ** pVal );
		STDMETHOD( CreateMesh )( /* [in] */ BSTR type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal );
		STDMETHOD( GetNode )( /* [in] */ BSTR name, /* [out, retval] */ ISceneNode ** pVal );
		STDMETHOD( GetGeometry )( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal );
		STDMETHOD( GetLight )( /* [in] */ BSTR name, /* [out, retval] */ ILight ** pVal );
		STDMETHOD( GetCamera )( /* [in] */ BSTR name, /* [out, retval] */ ICamera ** pVal );
		STDMETHOD( GetMesh )( /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal );
		STDMETHOD( RemoveLight )( /* [in] */ ILight * light );
		STDMETHOD( RemoveNode )( /* [in] */ ISceneNode * node );
		STDMETHOD( RemoveGeometry )( /* [in] */ IGeometry * geometry );
		STDMETHOD( RemoveCamera )( /* [in] */ ICamera * camera );
		STDMETHOD( RemoveMesh )( /* [in] */ IMesh * mesh );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Scene ), CScene );
}

#endif

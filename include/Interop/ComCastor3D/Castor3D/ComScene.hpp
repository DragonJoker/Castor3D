/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SCENE_H__
#define __COMC3D_COM_SCENE_H__

#include "ComCastor3D/Castor3D/ComSceneNode.hpp"
#include "ComCastor3D/Castor3D/ComSceneBackground.hpp"
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
		COM_PROPERTY( BackgroundColour, IRgbColour *, makeGetter( m_internal, &castor3d::Scene::getBackgroundColour ), makePutter( m_internal, &castor3d::Scene::setBackgroundColour ) );
		COM_PROPERTY( Background, ISceneBackground *, makeGetter( m_internal, &castor3d::Scene::getBackground ), makePutter( m_internal, &castor3d::Scene::setBackground ) );
		COM_PROPERTY( AmbientLight, IRgbColour *, makeGetter( m_internal, &castor3d::Scene::getAmbientLight ), makePutter( m_internal, &castor3d::Scene::setAmbientLight ) );

		COM_PROPERTY_GET( Name, BSTR, makeGetter( m_internal, &castor3d::Scene::getName ) );
		COM_PROPERTY_GET( RootNode, ISceneNode *, makeGetter( m_internal, &castor3d::Scene::getRootNode ) );
		COM_PROPERTY_GET( ObjectRootNode, ISceneNode *, makeGetter( m_internal, &castor3d::Scene::getObjectRootNode ) );
		COM_PROPERTY_GET( CameraRootNode, ISceneNode *, makeGetter( m_internal, &castor3d::Scene::getCameraRootNode ) );

		STDMETHOD( ClearScene )();
		STDMETHOD( CreateSceneNode )( /* [in] */ BSTR name, /* [in] */ ISceneNode * parent, /* [out, retval] */ ISceneNode ** pVal );
		STDMETHOD( CreateGeometry )( /* [in] */ BSTR name, /*[in] */ ISceneNode * parent, /* [out, retval] */ IGeometry ** pVal );
		STDMETHOD( CreateCamera )( /* [in] */ BSTR name, /* [in] */ int ww, /* [in] */ int wh, /* [in] */ ISceneNode * node, /* [out, retval] */ ICamera ** pVal );
		STDMETHOD( CreateLight )( /* [in] */ BSTR name, /* [in] */ ISceneNode * node, /* [in] */ eLIGHT_TYPE type, /* [out, retval] */ ILight ** pVal );
		STDMETHOD( CreateMesh )( /* [in] */ BSTR type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal );
		STDMETHOD( GetNode )( /* [in] */ BSTR name, /* [out, retval] */ ISceneNode ** pVal );
		STDMETHOD( GetGeometry )( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal );
		STDMETHOD( GetLight )( /* [in] */ BSTR name, /* [out, retval] */ ILight ** pVal );
		STDMETHOD( GetCamera )( /* [in] */ BSTR name, /* [out, retval] */ ICamera ** pVal );
		STDMETHOD( RemoveLight )( /* [in] */ ILight * light );
		STDMETHOD( RemoveNode )( /* [in] */ ISceneNode * node );
		STDMETHOD( RemoveGeometry )( /* [in] */ IGeometry * geometry );
		STDMETHOD( RemoveCamera )( /* [in] */ ICamera * camera );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Scene ), CScene );
}

#endif

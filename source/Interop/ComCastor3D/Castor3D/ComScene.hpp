/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SCENE_H__
#define __COMC3D_COM_SCENE_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComSkybox.hpp"
#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Scene, C3DScene );
	/*!
	\~english
	\brief		This class defines a CScene object accessible from COM.
	\~french
	\brief		Cette classe définit un CScene accessible depuis COM.
	*/
	class CScene
		: public CComAtlObjectT< Scene, C3DScene >
	{
	public:
		COM_PROPERTY_SPTR( BackgroundColour, IRgbColour, c3dScene_getBackgroundColour, c3dScene_setBackgroundColour );
		COM_PROPERTY_PUT_EX( BackgroundImage, BSTR, C3DString, c3dScene_setBackgroundImage );
		COM_PROPERTY_GET_EX( Name, BSTR, C3DString, c3dScene_getName );
		COM_PROPERTY_SPTR( AmbientLight, IRgbColour, c3dScene_getAmbientLight, c3dScene_setAmbientLight );
		COM_PROPERTY_PUT_MPTR( BackgroundSkybox, ISkybox, c3dScene_setBackgroundSkybox );
		STDMETHOD( get_RootNode )( /*[out, retval]*/ ISceneNode ** pRet );
		STDMETHOD( get_ObjectRootNode )( /*[out, retval]*/ ISceneNode ** pRet );
		STDMETHOD( get_CameraRootNode )( /*[out, retval]*/ ISceneNode ** pRet );

		COM_DESTROY( CScene, c3dScene_delete );
		STDMETHOD( Create )( /*[in]*/ IEngine * engine, /*[in]*/ BSTR name )override;
		STDMETHOD( AddNode )( /*[in]*/ ISceneNode * val )override;
		STDMETHOD( AddGeometry )( /*[in]*/ IGeometry * val )override;
		STDMETHOD( AddCamera )( /*[in]*/ ICamera * val )override;
		STDMETHOD( AddLight )( /*[in]*/ ILight * val )override;
		STDMETHOD( AddLightGroup )( /*[in]*/ ILightGroup * val )override;
		STDMETHOD( AddMesh )( /*[in]*/ IMesh * val )override;
		STDMETHOD( GetNode )( /*[in]*/ BSTR name, /*[out, retval]*/ ISceneNode ** pVal )override;
		STDMETHOD( GetGeometry )( /*[in]*/ BSTR name, /*[out, retval]*/ IGeometry ** pVal )override;
		STDMETHOD( GetCamera )( /*[in]*/ BSTR name, /*[out, retval]*/ ICamera ** pVal )override;
		STDMETHOD( GetLight )( /*[in]*/ BSTR name, /*[out, retval]*/ ILight ** pVal )override;
		STDMETHOD( GetLightGroup )( /*[in]*/ BSTR name, /*[out, retval]*/ ILightGroup ** pVal )override;
		STDMETHOD( GetMesh )( /*[in]*/ BSTR name, /*[out, retval]*/ IMesh ** pVal )override;
		STDMETHOD( RemoveNode )( /*[in]*/ ISceneNode * node )override;
		STDMETHOD( RemoveGeometry )( /*[in]*/ IGeometry * geometry )override;
		STDMETHOD( RemoveCamera )( /*[in]*/ ICamera * camera )override;
		STDMETHOD( RemoveLight )( /*[in]*/ ILight * light )override;
		STDMETHOD( RemoveLightGroup )( /*[in]*/ ILightGroup * light )override;
		STDMETHOD( RemoveMesh )( /*[in]*/ IMesh * mesh )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Scene ), CScene );
}

#endif

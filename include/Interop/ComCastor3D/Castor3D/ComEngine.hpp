/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_ENGINE_H__
#define __COMC3D_COM_ENGINE_H__

#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <Castor3D/Engine.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Engine );
	/*!
	\~english
	\brief		This class defines a CEngine object accessible from COM.
	\~french
	\brief		Cette classe définit un CEngine accessible depuis COM.
	*/
	class CEngine
		: public CComAtlObject< Engine, castor3d::Engine >
	{
	public:
		COM_PROPERTY_GET( PluginsDirectory, BSTR, makeGetter( &castor3d::Engine::getPluginsDirectory ) );
		COM_PROPERTY_GET( EngineDirectory, BSTR, makeGetter( &castor3d::Engine::getEngineDirectory ) );
		COM_PROPERTY_GET( DataDirectory, BSTR, makeGetter( &castor3d::Engine::getDataDirectory ) );

		STDMETHOD( Create )( /* [in] */ BSTR appName, /* [in] */ boolean enableValidation );
		STDMETHOD( Destroy )();
		STDMETHOD( Initialise )( /* [in] */ int fps, /* [in] */ boolean threaded );
		STDMETHOD( Cleanup )();
		STDMETHOD( CreateScene )( /* [in] */ BSTR name, /* [out, retval] */ IScene ** pVal );
		STDMETHOD( ClearScenes )();
		STDMETHOD( LoadRenderer )( /* [in] */ BSTR type );
		STDMETHOD( RenderOneFrame )();
		STDMETHOD( StartRendering )();
		STDMETHOD( PauseRendering )();
		STDMETHOD( EndRendering )();
		STDMETHOD( LoadPlugin )( /* [in] */ BSTR path );
		STDMETHOD( CreateOverlay )( /* [in] */ eOVERLAY_TYPE type, /* [in] */ BSTR name, /* [in] */ IOverlay * parent, /* [in] */ IScene * scene, /* [out, retval] */ IOverlay ** pVal );
		STDMETHOD( CreateRenderWindow )( /* [in] */ BSTR name, /* [in] */ ISize * size, /* [in] */ LPVOID hWnd, /* [out, retval] */ IRenderWindow ** pVal );
		STDMETHOD( RemoveWindow )( /* [in] */ IRenderWindow * val );
		STDMETHOD( CreateSampler )( /* [in] */ BSTR name, /* [out, retval] */ ISampler ** pVal );
		STDMETHOD( RemoveScene )( /* [in] */ BSTR name );
		STDMETHOD( LoadScene )( /* [in] */ BSTR name, /* [out, retval] */ IRenderTarget ** pTarget );

	private:
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Engine ), CEngine )
}

#endif

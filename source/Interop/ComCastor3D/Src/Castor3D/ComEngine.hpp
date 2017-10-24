/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_ENGINE_H__
#define __COMC3D_COM_ENGINE_H__

#include "ComSize.hpp"

#include <Engine.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CEngine object accessible from COM.
	\~french
	\brief		Cette classe définit un CEngine accessible depuis COM.
	*/
	class ATL_NO_VTABLE CEngine
		:	COM_ATL_OBJECT( Engine )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CEngine();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CEngine();

		inline castor3d::Engine * getInternal()const
		{
			return m_internal;
		}

		COM_PROPERTY_GET( PluginsDirectory, BSTR, make_static_getter( &castor3d::Engine::getPluginsDirectory ) );
		COM_PROPERTY_GET( EngineDirectory, BSTR, make_static_getter( &castor3d::Engine::getEngineDirectory ) );
		COM_PROPERTY_GET( DataDirectory, BSTR, make_static_getter( &castor3d::Engine::getDataDirectory ) );

		STDMETHOD( create )();
		STDMETHOD( Destroy )();
		STDMETHOD( Initialise )( /* [in] */ int fps );
		STDMETHOD( Cleanup )();
		STDMETHOD( CreateScene )( /* [in] */ BSTR name, /* [out, retval] */ IScene ** pVal );
		STDMETHOD( ClearScenes )();
		STDMETHOD( loadRenderer )( /* [in] */ BSTR type );
		STDMETHOD( RenderOneFrame )();
		STDMETHOD( loadPlugin )( /* [in] */ BSTR path );
		STDMETHOD( CreateOverlay )( /* [in] */ eOVERLAY_TYPE type, /* [in] */ BSTR name, /* [in] */ IOverlay * parent, /* [in] */ IScene * scene, /* [out, retval] */ IOverlay ** pVal );
		STDMETHOD( CreateRenderWindow )( /* [in] */ BSTR name, /* [out, retval] */ IRenderWindow ** pVal );
		STDMETHOD( RemoveWindow )( /* [in] */ IRenderWindow * val );
		STDMETHOD( createSampler )( /* [in] */ BSTR name, /* [out, retval] */ ISampler ** pVal );
		STDMETHOD( CreateBlendState )( /* [in] */ BSTR name, /* [out, retval] */ IBlendState ** pVal );
		STDMETHOD( CreateDepthStencilState )( /* [in] */ BSTR name, /* [out, retval] */ IDepthStencilState ** pVal );
		STDMETHOD( CreateMultisampleState )( /* [in] */ BSTR name, /* [out, retval] */ IMultisampleState ** pVal );
		STDMETHOD( CreateRasteriserState )( /* [in] */ BSTR name, /* [out, retval] */ IRasteriserState ** pVal );
		STDMETHOD( RemoveScene )( /* [in] */ BSTR name );
		STDMETHOD( LoadScene )( /* [in] */ BSTR name, /* [out, retval] */ IRenderWindow ** window );

	private:
		castor3d::Engine * m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Engine ), CEngine )
}

#endif

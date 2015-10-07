/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
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
		COMC3D_API CEngine();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CEngine();
		inline Castor3D::Engine * GetInternal()const
		{
			return m_internal;
		}

		COM_PROPERTY_GET( PluginsDirectory, BSTR, make_static_getter( &Castor3D::Engine::GetPluginsDirectory ) );
		COM_PROPERTY_GET( EngineDirectory, BSTR, make_static_getter( &Castor3D::Engine::GetEngineDirectory ) );
		COM_PROPERTY_GET( DataDirectory, BSTR, make_static_getter( &Castor3D::Engine::GetDataDirectory ) );
		
		STDMETHOD( Create )();
		STDMETHOD( Destroy )();
		STDMETHOD( Initialise )( /* [in] */ int fps );
		STDMETHOD( Cleanup )();
		STDMETHOD( CreateScene )( /* [in] */ BSTR name, /* [out, retval] */ IScene ** pVal );
		STDMETHOD( ClearScenes )();
		STDMETHOD( LoadRenderer )( /* [in] */ eRENDERER_TYPE type );
		STDMETHOD( RenderOneFrame )();
		STDMETHOD( LoadPlugin )( /* [in] */ BSTR path );
		STDMETHOD( CreateMesh )( /* [in] */ eMESH_TYPE type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal );
		STDMETHOD( CreateOverlay )( /* [in] */ eOVERLAY_TYPE type, /* [in] */ BSTR name, /* [in] */ IOverlay * parent, /* [in] */ IScene * scene, /* [out, retval] */ IOverlay ** pVal );
		STDMETHOD( CreateRenderWindow )( /* [out, retval] */ IRenderWindow ** pVal );
		STDMETHOD( RemoveWindow )( /* [in] */ IRenderWindow * val );
		STDMETHOD( CreateSampler )( /* [in] */ BSTR name, /* [out, retval] */ ISampler ** pVal );
		STDMETHOD( CreateBlendState )( /* [in] */ BSTR name, /* [out, retval] */ IBlendState ** pVal );
		STDMETHOD( CreateDepthStencilState )( /* [in] */ BSTR name, /* [out, retval] */ IDepthStencilState ** pVal );
		STDMETHOD( CreateRasteriserState )( /* [in] */ BSTR name, /* [out, retval] */ IRasteriserState ** pVal );
		STDMETHOD( RemoveScene )( /* [in] */ BSTR name );
		STDMETHOD( LoadScene )( /* [in] */ BSTR name, /* [out, retval] */ IRenderWindow ** window );

	private:
		Castor3D::Engine * m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Engine ), CEngine )
}

#endif

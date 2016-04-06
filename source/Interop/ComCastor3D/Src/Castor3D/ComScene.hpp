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
#ifndef __COMC3D_COM_SCENE_H__
#define __COMC3D_COM_SCENE_H__

#include "ComSceneNode.hpp"
#include "ComColour.hpp"

#include <Scene.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CScene object accessible from COM.
	\~french
	\brief		Cette classe définit un CScene accessible depuis COM.
	*/
	class ATL_NO_VTABLE CScene
		:	COM_ATL_OBJECT( Scene )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CScene();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CScene();

		inline Castor3D::SceneSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::SceneSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY( BackgroundColour, IColour *, make_getter( m_internal.get(), &Castor3D::Scene::GetBackgroundColour ), make_putter( m_internal.get(), &Castor3D::Scene::SetBackgroundColour ) );
		COM_PROPERTY( Name, BSTR, make_getter( m_internal.get(), &Castor3D::Scene::GetName ), make_putter( m_internal.get(), &Castor3D::Scene::SetName ) );
		COM_PROPERTY( AmbientLight, IColour *, make_getter( m_internal.get(), &Castor3D::Scene::GetAmbientLight ), make_putter( m_internal.get(), &Castor3D::Scene::SetAmbientLight ) );

		COM_PROPERTY_GET( RootNode, ISceneNode *, make_getter( m_internal.get(), &Castor3D::Scene::GetRootNode ) );
		COM_PROPERTY_GET( ObjectRootNode, ISceneNode *, make_getter( m_internal.get(), &Castor3D::Scene::GetObjectRootNode ) );
		COM_PROPERTY_GET( CameraRootNode, ISceneNode *, make_getter( m_internal.get(), &Castor3D::Scene::GetCameraRootNode ) );

		STDMETHOD( ClearScene )();
		STDMETHOD( SetBackgroundImage )( /* [in] */ BSTR path );
		STDMETHOD( CreateSceneNode )( /* [in] */ BSTR name, /* [in] */ ISceneNode * parent, /* [out, retval] */ ISceneNode ** pVal );
		STDMETHOD( CreateGeometry )( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal );
		STDMETHOD( CreateCamera )( /* [in] */ BSTR name, /* [in] */ int ww, /* [in] */ int wh, /* [in] */ ISceneNode * node, /* [out, retval] */ ICamera ** pVal );
		STDMETHOD( CreateLight )( /* [in] */ BSTR name, /* [in] */ ISceneNode * node, /* [in] */ eLIGHT_TYPE type, /* [out, retval] */ ILight ** pVal );
		STDMETHOD( GetNode )( /* [in] */ BSTR name, /* [out, retval] */ ISceneNode ** pVal );
		STDMETHOD( GetGeometry )( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal );
		STDMETHOD( GetLight )( /* [in] */ BSTR name, /* [out, retval] */ ILight ** pVal );
		STDMETHOD( GetCamera )( /* [in] */ BSTR name, /* [out, retval] */ ICamera ** pVal );
		STDMETHOD( RemoveLight )( /* [in] */ ILight * light );
		STDMETHOD( RemoveNode )( /* [in] */ ISceneNode * node );
		STDMETHOD( RemoveGeometry )( /* [in] */ IGeometry * geometry );
		STDMETHOD( RemoveCamera )( /* [in] */ ICamera * camera );
		STDMETHOD( GetBackgroundImage )( /* [out, retval] */ ITexture ** pVal );

	private:
		Castor3D::SceneSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Scene ), CScene );

	DECLARE_VARIABLE_PTR_GETTER( Scene, Castor3D, Scene );
	DECLARE_VARIABLE_PTR_PUTTER( Scene, Castor3D, Scene );
}

#endif

/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __COMC3D_COM_SCENE_H__
#define __COMC3D_COM_SCENE_H__

#include "ComSceneNode.hpp"
#include "ComColour.hpp"

#include <Scene/Scene.hpp>

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
		STDMETHOD( CreateMesh )( /* [in] */ BSTR type, /* [in] */ BSTR name, /* [out, retval] */ IMesh ** pVal );
		STDMETHOD( GetNode )( /* [in] */ BSTR name, /* [out, retval] */ ISceneNode ** pVal );
		STDMETHOD( GetGeometry )( /* [in] */ BSTR name, /* [out, retval] */ IGeometry ** pVal );
		STDMETHOD( GetLight )( /* [in] */ BSTR name, /* [out, retval] */ ILight ** pVal );
		STDMETHOD( GetCamera )( /* [in] */ BSTR name, /* [out, retval] */ ICamera ** pVal );
		STDMETHOD( RemoveLight )( /* [in] */ ILight * light );
		STDMETHOD( RemoveNode )( /* [in] */ ISceneNode * node );
		STDMETHOD( RemoveGeometry )( /* [in] */ IGeometry * geometry );
		STDMETHOD( RemoveCamera )( /* [in] */ ICamera * camera );
		STDMETHOD( GetBackgroundImage )( /* [out, retval] */ ITextureLayout ** pVal );

	private:
		Castor3D::SceneSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Scene ), CScene );

	DECLARE_VARIABLE_PTR_GETTER( Scene, Castor3D, Scene );
	DECLARE_VARIABLE_PTR_PUTTER( Scene, Castor3D, Scene );
}

#endif

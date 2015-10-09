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
#ifndef __COMC3D_COM_CAMERA_H__
#define __COMC3D_COM_CAMERA_H__

#include "ComAtlObject.hpp"
#include "ComScene.hpp"

#include <Camera.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CCamera object accessible from COM.
	\~french
	\brief		Cette classe définit un CCamera accessible depuis COM.
	*/
	class ATL_NO_VTABLE CCamera
		:	COM_ATL_OBJECT( Camera )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CCamera();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CCamera();

		inline Castor3D::CameraSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::CameraSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( PrimitiveType, eTOPOLOGY, make_getter( m_internal.get(), &Castor3D::Camera::GetPrimitiveType ), make_putter( m_internal.get(), &Castor3D::Camera::SetPrimitiveType ) );
		COM_PROPERTY( ViewportType, eVIEWPORT_TYPE, make_getter( m_internal.get(), &Castor3D::Camera::GetViewportType ), make_putter( m_internal.get(), &Castor3D::Camera::SetViewportType ) );

		COM_PROPERTY_GET( Width, unsigned int, make_getter( m_internal.get(), &Castor3D::Camera::GetWidth ) );
		COM_PROPERTY_GET( Height, unsigned int, make_getter( m_internal.get(), &Castor3D::Camera::GetHeight ) );
		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &Castor3D::MovableObject::GetName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, make_getter( m_internal.get(), &Castor3D::MovableObject::GetType ) );
		COM_PROPERTY_GET( Scene, IScene *, make_getter( m_internal.get(), &Castor3D::MovableObject::GetScene ) );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
		STDMETHOD( Resize )( /* [in] */ unsigned int width, /* [in] */ unsigned int height );

	private:
		Castor3D::CameraSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Camera ), CCamera );

	DECLARE_VARIABLE_PTR_GETTER( Camera, Castor3D, Camera );
	DECLARE_VARIABLE_PTR_PUTTER( Camera, Castor3D, Camera );
}

#endif

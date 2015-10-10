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
#ifndef __COMC3D_COM_MOVABLE_OBJECT_H__
#define __COMC3D_COM_MOVABLE_OBJECT_H__

#include "ComAtlObject.hpp"

#include <MovableObject.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMovableObject object accessible from COM.
	\~french
	\brief		Cette classe définit un CMovableObject accessible depuis COM.
	*/
	class ATL_NO_VTABLE CMovableObject
		:	COM_ATL_OBJECT( MovableObject )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CMovableObject();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CMovableObject();

		inline Castor3D::MovableObjectSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::MovableObjectSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &Castor3D::MovableObject::GetName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, make_getter( m_internal.get(), &Castor3D::MovableObject::GetType ) );
		COM_PROPERTY_GET( Scene, IScene *, make_getter( m_internal.get(), &Castor3D::MovableObject::GetScene ) );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();

	private:
		Castor3D::MovableObjectSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( MovableObject ), CMovableObject );

	//DECLARE_VARIABLE_PTR_GETTER( MovableObject, Castor3D, MovableObject );
	//DECLARE_VARIABLE_PTR_PUTTER( MovableObject, Castor3D, MovableObject );
}

#endif

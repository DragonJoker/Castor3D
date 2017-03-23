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
#ifndef __COMC3D_COM_MOVABLE_OBJECT_H__
#define __COMC3D_COM_MOVABLE_OBJECT_H__

#include "ComAtlObject.hpp"

#include <Scene/MovableObject.hpp>

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

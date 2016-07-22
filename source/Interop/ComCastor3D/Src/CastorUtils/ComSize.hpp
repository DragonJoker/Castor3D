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
#ifndef __COMC3D_COM_SIZE_H__
#define __COMC3D_COM_SIZE_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Graphics/Size.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CSize object accessible from COM.
	\~french
	\brief		Cette classe définit un CSize accessible depuis COM.
	*/
	class ATL_NO_VTABLE CSize
		:	COM_ATL_OBJECT( Size )
		,	public Castor::Size
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSize();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSize();

		COM_PROPERTY( Width, UINT, make_getter( this, &Castor::Size::width ), make_putter( this, &Castor::Size::width ) );
		COM_PROPERTY( Height, UINT, make_getter( this, &Castor::Size::height ), make_putter( this, &Castor::Size::height ) );

		STDMETHOD( Set )( /* [in] */ UINT x, /* [in] */ UINT y );
		STDMETHOD( Grow )( /* [in] */ INT x, /* [in] */ INT y );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Size ), CSize );

	DECLARE_VARIABLE_VAL_GETTER( Size, Castor, Size );
	DECLARE_VARIABLE_REF_GETTER( Size, Castor, Size );
	DECLARE_VARIABLE_REF_PUTTER( Size, Castor, Size );
}

#endif

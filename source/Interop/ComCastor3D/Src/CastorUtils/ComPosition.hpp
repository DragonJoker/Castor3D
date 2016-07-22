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
#ifndef __COMC3D_COM_POSITION_H__
#define __COMC3D_COM_POSITION_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Graphics/Position.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CPosition object accessible from COM.
	\~french
	\brief		Cette classe définit un CPosition accessible depuis COM
	*/
	class ATL_NO_VTABLE CPosition
		:	COM_ATL_OBJECT( Position )
		,	public Castor::Position
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CPosition();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CPosition();

		COM_PROPERTY( X, INT, make_getter( this, &Castor::Position::x ), make_putter( this, &Castor::Position::x ) );
		COM_PROPERTY( Y, INT, make_getter( this, &Castor::Position::y ), make_putter( this, &Castor::Position::y ) );

		STDMETHOD( Set )( /* [in] */ INT x, /* [in] */ INT y );
		STDMETHOD( Offset )( /* [in] */ INT x, /* [in] */ INT y );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Position ), CPosition );

	DECLARE_VARIABLE_REF_GETTER( Position, Castor, Position );
	DECLARE_VARIABLE_REF_PUTTER( Position, Castor, Position );
}

#endif

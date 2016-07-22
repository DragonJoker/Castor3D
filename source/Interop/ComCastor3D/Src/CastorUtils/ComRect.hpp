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
#ifndef __COMC3D_COM_RECT_H__
#define __COMC3D_COM_RECT_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Graphics/Rectangle.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CRect object accessible from COM.
	\~french
	\brief		Cette classe définit un CRect accessible depuis COM.
	*/
	class ATL_NO_VTABLE CRect
		:	COM_ATL_OBJECT( Rect )
		,	public Castor::Rectangle
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CRect();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CRect();

		COM_PROPERTY( Left, INT, make_getter( this, &Castor::Rectangle::left ), make_putter( this, &Castor::Rectangle::left ) );
		COM_PROPERTY( Right, INT, make_getter( this, &Castor::Rectangle::right ), make_putter( this, &Castor::Rectangle::right ) );
		COM_PROPERTY( Top, INT, make_getter( this, &Castor::Rectangle::top ), make_putter( this, &Castor::Rectangle::top ) );
		COM_PROPERTY( Bottom, INT, make_getter( this, &Castor::Rectangle::bottom ), make_putter( this, &Castor::Rectangle::bottom ) );

		COM_PROPERTY_GET( Width, INT, make_getter( this, &Castor::Rectangle::width ) );
		COM_PROPERTY_GET( Height, INT, make_getter( this, &Castor::Rectangle::width ) );

		STDMETHOD( Set )( /* [in] */ INT left, /* [in] */ INT top, /* [in] */ INT right, /* [in] */ INT bottom );
		STDMETHOD( IntersectsPosition )( /* [in] */ IPosition * pos, /* [retval][out] */ eINTERSECTION * pVal );
		STDMETHOD( IntersectsRectangle )( /* [in] */ IRect * pos, /* [retval][out] */ eINTERSECTION * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Rect ), CRect );

	DECLARE_VARIABLE_REF_GETTER( Rect, Castor, Rectangle );
	DECLARE_VARIABLE_REF_PUTTER( Rect, Castor, Rectangle );
}

#endif

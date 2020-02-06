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

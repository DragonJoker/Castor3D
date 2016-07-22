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
#ifndef __COMC3D_COM_GLYPH_H__
#define __COMC3D_COM_GLYPH_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComSize.hpp"
#include "ComPosition.hpp"
#include <Graphics/Glyph.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CGlyph object accessible from COM.
	\~french
	\brief		Cette classe définit un CGlyph accessible depuis COM.
	*/
	class ATL_NO_VTABLE CGlyph
		: COM_ATL_OBJECT( Glyph )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CGlyph();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CGlyph();

		void SetInternal( Castor::Glyph * p_glyph )
		{
			m_glyph = p_glyph;
		}

		COM_PROPERTY_GET( Size, ISize *, make_getter( m_glyph, &Castor::Glyph::GetSize ) );
		COM_PROPERTY_GET( Bearing, IPosition *, make_getter( m_glyph, &Castor::Glyph::GetBearing ) );
		COM_PROPERTY_GET( Advance, INT, make_getter( m_glyph, &Castor::Glyph::GetAdvance ) );

	private:
		Castor::Glyph * m_glyph;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Glyph ), CGlyph )
}

#endif

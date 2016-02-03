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
#ifndef ___C3D_BUFFER_ELEMENT_DECLARATION_H___
#define ___C3D_BUFFER_ELEMENT_DECLARATION_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Buffer element description.
	\remark		Describes usage and type of an element in a vertex buffer.
	\~french
	\brief		Description d'un élément de tampon.
	\remark		Décrit l'utilisation et le type d'un élément de tampon de sommets.
	*/
	struct BufferElementDeclaration
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		BufferElementDeclaration()
			: m_dataType()
			, m_offset()
			, m_name()
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name		The associated variable name.
		 *\param[in]	p_type		Element type.
		 *\param[in]	p_offset	Offset in the stream.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name		Le nom de la variable associée.
		 *\param[in]	p_type		Type de l'élément.
		 *\param[in]	p_offset	Offset dans le tampon.
		 */
		BufferElementDeclaration( Castor::String const & p_name, eELEMENT_TYPE p_type, uint32_t p_offset = 0 )
			: m_dataType( p_type )
			, m_offset( p_offset )
			, m_name( p_name )
		{
		}

		//!\~english The associated variable name.	\~french Le nom de la variable associée.
		Castor::String m_name;
		//!\~english Element type.	\~french Type de l'élément.
		eELEMENT_TYPE m_dataType;
		//!\~english Offset in buffer.	\~french Offset dans le tampon.
		uint32_t m_offset;
	};
}

#endif

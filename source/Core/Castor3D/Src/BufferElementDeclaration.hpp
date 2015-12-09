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
	\brief		Buffer element description
	\remark		Describes usage and type of an element in a vertex buffer
	\~french
	\brief		Description d'un élément de tampon
	\remark		Décrit l'utilisation et le type d'un élément de tampon de sommets
	*/
	struct BufferElementDeclaration
	{
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		BufferElementDeclaration()
			:	m_uiStream()
			,	m_eUsage()
			,	m_eDataType()
			,	m_offset()
		{
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiStream	Stream index
		 *\param[in]	p_eUsage	Element usage
		 *\param[in]	p_type		Element type
		 *\param[in]	p_offset	Offset in the stream
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiStream	Index du flux
		 *\param[in]	p_eUsage	Utilisation de l'élément
		 *\param[in]	p_type		Type de l'élément
		 *\param[in]	p_offset	Offset dans le tampon
		 */
		BufferElementDeclaration( uint32_t p_uiStream, eELEMENT_USAGE p_eUsage, eELEMENT_TYPE p_type, uint32_t p_offset = 0 )
			:	m_uiStream( p_uiStream )
			,	m_eUsage( p_eUsage )
			,	m_eDataType( p_type )
			,	m_offset( p_offset )
		{
		}
		//!\~english Stream index	\~french Index du flux
		uint32_t m_uiStream;
		//!\~english Element usage	\~french Utilisation de l'élément
		eELEMENT_USAGE m_eUsage;
		//!\~english Element type	\~french Type de l'élément
		eELEMENT_TYPE m_eDataType;
		//!\~english Offset in stream	\~french Offset dans le tampon
		uint32_t m_offset;
	};
}

#endif

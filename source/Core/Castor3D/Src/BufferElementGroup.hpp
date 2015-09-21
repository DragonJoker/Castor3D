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
#ifndef ___C3D_BUFFER_ELEMENT_GROUP_H___
#define ___C3D_BUFFER_ELEMENT_GROUP_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Holds data of a vertex buffer elements group
	\~french
	\brief		Contient les données d'un groupe d'éléments de tampon de sommets
	*/
	class C3D_API BufferElementGroup
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pBuffer		The data buffer
		 *\param[in]	p_index		The group index
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pBuffer		Le tampon de données
		 *\param[in]	p_index		L'indice du groupe
		 */
		BufferElementGroup( uint8_t * p_pBuffer = NULL, uint32_t p_index = 0 );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BufferElementGroup();
		/**
		 *\~english
		 *\brief		Links the element values to the ones in parameter.
		 *\~french
		 *\brief		Lie les valeurs de l'élément à celles données en paramètre.
		 */
		void LinkCoords( uint8_t * p_pBuffer );
		/**
		 *\~english
		 *\brief		Links the element values to the ones in parameter.
		 *\remark		Replace buffer values by element values
		 *\param[in]	p_pBuffer	The values buffer
		 *\~french
		 *\brief		Lie les valeurs de l'élément à celles données en paramètre.
		 *\remark		Remplace les valeurs du tampon par celles de l'élément
		 *\param[in]	p_pBuffer	Le tampon de valeurs
		 */
		void LinkCoords( uint8_t * p_pBuffer, uint32_t p_uiSize );
		/**
		 *\~english
		 *\brief		Retrieves a pointer on the data buffer
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur le tampon de données
		 *\return		Le pointeur
		 */
		inline uint8_t * ptr()
		{
			return m_pBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer on the data buffer
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur le tampon de données
		 *\return		Le pointeur
		 */
		inline uint8_t const * const_ptr()const
		{
			return m_pBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the group index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'indice du groupe
		 *\return		La valeur
		 */
		inline uint32_t GetIndex()const
		{
			return m_uiIndex;
		}
		/**
		 *\~english
		 *\brief		Sets the group index
		 *\param[in]	p_index	The new value
		 *\~french
		 *\brief		Définit l'indice du groupe
		 *\param[in]	p_index	La nouvelle valeur
		 */
		inline void SetIndex( uint32_t p_index )
		{
			m_uiIndex = p_index;
		}

	protected:
		//!\~english The elements values	\~french Les valeurs des éléments
		uint8_t * m_pBuffer;
		//!\~english The group index	\~french L'indice du groupe
		uint32_t m_uiIndex;
	};
}

#endif

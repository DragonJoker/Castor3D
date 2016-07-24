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
	class BufferElementGroup
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_buffer		The data buffer
		 *\param[in]	p_index		The group index
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_buffer		Le tampon de données
		 *\param[in]	p_index		L'indice du groupe
		 */
		C3D_API BufferElementGroup( uint8_t * p_buffer = nullptr, uint32_t p_index = 0 );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~BufferElementGroup();
		/**
		 *\~english
		 *\brief		Links the element values to the ones in parameter.
		 *\~french
		 *\brief		Lie les valeurs de l'élément à celles données en paramètre.
		 */
		C3D_API void LinkCoords( uint8_t * p_buffer );
		/**
		 *\~english
		 *\brief		Links the element values to the ones in parameter.
		 *\remarks		Replace buffer values by element values
		 *\param[in]	p_buffer	The values buffer
		 *\param[in]	p_stride	The element binay size
		 *\~french
		 *\brief		Lie les valeurs de l'élément à celles données en paramètre.
		 *\remarks		Remplace les valeurs du tampon par celles de l'élément
		 *\param[in]	p_buffer	Le tampon de valeurs
		 *\param[in]	p_stride	La taille binaire de l'élément
		 */
		C3D_API void LinkCoords( uint8_t * p_buffer, uint32_t p_stride );
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
			return m_buffer;
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
			return m_buffer;
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
			return m_index;
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
			m_index = p_index;
		}

	protected:
		//!\~english The elements values	\~french Les valeurs des éléments
		uint8_t * m_buffer;
		//!\~english The group index	\~french L'indice du groupe
		uint32_t m_index;
	};
}

#endif

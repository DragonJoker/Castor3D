/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_BUFFER_DECLARATION_H___
#define ___C3D_BUFFER_DECLARATION_H___

#include "Castor3DPrerequisites.hpp"
#include "BufferElementDeclaration.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Description of all elements in a vertex buffer
	\~french
	\brief		Description de tous les éléments dans un tampon de sommets
	*/
	class BufferDeclaration
	{
	public:
		DECLARE_VECTOR( BufferElementDeclaration, BufferElementDeclaration );
		using iterator = BufferElementDeclarationArrayIt;
		using const_iterator = BufferElementDeclarationArrayConstIt;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API BufferDeclaration();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_elements	The elements array
		 *\param[in]	p_count		Elements array size
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_elements	Tableau d'éléments
		 *\param[in]	p_count		Taille du tableau d'éléments
		 */
		C3D_API BufferDeclaration( BufferElementDeclaration const * p_elements, uint32_t p_count );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_elements	The elements array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_elements	Tableau des éléments
		 */
		template< uint32_t N >
		inline explicit BufferDeclaration( BufferElementDeclaration const( & p_elements )[N] )
			: BufferDeclaration( p_elements, N )
		{
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_elements		The elements array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_elements		Tableau d'éléments
		 */
		inline explicit BufferDeclaration( std::vector< BufferElementDeclaration > const & p_elements )
			: BufferDeclaration( p_elements.data(), uint32_t( p_elements.size() ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~BufferDeclaration();
		/**
		 *\~english
		 *\brief		Retrieves the elements count
		 *\return		The elements count
		 *\~french
		 *\brief		Récupère le compte des éléments
		 *\return		Le compte des éléments
		 */
		inline uint32_t GetSize()const
		{
			return uint32_t( m_arrayElements.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the total elements byte count
		 *\return		The byte count
		 *\~french
		 *\brief		Récupère la taille totale en octet des éléments
		 *\return		La taille en octets
		 */
		inline uint32_t GetStride()const
		{
			return m_uiStride;
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments.
		 */
		inline BufferElementDeclarationArray::iterator begin()
		{
			return m_arrayElements.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments.
		 */
		inline BufferElementDeclarationArray::const_iterator begin()const
		{
			return m_arrayElements.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline BufferElementDeclarationArray::iterator end()
		{
			return m_arrayElements.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline BufferElementDeclarationArray::const_iterator end()const
		{
			return m_arrayElements.end();
		}

	private:
		C3D_API void DoInitialise( BufferElementDeclaration const * p_elements, uint32_t p_count );

	protected:
		//!\~english Element description array	\~french Tableau de descriptions d'éléments
		BufferElementDeclarationArray m_arrayElements;
		//!\~english Byte size of the element	\~french Taille de l'élément, en octets
		uint32_t m_uiStride;
	};
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	C3D_API bool operator==( BufferDeclaration & p_lhs, BufferDeclaration & p_rhs );
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	C3D_API bool operator!=( BufferDeclaration & p_lhs, BufferDeclaration & p_rhs );
}

#endif

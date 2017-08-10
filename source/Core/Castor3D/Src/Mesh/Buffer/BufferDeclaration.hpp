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
#ifndef ___C3D_BUFFER_DECLARATION_H___
#define ___C3D_BUFFER_DECLARATION_H___

#include "Castor3DPrerequisites.hpp"
#include "BufferElementDeclaration.hpp"

namespace castor3d
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
		inline uint32_t size()const
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
		inline uint32_t stride()const
		{
			return m_stride;
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
		/**
		 *\~english
		 *\brief		adds an element to the end of the list.
		 *\param[in]	p_element	The element.
		 *\~french
		 *\brief		Ajoute un élément à la fin de la liste.
		 *\param[in]	p_element	L'élément.
		 */
		inline void push_back( BufferElementDeclaration const & p_element )
		{
			m_arrayElements.push_back( p_element );
			m_stride += castor3d::getSize( p_element.m_dataType );
		}

	private:
		C3D_API void doInitialise( BufferElementDeclaration const * p_elements, uint32_t p_count );

	protected:
		//!\~english Element description array	\~french Tableau de descriptions d'éléments
		BufferElementDeclarationArray m_arrayElements;
		//!\~english Byte size of the element	\~french Taille de l'élément, en octets
		uint32_t m_stride;
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

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
#ifndef ___C3D_TRANSFORM_BUFFER_DECLARATION_H___
#define ___C3D_TRANSFORM_BUFFER_DECLARATION_H___

#include "Castor3DPrerequisites.hpp"
#include "TransformBufferElementDeclaration.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		20/10/2016
	\~english
	\brief		Description of all elements in a transform feedback buffer
	\~french
	\brief		Description de tous les éléments dans un tampon de transform feedback.
	*/
	class TransformBufferDeclaration
	{
	public:
		DECLARE_VECTOR( TransformBufferElementDeclaration, TransformBufferElementDeclaration );
		using iterator = TransformBufferElementDeclarationArrayIt;
		using const_iterator = TransformBufferElementDeclarationArrayConstIt;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API TransformBufferDeclaration();
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
		C3D_API TransformBufferDeclaration( TransformBufferElementDeclaration const * p_elements, uint32_t p_count );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_elements	The elements array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_elements	Tableau des éléments
		 */
		template< uint32_t N >
		inline explicit TransformBufferDeclaration( TransformBufferElementDeclaration const( & p_elements )[N] )
			: TransformBufferDeclaration( p_elements, N )
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
		inline explicit TransformBufferDeclaration( std::vector< TransformBufferElementDeclaration > const & p_elements )
			: TransformBufferDeclaration( p_elements.data(), uint32_t( p_elements.size() ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TransformBufferDeclaration();
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
		inline iterator begin()
		{
			return m_arrayElements.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments.
		 */
		inline const_iterator begin()const
		{
			return m_arrayElements.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline iterator end()
		{
			return m_arrayElements.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline const_iterator end()const
		{
			return m_arrayElements.end();
		}

	private:
		C3D_API void DoInitialise( TransformBufferElementDeclaration const * p_elements, uint32_t p_count );

	protected:
		//!\~english Element description array	\~french Tableau de descriptions d'éléments
		TransformBufferElementDeclarationArray m_arrayElements;
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
	C3D_API bool operator==( TransformBufferDeclaration & p_lhs, TransformBufferDeclaration & p_rhs );
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	C3D_API bool operator!=( TransformBufferDeclaration & p_lhs, TransformBufferDeclaration & p_rhs );
}

#endif

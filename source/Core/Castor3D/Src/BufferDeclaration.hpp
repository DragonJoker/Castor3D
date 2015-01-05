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
	class C3D_API BufferDeclaration
	{
	public:
		DECLARE_VECTOR( BufferElementDeclaration, BufferElementDeclaration );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_elements	The elements array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_elements	Tableau des éléments
		 */
		template< uint32_t N >
		BufferDeclaration( BufferElementDeclaration const( & p_elements )[N] )
		{
			DoInitialise( p_elements, N );
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pElements		The elements array
		 *\param[in]	p_uiNbElements	Elements array size
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pElements		Tableau d'éléments
		 *\param[in]	p_uiNbElements	Taille du tableau d'éléments
		 */
		BufferDeclaration( BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_declaration	The BufferDeclaration object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_declaration	L'objet BufferDeclaration à copier
		 */
		BufferDeclaration( BufferDeclaration const & p_declaration );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_declaration	The BufferDeclaration object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_declaration	L'objet BufferDeclaration à déplacer
		 */
		BufferDeclaration( BufferDeclaration && p_declaration );
		/**
		 *\~english
		 *\brief		Assignment operator
		 *\param[in]	p_declaration	The BufferDeclaration object to copy
		 *\return		A reference to this6.5.0 BufferDeclaration object
		 *\~french
		 *\brief		Opérateur d'affectation
		 *\param[in]	p_declaration	L'objet BufferDeclaration à copier
		 *\return		Une référence sur cet objet BufferDeclaration
		 */
		BufferDeclaration & operator =( BufferDeclaration p_declaration );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BufferDeclaration();
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the elements array
		 *\return		Constant iterator to the begin of the elements array
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau d'éléments
		 *\return		Itérateur constant sur le début du tableau d'éléments
		 */
		inline BufferElementDeclarationArrayConstIt	Begin()const
		{
			return m_arrayElements.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the elements array
		 *\return		Constant iterator to the end of the elements array
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'éléments
		 *\return		Itérateur constant sur la fin du tableau d'éléments
		 */
		inline BufferElementDeclarationArrayConstIt	End()const
		{
			return m_arrayElements.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the elements count
		 *\return		The elements count
		 *\~french
		 *\brief		Récupère le compte des éléments
		 *\return		Le compte des éléments
		 */
		inline uint32_t Size()const
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
		 *\brief		Array accessor
		 *\remark		That function doesn't check if index is out of bound so expect crashes if used incorrectly
		 *\param[in]	p_uiIndex	The index
		 *\return		The element at given index
		 *\~french
		 *\brief		Accesseur de type tableau
		 *\remark		Cette fonction ne vérifie pas la validité de l'index donné, donc attention aux crashes si mal utilisée
		 *\param[in]	p_uiIndex	L'index
		 *\return		L'élément à l'index donné
		 */
		inline BufferElementDeclaration const & operator []( uint32_t p_uiIndex )const
		{
			return m_arrayElements[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Array accessor
		 *\remark		That function doesn't check if index is out of bound so expect crashes if used incorrectly
		 *\param[in]	p_uiIndex	The index
		 *\return		The element at given index
		 *\~french
		 *\brief		Accesseur de type tableau
		 *\remark		Cette fonction ne vérifie pas la validité de l'index donné, donc attention aux crashes si mal utilisée
		 *\param[in]	p_uiIndex	L'index
		 *\return		L'élément à l'index donné
		 */
		inline BufferElementDeclaration & operator []( uint32_t p_uiIndex )
		{
			return m_arrayElements[p_uiIndex];
		}

	private:
		void DoInitialise( BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements );

	protected:
		friend void swap( BufferDeclaration & p_obj1, BufferDeclaration & p_obj2 );
		//!\~english Element description array	\~french Tableau de descriptions d'éléments
		BufferElementDeclarationArray m_arrayElements;
		//!\~english Byte size of the element	\~french Taille de l'élément, en octets
		uint32_t m_uiStride;
	};
	/**
	 *\~english
	 *\brief		Swapping function
	 *\param[in]	p_obj1, p_obj2	The 2 objects to swap
	 *\~french
	 *\brief		Fonction d'échange
	 *\param[in]	p_obj1, p_obj2	Les 2 objets à échanger
	 */
	void swap( BufferDeclaration & p_obj1, BufferDeclaration & p_obj2 );
}

#endif

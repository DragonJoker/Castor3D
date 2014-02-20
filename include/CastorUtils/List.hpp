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
#ifndef ___Castor_List___
#define ___Castor_List___

#include "SmartPtr.hpp"
#include <list>

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		std::list helper functions
	\~french
	\brief		Fonctions d'aide pour std::list
	*/
	class list
	{
	public:
		/**
		 *\~english
		 *\brief		Tells if the list contains a given element
		 *\remark		Uses std::find
		 *\param[in]	p_list		The list
		 *\param[in]	p_tValue	The value to look for
		 *\return		\p true if p_tValue is in p_list, \p false if not
		 *\~french
		 *\brief		Dit si la liste contient l'élément donné
		 *\remark		Utilise std::find
		 *\param[in]	p_list		La liste
		 *\param[in]	p_tValue	La valeur à rechercher
		 *\return		\p true si p_tValue est dans p_list, \p false sinon
		 */
		template <typename T>
		static inline bool has( std::list< T > const & p_list, T const & p_tValue)
		{
			return (std::find( p_list.begin(), p_list.end(), p_tValue ) != p_list.end());
		}
		/**
		 *\~english
		 *\brief		Gets the value at given index from a list
		 *\param[in]	p_list		The list
		 *\param[in]	p_uiIndex	The value index
		 *\return		A pointer to the found value, \p nullptr if not found
		 *\~french
		 *\brief		Dit si la liste contient l'élément donné
		 *\param[in]	p_list		La liste
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\return		Un pointeur sur la valeur trouvée, \p nullptr si non-trouvée
		 */
		template <typename T>
		static inline T * get_or_null( std::list< T * > const & p_list, const std::size_t p_uiIndex )
		{
			T * pReturn = nullptr;

			if( p_uiIndex < p_list.size() )
			{
				typename std::list< T * >::const_iterator it = p_list.begin();
				typename std::list< T * >::const_iterator iend = p_list.end();
				std::size_t iCount = 0;

				for(; it != iend && iCount < p_uiIndex; ++it, ++iCount );

				pReturn = *it;
			}

			return pReturn;
		}
		/**
		 *\~english
		 *\brief		Searches an element in the list returns it's iterator
		 *\param[in]	p_list	The list
		 *\param[in]	tKey	The value
		 *\return		An iterator to the found value, \p p_list.end() if not found
		 *\~french
		 *\brief		Cherche un élément dans une liste et retourne son itérateur
		 *\param[in]	p_list	La liste
		 *\param[in]	tKey	La valeur
		 *\return		Un itérateur sur la valeur trouvée, \p p_list.end() si non-trouvé
		 */
		template< class T >
		typename std::list< T >::iterator find( std::list< T > const & p_list, T const & tKey )
		{
			bool bFound = false;
			typename std::list< T >::const_iterator it = p_list.begin();

			while( it != p_list.end() && ! bFound )
			{
				if( *it == tKey )
				{
					bFound = true;
				}
				else
				{
					it++;
				}
			}

			return it;
		}
		/**
		 *\~english
		 *\brief		Searches an element in the list returns it's reverse iterator
		 *\param[in]	p_list	The list
		 *\param[in]	tKey	The value
		 *\return		An iterator to the found value, \p p_list.rend() if not found
		 *\~french
		 *\brief		Cherche un élément dans une liste et retourne son itérateur inversé
		 *\param[in]	p_list	La liste
		 *\param[in]	tKey	La valeur
		 *\return		Un itérateur sur la valeur trouvée, \p p_list.rend() si non-trouvé
		 */
		template< class T >
		typename std::list< T >::reverse_iterator rfind( std::list< T > & p_list, T const & tKey)
		{
			return typename std::list< T >::reverse_iterator( find( p_list, tKey));
		}
		/**
		 *\~english
		 *\brief		Searches an element in the list returns it's reverse iterator
		 *\param[in]	p_list	The list
		 *\param[in]	tKey	The value
		 *\return		An iterator to the found value, \p p_list.rend() if not found
		 *\~french
		 *\brief		Cherche un élément dans une liste et retourne son itérateur inversé
		 *\param[in]	p_list	La liste
		 *\param[in]	tKey	La valeur
		 *\return		Un itérateur sur la valeur trouvée, \p p_list.rend() si non-trouvé
		 */
		template< class T >
		typename std::list< T >::const_reverse_iterator rfind( std::list< T > const & p_list, T const & tKey)
		{
			return typename std::list< T >::const_reverse_iterator( find( p_list, tKey));
		}
		/**
		 *\~english
		 *\brief		Erases the given value from the list
		 *\remark		Uses std::remove
		 *\param[in]	p_list	The list
		 *\param[in]	p_tKey		The value to look for
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la valeur donnée de la liste
		 *\remark		Utilise std::remove
		 *\param[in]	p_list	La liste
		 *\param[in]	p_tKey		La valeur à rechercher
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_at( std::list< T > & p_list, T const & p_tKey)
		{
			bool l_bReturn = false;
			std::size_t i = 0;
			typename std::list< T >::iterator it = p_list.begin();
			typename std::list< T >::const_iterator iend = p_list.end();

			while (it < iend && ! l_bReturn)
			{
				if ( * it == p_tKey)
				{
					p_list.erase( it);
					l_bReturn = true;
				}
				else
				{
					++it;
				}
			}

			return l_bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the value at the given index from the list
		 *\param[in]	p_list	The list
		 *\param[in]	p_uiIndex	The value index
		 *\param[out]	p_tResult	Receives the value removed from the list
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la valeur à l'index donné de la liste
		 *\param[in]	p_list	La liste
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\param[out]	p_tResult	Reçoit la valeur enlevée de la liste
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_at( std::list< T > & p_list, std::size_t p_uiIndex, T & p_tResult)
		{
			bool bReturn = false;

			if (p_uiIndex < p_list.size())
			{
				std::size_t i = 0;
				typename std::list< T >::iterator it = p_list.begin();
				typename std::list< T >::const_iterator iend = p_list.end();

				for (; it != iend && i < p_uiIndex; ++it, ++i);
				p_tResult = ( * it);
				p_list.erase( it);
				bReturn = true;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the first value from the list
		 *\param[in]	p_list	The list
		 *\param[out]	p_tResult	Receives the value removed from the list
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la première valeur de la liste
		 *\param[in]	p_list	La liste
		 *\param[out]	p_tResult	Reçoit la valeur enlevée de la liste
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_first( std::list< T > & p_list, T & p_tResult)
		{
			bool bReturn = false;

			if (p_list.size() > 0)
			{
				bReturn = true;
				p_tResult = * p_list.begin();
				p_list.erase( p_list.begin());
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the last value from the list
		 *\param[in]	p_list	The list
		 *\param[out]	p_tResult	Receives the value removed from the list
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la dernière valeur de la liste
		 *\param[in]	p_list	La liste
		 *\param[out]	p_tResult	Reçoit la valeur enlevée de la liste
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_last( std::list< T > & p_list, T & p_tResult)
		{
			bool bReturn = false;

			if (p_list.size() > 0)
			{
				bReturn = true;
				typename std::list< T >::reverse_iterator i = p_list.rbegin();
				p_tResult = * i;
				i++;
				p_list.erase( i.base());
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the given value from the list
		 *\remark		Uses list::erase_at
		 *\param[in]	p_list	The list
		 *\param[in]	p_pKey	The value to look for
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la valeur donnée de la liste
		 *\remark		Utilise list::erase_at
		 *\param[in]	p_list	La liste
		 *\param[in]	p_pKey	La valeur à rechercher
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_at( std::list< T * > & p_list, T * p_pKey)
		{
			bool bReturn = erase_at( p_list, p_pKey);

			if (bReturn)
			{
				delete p_pKey;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the value at the given index from the list
		 *\remark		Uses list::erase_at
		 *\param[in]	p_list	The list
		 *\param[in]	p_uiIndex	The value index
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la valeur à l'index donné de la liste
		 *\remark		Uses list::erase_at
		 *\param[in]	p_list	La liste
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_at( std::list< T * > & p_list, std::size_t p_uiIndex)
		{
			T * pResult;
			bool bReturn = erase_at( p_list, p_uiIndex, pResult);

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the first value from the list
		 *\remark		Uses list::erase_first
		 *\param[in]	p_list	The list
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la première valeur de la liste
		 *\remark		Uses list::delete_at
		 *\param[in]	p_list	La liste
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_first( std::list< T * > & p_list)
		{
			T * pResult;
			bool bReturn = erase_first( p_list, pResult);

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the last value from the list
		 *\remark		Uses list::erase_last
		 *\param[in]	p_list	The list
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la dernière valeur de la liste
		 *\remark		Uses list::delete_at
		 *\param[in]	p_list	La liste
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_last( std::list< T * > & p_list)
		{
			T * pResult;
			bool bReturn = erase_last( p_list, pResult);

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
	};
}

#endif

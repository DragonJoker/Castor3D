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
#ifndef ___Castor_Set___
#define ___Castor_Set___

#include "SmartPtr.hpp"
#include <set>

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		18/10/2011
	\~english
	\brief		std::set helper functions
	\~french
	\brief		Fonctions d'aide pour std::set
	*/
	class set
	{
	public:
		/**
		 *\~english
		 *\brief		Tells if the element at the given key exists in the given set
		 *\param[in]	p_set	The set containing elements
		 *\param[in]	p_key	The element to find
		 *\return		\p true if found
		 *\~french
		 *\brief		Dit l'élément à la clef donnée existe dans le set donné
		 *\param[in]	p_set	Le set contenant des éléments
		 *\param[in]	p_key	L'élément à trouver
		 *\return		\p true si trouvé
		 */
		template <typename T>
		static inline bool has( std::set< T > const & p_set, T const & p_key)
		{
			return p_set.find( p_key) != p_set.end();
		}
		/**
		 *\~english
		 *\brief		Gives the element in a given set at a given key
		 *\param[in]	p_set	The set containing elements
		 *\param[in]	p_key	The element to find
		 *\return		The element if found, \p NULL if not
		 *\~french
		 *\brief		Donne l'élément dans le set donné  correspondant à la clef donnée
		 *\param[in]	p_set	Le set contenant des éléments
		 *\param[in]	p_key	L'élément à trouver
		 *\return		L'élément trouvé, \p NULL si non trouvé
		 */
		template <typename T>
		static inline T * find_or_null( std::set< T * > const & p_set, T const & p_key)
		{
			typename std::set< T * >::const_iterator ifind = p_set.find( &p_key );
			T * l_pReturn = nullptr;

			if (ifind != p_set.end())
			{
				l_pReturn = *ifind;
			}

			return l_pReturn;
		}
		/**
		 *\~english
		 *\brief		Gives the element in a given set at a given key
		 *\param[in]	p_set	The set containing elements
		 *\param[in]	p_key	The element to find
		 *\return		The element if found, default value if not
		 *\~french
		 *\brief		Donne l'élément dans le set donné correspondant à la clef donnée
		 *\param[in]	p_set	Le set contenant des éléments
		 *\param[in]	p_key	L'élément à trouver
		 *\return		L'élément trouvé, valeur par défaut si non trouvé
		 */
		template <typename T>
		static inline T find_or_null( std::set< T > const & p_set, T const & p_key)
		{
			typename std::set< T >::const_iterator ifind = p_set.find( p_key);
			T l_pReturn;

			if (ifind != p_set.end())
			{
				l_pReturn = *ifind;
			}

			return l_pReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the element at the given key, doesn't delete the data
		 *\param[in]	p_set	The set to modify
		 *\param[in]	p_key	The key of the element to remove from the set
		 *\return		\p true if the element was in the set
		 *\~french
		 *\brief		Enlève l'élément à la clef donnée, ne le désalloue pas
		 *\param[in]	p_set	Le set à modifier
		 *\param[in]	p_key	La clef de l'élément à enlever
		 *\return		\p true si l'élément était dans le set
		 */
		template <typename T>
		static inline bool erase_at( std::set< T > & p_set, T const & p_key)
		{
			bool bReturn = false;
			typename std::set< T >::iterator ifind = p_set.find( p_key);

			if (ifind != p_set.end())
			{
				bReturn = true;
				p_set.erase( ifind);
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the value at the given index from the set
		 *\param[in]	p_set	The set
		 *\param[in]	p_where	The iterator on the value
		 *\~french
		 *\brief		Efface la valeur à l'index donné du set
		 *\param[in]	p_set	Le set
		 *\param[in]	p_where	L'itérateur sur la valeur
		 */
		template <typename T>
		static inline void erase_at( std::set< T > & p_set, typename std::set< T >::iterator & p_where )
		{
			typename std::set< T >::iterator l_erase = p_where;
			++ p_where;
			p_set.erase( l_erase );
		}
		/**
		 *\~english
		 *\brief		Erases the first value from the set
		 *\param[in]	p_set		The set
		 *\param[out]	p_tResult	Receives the value removed from the set
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la première valeur du set
		 *\param[in]	p_set		Le set
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du set
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_first( std::set< T > & p_set, T & p_tResult)
		{
			bool bReturn = false;

			if( p_set.size() > 0 )
			{
				bReturn = true;
				p_tResult = *p_set.begin();
				p_set.erase( p_set.begin());
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the last value from the set
		 *\param[in]	p_set		The set
		 *\param[out]	p_tResult	Receives the value removed from the set
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la dernière valeur du set
		 *\param[in]	p_set		Le set
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du set
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool erase_last( std::set< T > & p_set, T & p_tResult)
		{
			bool bReturn = false;

			if( p_set.size() > 0 )
			{
				bReturn = true;
				typename std::set< T >::reverse_iterator i = p_set.rbegin();
				p_tResult = * i;
				i++;
				p_set.erase( i.base() );
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the given value from the set
		 *\remark		Uses set::erase_at
						<br />Value is deleted only if it was erased
		 *\param[in]	p_set	The set
		 *\param[in]	p_key	The value to look for
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la valeur donnée du set
		 *\remark		Utilise set::erase_at
						<br />La valeur est désallouée seulement si elle a été effacée
		 *\param[in]	p_set	Le set
		 *\param[in]	p_key	La valeur à rechercher
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_at( std::set< T * > & p_set, T * p_key)
		{
			bool bReturn = erase_at( p_set, p_key );

			if( bReturn )
			{
				delete p_key;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the first value from the set
		 *\remark		Uses set::delete_at
						<br />Value is deleted only if it was erased
		 *\param[in]	p_set	The set
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la première valeur du set
		 *\remark		Uses set::delete_at
						<br />La valeur est désallouée seulement si elle a été effacée
		 *\param[in]	p_set	Le set
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_first( std::set< T * > & p_set )
		{
			T * pResult;
			bool bReturn = erase_first( p_set, pResult );

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the last value from the set
		 *\remark		Uses set::delete_at
						<br />Value is deleted only if it was erased
		 *\param[in]	p_set	The set
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la dernière valeur du set
		 *\remark		Uses set::delete_at
						<br />La valeur est désallouée seulement si elle a été effacée
		 *\param[in]	p_set	Le set
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template <typename T>
		static inline bool delete_last( std::set< T * > & p_set)
		{
			T * pResult;
			bool bReturn = erase_last( p_set, pResult);

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Creates and returns the intersection between two sets
		 *\remark		The intersection is the group of elements that are in both sets
		 *\param[in]	p_setA, p_setB	The sets to test
		 *\return		The intersection, if it exists
		 *\~french
		 *\brief		Crée et retourne l'intersection entre 2 sets
		 *\remark		L'intersection est l'ensemble d'éléments communs aux 2 sets
		 *\param[in]	p_setA, p_setB	Les sets
		 *\return		L'intersection, si elle existe
		 */
		template <typename T>
		static inline std::set< T > intersection( std::set< T > const & p_setA, std::set< T > const & p_setB)
		{
			std::set< T > l_set;

			if (p_setA.m_set.empty() || p_setB.m_set.empty())
			{
				return l_set;
			}

			typename std::set< T >::const_iterator i = p_setA.m_set.begin();
			typename std::set< T >::const_iterator j = p_setB.m_set.begin();

			typename std::set< T >::const_iterator iend = p_setA.m_set.end();
			typename std::set< T >::const_iterator jend = p_setB.m_set.end();

			while (i != iend)
			{
				while (j->first < i->first)
				{
					++ j;

					if (j == jend)
					{
						return l_set;
					}
				}

				if (* i == * j)
				{
					l_set.insert( * i);
				}

				++ i;
			}

			return l_set;
		}
	};
}

#endif

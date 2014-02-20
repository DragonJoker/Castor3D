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
#ifndef ___Castor_Map___
#define ___Castor_Map___

#include "SmartPtr.hpp"
#include <map>

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		std::map helper functions
	\~french
	\brief		Fonctions d'aide pour std::map
	*/
	class map
	{
	public:
		/**
		 *\~english
		 *\brief		Tells if the map contains an element at the given key
		 *\param[in]	p_map	The map
		 *\param[in]	p_key	The value to look for
		 *\return		\p true if p_tValue is in p_map, \p false if not
		 *\~french
		 *\brief		Dit si le map contient un élément à la clef donnée
		 *\param[in]	p_map	Le map
		 *\param[in]	p_key	La valeur à rechercher
		 *\return		\p true si p_tValue est dans p_map, \p false sinon
		 */
		template< typename T, typename U >
		static inline bool has( std::map< T, U > const & p_map, T const & p_key)
		{
			return p_map.find( p_key ) != p_map.end();
		}
		/**
		 *\~english
		 *\brief		Gets the value at given key from a map
		 *\param[in]	p_map	The map
		 *\param[in]	p_key	The value key
		 *\return		A pointer to the found value, \p nullptr if not found
		 *\~french
		 *\brief		Récupère la valeur à la clef donnée
		 *\param[in]	p_map	Le map
		 *\param[in]	p_key	La clef de la valeur
		 *\return		Un pointeur sur la valeur trouvée, \p nullptr si non-trouvée
		 */
		template< typename T, typename U >
		static inline U * find_or_null( std::map <T, U *> const & p_map, T const & p_key)
		{
			U * pReturn = nullptr;
			typename std::map <T, U const *> ::const_iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				pReturn = ifind->second;
			}

			return pReturn;
		}
		/**
		 *\~english
		 *\brief		Gives the element in a given map at a given key
		 *\param[in]	p_map	The map containing elements
		 *\param[in]	p_key	The element to find
		 *\return		The element if found, default value if not
		 *\~french
		 *\brief		Donne l'élément dans le map donné correspondant à la clef donnée
		 *\param[in]	p_map	Le map contenant des éléments
		 *\param[in]	p_key	L'élément à trouver
		 *\return		L'élément trouvé, valeur par défaut si non trouvé
		 */
		template< typename T, typename U >
		static inline U find_or_null( std::map< T, U > const & p_map, T const & p_key)
		{
			U uReturn;
			typename std::map< T, U > ::const_iterator const & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				uReturn = ifind->second;
			}

			return uReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the element at the given key, doesn't delete the data
		 *\param[in]	p_map		The map to modify
		 *\param[in]	p_key		The key of the element to remove from the map
		 *\param[out]	p_tResult	Receives the value removed from the map
		 *\return		\p true if the element was in the map
		 *\~french
		 *\brief		Enlève l'élément à la clef donnée, ne le désalloue pas
		 *\param[in]	p_map		Le map à modifier
		 *\param[in]	p_key		La clef de l'élément à enlever
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du map
		 *\return		\p true si l'élément était dans le map
		 */
		template< typename T, typename U >
		static inline bool erase_at( std::map< T, U > & p_map, T const & p_key, U & p_tResult)
		{
			bool bReturn = false;
			typename std::map< T, U > ::iterator ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				bReturn = true;
				p_tResult = ifind->second;
				p_map.erase( ifind);
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the value at the given index from the map
		 *\param[in]	p_map	The map
		 *\param[in]	p_where	The iterator on the value
		 *\~french
		 *\brief		Efface la valeur à l'index donné du map
		 *\param[in]	p_map	Le map
		 *\param[in]	p_where	L'itérateur sur la valeur
		 */
		template< typename T, typename U >
		static inline void erase_at( std::map< T, U > & p_map, typename std::map< T, U > ::iterator & p_where)
		{
			typename std::map< T, U > ::iterator l_erase = p_where;
			++ p_where;
			p_map.erase( l_erase);
		}
		/**
		 *\~english
		 *\brief		Erases the first value from the map
		 *\param[in]	p_map		The map
		 *\param[out]	p_tResult	Receives the value removed from the map
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la première valeur du map
		 *\param[in]	p_map		Le map
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du map
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template< typename T, typename U >
		static inline bool erase_first( std::map< T, U > & p_map, U & p_tResult)
		{
			bool bReturn = false;

			if (p_map.size() > 0)
			{
				bReturn = true;
				p_tResult = p_map.begin()->second;
				p_map.erase( p_map.begin());
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases the last value from the map
		 *\param[in]	p_map		The map
		 *\param[out]	p_tResult	Receives the value removed from the map
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface la dernière valeur du map
		 *\param[in]	p_map		Le map
		 *\param[out]	p_tResult	Reçoit la valeur enlevée du map
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template< typename T, typename U >
		static inline bool erase_last( std::map< T, U > & p_map, U & p_tResult)
		{
			bool bReturn = false;

			if (p_map.size() > 0)
			{
				typename std::map< T, U >::reverse_iterator i = p_map.rbegin();
				bReturn = true;
				p_tResult = i->second;
				i++;
				p_map.erase( i.base());
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the given value from the map
		 *\remark		Uses map::erase_at
						<br />Value is deleted only if it was erased
		 *\param[in]	p_map	The map
		 *\param[in]	p_tKey	The value to look for
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la valeur donnée du map
		 *\remark		Utilise map::erase_at
						<br />La valeur est désallouée seulement si elle a été effacée
		 *\param[in]	p_map	Le map
		 *\param[in]	p_tKey	La valeur à rechercher
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template< typename T, typename U >
		static inline bool delete_at( std::map <T, U *> & p_map, T const & p_tKey)
		{
			U * pResult;
			bool bReturn = erase_at( p_map, p_tKey, pResult);

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the first value from the map
		 *\remark		Uses map::erase_first
						<br />Value is deleted only if it was erased
		 *\param[in]	p_map	The map
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la première valeur du map
		 *\remark		Uses map::erase_first
						<br />La valeur est désallouée seulement si elle a été effacée
		 *\param[in]	p_map	Le map
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template< typename T, typename U >
		static inline bool delete_first( std::map <T, U *> & p_map)
		{
			U * pResult;
			bool bReturn = erase_first( p_map, pResult);

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Erases and deletes the last value from the map
		 *\remark		Uses map::erase_last
						<br />Value is deleted only if it was erased
		 *\param[in]	p_map	The map
		 *\return		\p true if The value was removed, \p false if not
		 *\~french
		 *\brief		Efface et désalloue la dernière valeur du map
		 *\remark		Uses map::erase_last
						<br />La valeur est désallouée seulement si elle a été effacée
		 *\param[in]	p_map	Le map
		 *\return		\p true si la valeur a été enlevée, \p false sinon
		 */
		template< typename T, typename U >
		static inline bool delete_last( std::map <T, U *> & p_map)
		{
			U * pResult;
			bool bReturn = erase_last( p_map, pResult);

			if (bReturn)
			{
				delete pResult;
			}

			return bReturn;
		}
		/**
		 *\~english
		 *\brief		Creates and returns the intersection between two maps
		 *\remark		The intersection is the group of elements that are in both maps
		 *\param[in]	p_mapA, p_mapB	The maps to test
		 *\return		The intersection, if it exists
		 *\~french
		 *\brief		Crée et retourne l'intersection entre 2 maps
		 *\remark		L'intersection est l'ensemble d'éléments communs aux 2 maps
		 *\param[in]	p_mapA, p_mapB	Les maps
		 *\return		L'intersection, si elle existe
		 */
		template< typename T, typename U >
		static inline std::map< T, U > intersects( std::map< T, U > const & p_mapA, std::map< T, U > const & p_mapB)
		{
			std::map< T, U > l_map;

			if (p_mapA.empty() || p_mapB.empty())
			{
				return l_map;
			}

			typename std::map< T, U > ::const_iterator i = p_mapA.begin();
			typename std::map< T, U > ::const_iterator j = p_mapB.begin();

			typename std::map< T, U > ::const_iterator iend = p_mapA.end();
			typename std::map< T, U > ::const_iterator jend = p_mapB.end();

			while (i != iend)
			{
				while (j->first < i->first)
				{
					++ j;

					if (j == jend)
					{
						return l_map;
					}
				}

				if (* i == * j)
				{
					l_map.insert( l_map.end(), * i);
				}

				++ i;
			}

			return l_map;
		}
	};
}

#endif

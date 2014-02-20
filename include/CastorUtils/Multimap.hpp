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
#ifndef ___Castor_Multimap___
#define ___Castor_Multimap___

#include <map>

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		std::multimap helper functions
	\~french
	\brief		Fonctions d'aide pour std::multimap
	*/
	class multimap
	{
	public:
		/**
		 *\~english
		 *\brief		Tells if the map contains elements at the given key
		 *\param[in]	p_map	The map
		 *\param[in]	p_key	The value to look for
		 *\return		\p true if p_tValue is in p_map, \p false if not
		 *\~french
		 *\brief		Dit si le map contient un(des) élément(s) à la clef donnée
		 *\param[in]	p_map	Le map
		 *\param[in]	p_key	La valeur à rechercher
		 *\return		\p true si p_tValue est dans p_map, \p false sinon
		 */
		template <typename T, typename U>
		static inline bool has( std::multimap <T, U> const & p_map, T const & p_key)
		{
			return p_map.find( p_key) != p_map.end();
		}
		/**
		 *\~english
		 *\brief		Returns the number of occurences of the given key in a multimap
		 *\param[in]	p_map	The map
		 *\param[in]	p_key	The value to look for
		 *\return		Number of occurences
		 *\~french
		 *\brief		Récupère le nombre d'occurences des éléments correspondant à la clef donnée
		 *\param[in]	p_map	Le map
		 *\param[in]	p_key	La valeur à rechercher
		 *\return		Nombre d'occurences
		 */
		template <typename T, typename U>
		static inline uint32_t count( std::multimap <T, U> const & p_map, T const & p_key)
		{
			typename std::multimap <T, U> ::const_iterator i = p_map.find( p_key);
			typename std::multimap <T, U> ::const_iterator const & iend = p_map.end();

			uint32_t l_count = 0;

			while (i != iend && i->first == p_key)
			{
				++ i;
				++ l_count;
			}

			return l_count;
		}
	};
}

#endif

/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___STL_MACROS_H___
#define ___STL_MACROS_H___

#include <set>

#include "Module_Utils.h"

namespace General
{ namespace Utils
{
	//! std::set class functions
	/*!
	Collection of functions for std::set
	*/
	class set
	{
	public:
		/**
		 * Applies a function with no arguments to all the members of the set. The function must be a member of the class stocked in the set
		 */
		template <typename T>
		static inline void cycle( const std::set <T *> & p_set, void( T::* p_func)( void))
		{
			typename std::set <T *> ::const_iterator i = p_set.begin();
			const typename std::set <T *> ::const_iterator & iend = p_set.end();

			for ( ; i != iend; ++ i)
			{
				(* i->* p_func)();
			}
		}
		/**
		 * Applies a function with one [in] argument to all the members of the set. The function must be a member of the class stocked in the set
		 */
		template <typename T, typename U>
		static inline void cycle( const std::set <T *> & p_set, void( T::* p_func)( const U &), const U & p_param)
		{
			typename std::set <T *> ::const_iterator i = p_set.begin();
			const typename std::set <T *> ::const_iterator & iend = p_set.end();

			for ( ; i != iend; ++ i)
			{
				(* i->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with one [in/out] argument to all the members of the set. The function must be a member of the class stocked in the set
		 */
		template <typename T, typename U>
		static inline void cycle( const std::set <T *> & p_set, void( T::* p_func)( U &), U & p_param)
		{
			typename std::set <T *> ::const_iterator i = p_set.begin();
			const typename std::set <T *> ::const_iterator & iend = p_set.end();

			for ( ; i != iend; ++ i)
			{
				(* i->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with one argument to all the members of the set. The function must be a member of the class stocked in the set
		 */
		template <typename T, typename U>
		static inline void cycle( const std::set <T *> & p_set, void( T::* p_func)( U), U p_param)
		{
			typename std::set <T *> ::const_iterator i = p_set.begin();
			const typename std::set <T *> ::const_iterator & iend = p_set.end();

			for ( ; i != iend; ++ i)
			{
				(* i->* p_func)( p_param);
			}
		}
		/**
		 * Deletes all the items in the set
		 */
		template <typename T>
		static inline void deleteAll( std::set <T *> & p_set)
		{
			typename std::set <T *> ::iterator i = p_set.begin();
			const typename std::set <T *> ::iterator & iend = p_set.end();

			for ( ; i != iend ; ++ i)
			{
				delete (* i);
			}

			p_set.clear();
		}
		/**
		 * Erases a value from a set, doesn't delete it
		 */
		template <typename T>
		static inline bool eraseValue( std::set <T> & p_set, const T & p_element)
		{
			const typename std::set <T> ::iterator ifind = p_set.find( p_element);

			if (ifind != p_set.end())
			{
				p_set.erase( ifind);
				return true;
			}

			return false;
		}
		/**
		 * Erases an iterator from the set, the given iterator the points over the next element. Doesn't erase the item
		 */
		template <typename T>
		static inline void eraseIterator( std::set <T> & p_set, typename std::set <T> ::iterator & p_where)
		{
			typename std::set <T> ::iterator l_erase = p_where;
			++ p_where;
			p_set.erase( l_erase);
		}
		/**
		 * Tells whether or not the set has an element with the given key
		 */
		template <typename T>
		static inline bool has( const std::set <T> & p_set, const T & p_key)
		{
			return p_set.find( p_key) != p_set.end();
		}
/*
		template <typename T>
		std::set <T> intersects( const std::set <T> & p_setA, const std::set <T> & p_setB)
		{
			std::set <T> l_intersection;

			std::insert_iterator <std::set <T> > l_insertIter( l_intersection, l_intersection.begin());

			std::set_intersection( p_setA.begin(), p_setA.end(), p_setB.begin(), p_setB.end(), l_insertIter);

			return l_intersection;
		}
*/
		/**
		 * Creates a set from the keys of a given map
		 */
		template <typename T,typename U>
		std::set <T> createFromMapKey( const std::map <T, U> & p_map)
		{
			std::set <T> l_set;

			typename std::map <T, U> ::const_iterator i = p_map.begin();
			const typename std::map <T, U> ::const_iterator & iend = p_map.end();

			for ( ; i != iend ; ++ i)
			{
				l_set.insert( l_set.end(), i->first);
			}

			return l_set;
		}
		/**
		 * Creates a set from the datas of a given map
		 */
		template <typename T, typename U>
		std::set <U> createFromMapData( const std::map <T, U> & p_map)
		{
			std::set <U> l_set;

			typename std::map <T, U> ::const_iterator i = p_map.begin();
			const typename std::map <T, U> ::const_iterator & iend = p_map.end();

			for ( ; i != iend ; ++ i)
			{
				l_set.insert( l_set.end(), i->second);
			}

			return l_set;
		}
	};
}
}

#endif

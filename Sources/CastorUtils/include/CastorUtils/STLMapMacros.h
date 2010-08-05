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
#ifndef ___STL_MAP_MACROS_H___
#define ___STL_MAP_MACROS_H___

#include <map>
#include <string>

#include "Module_Utils.h"

namespace General
{ namespace Utils
{
	//! std::map class functions
	/*!
	Collection of functions for std::map 
	*/
	class map
	{
	public:
		/**
		 * Deletes all the members of this map
		 */
		template <typename T, typename U>
		static inline void deleteAll( std::map <T, U *> & p_map)
		{
			typename std::map <T, U *> ::iterator i = p_map.begin();
			const typename std::map <T, U *> ::iterator & iend = p_map.end();

			for ( ; i != iend ; ++ i)
			{
				delete i->second;
			}

			p_map.clear();
		}
		/**
		 * Gives the element at the given key, NULL if it doesn't exist
		 */
		template <typename T, typename U>
		static inline U * findOrNull( const std::map <T, U *> & p_map, const T & p_key)
		{
			const typename std::map <T, U *> ::const_iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				return ifind->second;
			}

			return NULL;
		}
		/**
		 * Gives the element at the given key, default value if it doesn't exist
		 */		
		template <typename T, typename U>
		static inline U findOrNull( const std::map <T, U> & p_map, const T & p_key)
		{
			const typename std::map <T, U> ::const_iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				return ifind->second;
			}

			return U();
		}
		/**
		 * Tells whether or not the map has an element at the given key
		 */
		template <typename T, typename U>
		static inline bool has( const std::map <T, U> & p_map, const T & p_key)
		{
			return p_map.find( p_key) != p_map.end();
		}
		/**
		 * Erases the element at the given key, doesn't delete the data
		 */
		template <typename T, typename U>
		static inline bool erase( const std::map <T, U> & p_map, const T & p_key)
		{
			const typename std::map <T, U> ::const_iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				p_map.erase( ifind);
				return true;
			}

			return false;
		}
		/**
		 * Erases the element pointed by the iterator, doesn't delete the data
		 */
		template <typename T, typename U>
		static inline void eraseIterator( std::map <T, U> & p_map, typename std::map <T, U> ::iterator & p_where)
		{
			typename std::map <T, U> ::iterator l_erase = p_where;
			++ p_where;
			p_map.erase( l_erase);
		}
		/**
		 * Deletes the element and the value at the given key
		 */
		template <typename T, typename U>
		static inline bool deleteValue( std::map <T, U *> & p_map, const T & p_key)
		{
			const typename std::map <T, U *> ::iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				delete ifind->second;
				p_map.erase( ifind);
				return true;
			}

			return false;
		}
		/**
		 * Deletes the element and the value at the given key
		 */
		template <typename T, typename U>
		static inline bool deleteValue( std::map <T*, U *> & p_map, T * p_key)
		{
			if (p_key == NULL)
			{
				return false;
			}

			const typename std::map <T*, U *> ::iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				delete ifind->second;
				p_map.erase( ifind);
				return true;
			}

			return false;
		}
		/**
		 * Deletes the element and the value at the given key
		 */
		template <typename U>
		static inline bool deleteValue( std::map <String, U *> & p_map, const String & p_key)
		{
			if (p_key.empty())
			{
				return false;
			}

			const typename std::map <String, U *> ::iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				delete ifind->second;
				p_map.erase( ifind);
				return true;
			}

			return false;
		}
		/**
		 * Inserts a new element in the map, at the given key
		 */
		template <typename T, typename U>
		static inline U * insert( std::map <T, U *> & p_map, const T & p_key)
		{
			const typename std::map <T, U *> ::iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				return ifind->second;
			}
			
			U * l_object = new U();

			p_map.insert( typename std::map <T, U *> ::value_type( p_key, l_object));
			return l_object;
		}
		/**
		 * Inserts a new element in the map, at the given key, p_param is a param of the element constructor
		 */
		template <typename T, typename U, typename V>
		static inline U * insert( std::map <T, U *> & p_map, const T & p_key, const V & p_param)
		{
			const typename std::map <T, U *> ::iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				return ifind->second;
			}
			
			U * l_object = new U( p_param);

			p_map.insert( typename std::map <T, U *> ::value_type( p_key, l_object));
			return l_object;
		}
		/**
		 * Inserts a new element in the map, at the given key, param1 and p_param2 are params of the element constructor
		 */
		template <typename T, typename U, typename V,typename W>
		static inline U * insert( std::map <T, U *> & p_map, const T & p_key, const V & p_param, const W & p_param2)
		{
			const typename std::map <T, U *> ::iterator & ifind = p_map.find( p_key);

			if (ifind != p_map.end())
			{
				return ifind->second;
			}
			
			U * l_object = new U( p_param, p_param2);

			p_map.insert( typename std::map <T, U *> ::value_type( p_key, l_object));
			return l_object;
		}
		/**
		 * Applies a function without argument to each element of the map
		 */
		template <typename T, typename U>
		static inline void cycle( const std::map <T, U *> & p_map, void( U::* p_func)( void))
		{
			typename std::map <T, U *> ::const_iterator i = p_map.begin();
			const typename std::map <T, U *> ::const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)();
			}
		}
		/**
		 * Applies a function with one [in] argument to each element of the map
		 */
		template <typename T, typename U, typename Z>
		static inline void cycle( const std::map <T, U *> & p_map, void( U::* p_func)( const Z &), const Z & p_param)
		{
			typename std::map <T, U *> :: const_iterator i = p_map.begin();
			const typename std::map <T, U *> :: const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with two [in] argument to each element of the map
		 */
		template <typename T, typename U, typename V, typename W>
		static inline void cycle( const std::map <T, U *> & p_map, void( U::* p_func)( const V &, const W &), const V & p_param, const W & p_param2)
		{
			typename std::map <T, U *> :: const_iterator i = p_map.begin();
			const typename std::map <T, U *> :: const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)( p_param, p_param2);
			}
		}
		/**
		 * Applies a function with one argument to each element of the map
		 */
		template <typename T, typename U, typename Z>
		static inline void cycle( const std::map <T, U *> & p_map, void( U::* p_func)( Z), const Z & p_param)
		{
			typename std::map <T, U *> ::const_iterator i = p_map.begin();
			const typename std::map <T, U *> ::const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)( p_param);
			}
		}
		/**
		 * Creates and returns the intersection between two maps
		 */
		template <typename T, typename U>
		static inline std::map <T, U> intersects( const std::map <T, U> & p_mapA, const std::map <T, U> & p_mapB)
		{
			std::map <T, U> l_map;

			if (p_mapA.empty() || p_mapB.empty())
			{
				return l_map;
			}

			typename std::map <T, U> ::const_iterator i = p_mapA.begin();
			typename std::map <T, U> ::const_iterator j = p_mapB.begin();

			const typename std::map <T, U> ::const_iterator & iend = p_mapA.end();
			const typename std::map <T, U> ::const_iterator & jend = p_mapB.end();

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
}

#endif

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
#ifndef ___STL_MACROS3_H___
#define ___STL_MACROS3_H___

#include <map>
#include <set>
#include <vector>
#include <list>

#include "Module_Utils.h"

namespace General
{ namespace Utils
{
	//! std::multimap class functions
	/*!
	Collection of functions for std::multimap
	*/
	class multimap
	{
	public:
		/**
		 * Deletes all elements of a multimap
		 */
		template <typename T, typename U>
		static inline void deleteAll( std::multimap <T, U *> & p_map)
		{
			typename std::multimap <T, U *> ::iterator i = p_map.begin();
			const typename std::multimap <T, U *> ::iterator & iend = p_map.end();

			for ( ; i != iend ; ++ i)
			{
				delete i->second;
			}

			p_map.clear();
		}
		/**
		 * Tells whether or not a multimap has the couple of key and subkey
		 */
		template <typename T, typename U>
		static inline bool hasCouple( const std::multimap <T, U> & p_map, const T & p_key, const U & p_subkey)
		{
			typename std::multimap <T, U> ::const_iterator i = p_map.find( p_key);
			const typename std::multimap <T, U> ::const_iterator & iend = p_map.end();

			while (i != iend && i->first == p_key)
			{
				if (i->second == p_subkey)
				{
					return true;
				}

				++ i;
			}
			return false;
		}
		/**
		 * Returns the number of occurences of the given key in a multimap
		 */
		template <typename T, typename U>
		static inline unsigned int count( const std::multimap <T, U> & p_map, const T & p_key)
		{
			typename std::multimap <T, U> ::const_iterator i = p_map.find( p_key);
			const typename std::multimap <T, U> ::const_iterator & iend = p_map.end();

			unsigned int l_count = 0;

			while (i != iend && i->first == p_key)
			{
				++ i;
				++ l_count;
			}

			return l_count;
		}
		/**
		 * Tells whether or not the map has the given key
		 */
		template <typename T, typename U>
		static inline bool has( const std::multimap <T, U> & p_map, const T & p_key)
		{
			return p_map.find( p_key) != p_map.end();
		}
		/**
		 * Applies a function without argument to each argument of a multimap
		 */
		template <typename T, typename U>
		static inline void cycle( const std::multimap <T, U *> & p_map, void( U::* p_func)( void))
		{
			typename std::multimap <T, U *> ::const_iterator i = p_map.begin();
			const typename std::multimap <T, U *> ::const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)();
			}
		}
		/**
		 * Applies a function with one [in] argument to each argument of a multimap
		 */
		template <typename T, typename U, typename Z>
		static inline void cycle( const std::multimap <T, U *> & p_map, void( U::* p_func)( const Z &), const Z & p_param)
		{
			typename std::multimap <T, U *> :: const_iterator i = p_map.begin();
			const typename std::multimap <T, U *> :: const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with one [in/out] argument to each argument of a multimap
		 */
		template <typename T, typename U, typename Z>
		static inline void cycle( const std::multimap <T, U *> & p_map, void( U::* p_func)( Z &), Z & p_param)
		{
			typename std::multimap <T, U *> ::const_iterator i = p_map.begin();
			const typename std::multimap <T, U *> ::const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with one argument to each argument of a multimap
		 */
		template <typename T, typename U, typename Z>
		static inline void cycle( const std::multimap <T, U *> & p_map, void( U::* p_func)( Z), const Z & p_param)
		{
			typename std::multimap <T, U *> ::const_iterator i = p_map.begin();
			const typename std::multimap <T, U *> ::const_iterator & iend = p_map.end();

			for ( ; i != iend; ++ i)
			{
				(i->second->* p_func)( p_param);
			}
		}
	};

	//! std::vector class functions
	/*!
	Collection of functions for std::vector
	*/
	class vector
	{
	public:
		/**
		 * Deletes all element of a vector
		 */
		template <typename T>
		static inline void deleteAll( std::vector <T *> & p_vector)
		{
			size_t imax = p_vector.size();

			for (size_t i = 0 ; i < imax ; i ++)
			{
				delete p_vector[i];
			}

			p_vector.clear();
		}
		/**
		 * Erases a value from a vector, doesn't delete it
		 */
		template <typename T>
		static inline bool eraseValue( std::vector <T> & p_vector, const T & p_key)
		{
			for ( size_t i = 0 ; i < p_vector.size() ; )
			{
				if (p_vector[i] == p_key)
				{
					p_vector.erase( p_vector.begin() + i);
					return true;
				}
				else
				{
					++ i;
				}
			}
			return false;
		}
		/**
		 * Gets the value at given index from a vector, NULL if not found
		 */
		template <typename T>
		static inline T * getOrNull( std::vector <T *> & p_vector, const size_t p_index)
		{
			if (p_index < p_vector.size())
			{
				return p_vector[p_index];
			}
			return NULL;
		}
		/**
		 * Gets the value at given index from a vector, NULL if not found
		 */
		template <typename T>
		static inline bool has( std::vector <T *> & p_vector, const T * p_value)
		{
			bool l_bReturn = false;

			for (size_t i = 0 ; i < p_vector.size() && ! l_bReturn; i++)
			{
				l_bReturn = (p_vector[i] == p_value);
			}

			return l_bReturn;
		}
		/**
		 * Applies a function without argument to each element of a vector
		 */
		template <typename T>
		static inline void cycle( const std::vector <T *> & p_vector, void( T::* p_func)( void))
		{
			size_t imax = p_vector.size();

			for (size_t i = 0 ; i < imax ; i ++)
			{
				(p_vector[i]->* p_func)();
			}
		}
		/**
		 * Applies a function with one [in] argument to each element of a vector
		 */
		template <typename T, typename U>
		static inline void cycle( const std::vector <T *> & p_vector, void( T::* p_func)( const U &), const U & p_param)
		{
			size_t imax = p_vector.size();

			for (size_t i = 0 ; i < imax ; i ++)
			{
				(p_vector[i]->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with one [in/out] argument to each element of a vector
		 */
		template <typename T, typename U>
		static inline void cycle( const std::vector <T *> & p_vector, void( T::* p_func)( U &), U & p_param)
		{
			size_t imax = p_vector.size();

			for (size_t i = 0 ; i < imax ; i ++)
			{
				(p_vector[i]->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with one argument to each element of a vector
		 */
		template <typename T, typename U>
		static inline void cycle( const std::vector <T *> & p_vector, void( T::* p_func)( U), U p_param)
		{
			size_t imax = p_vector.size();

			for (size_t i = 0 ; i < imax ; i ++)
			{
				(p_vector[i]->* p_func)( p_param);
			}
		}
		/**
		 * Applies a function with two arguments to each element of a vector
		 */
		template <typename T, typename U, typename V>
		static inline void cycle( const std::vector <T *> & p_vector, void( T::* p_func)( U, V), U p_param1, V p_param2)
		{
			size_t imax = p_vector.size();

			for (size_t i = 0 ; i < imax ; i ++)
			{
				(p_vector[i]->* p_func)( p_param1, p_param2);
			}
		}
	};

	//! std::list class functions
	/*!
	Collection of functions for std::list
	*/
	class list
	{
	public:
		/**
		 * Deletes all elements from a list
		 */
		template <typename T>
		static inline void deleteAll( std::list <T *> & p_list)
		{
			typename std::list <T *> ::iterator i = p_list.begin();
			const typename std::list <T *> ::iterator & iend = p_list.end();

			for ( ; i != iend ; ++ i)
			{
				delete (* i);
			}

			p_list.clear();
		}
	};
}
}

#endif

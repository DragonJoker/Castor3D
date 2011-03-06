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
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_Multimap___
#define ___Castor_Multimap___

#include "Mutex.h"
#include <map>

namespace Castor
{
	namespace MultiThreading
	{
		//! Thread safe multimap
		/*!
		Overloads std::multimap and overloads all of it's function to replace them with thread safe ones
		\author Sylvain DOREMUS
		\version 0.1
		\date 09/02/2010
		*/
		template <typename T, typename U>
		class Multimap : public std::multimap <T, U>, public MemoryTraced< Multimap<T, U> >
		{
		private:
			template <typename _Ky, typename _Ty> friend class Castor::MultiThreading::Multimap;

		protected:
			typedef T _key_type;
			typedef U _value_type;

		public:
			typedef std::pair <_key_type, _value_type>	value_type;

		protected:
			typedef std::multimap <_key_type, _value_type>	_my_map;
			typedef _value_type								_element;
			typedef _value_type &							_reference;
			typedef const _value_type &						_const_reference;

		public:
			typedef typename _my_map::iterator					iterator;
			typedef typename _my_map::const_iterator			const_iterator;
			typedef typename _my_map::reverse_iterator			reverse_iterator;
			typedef typename _my_map::const_reverse_iterator	const_reverse_iterator;

		protected:
			typedef std::pair <iterator, bool>					_pair_ib;
			typedef std::pair <iterator, iterator>				_pair_ii;
			typedef std::pair <const_iterator, const_iterator>	_pair_cc;
			typedef Multimap <_key_type, _value_type>	_my_type;

		protected:
			RecursiveMutex m_mutex;

		public:
			Multimap()
			{
			}
			template <class _Iter>
			Multimap(_Iter p_itFirst, _Iter p_itLast)
				:	_my_map( p_itFirst, p_itLast)
			{
			}
			explicit Multimap( const typename _my_map::key_compare & p_cPred)
				:	_my_map( p_cPred)
			{
			}
			Multimap( const typename _my_map::key_compare& p_cPred, const typename _my_map::allocator_type& p_aAl)
				:	_my_map( p_cPred, p_aAl)
			{
			}
			~Multimap()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::clear();
			}
			virtual iterator begin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::begin();
			}
			virtual const_iterator begin() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::begin();
			}
			virtual iterator end()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::end();
			}
			virtual const_iterator end() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::end();
			}
			virtual reverse_iterator rbegin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rbegin();
			}
			virtual const_reverse_iterator rbegin()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rbegin();
			}
			virtual reverse_iterator rend()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rend();
			}
			virtual const_reverse_iterator rend()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rend();
			}
			virtual size_t size()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::size();
			}
			virtual size_t max_size()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::max_size();
			}
			virtual bool empty()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::empty();
			}
			iterator insert( const value_type & p_vVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::insert( p_vVal);
			}
			iterator insert( const_iterator p_itWhere, const value_type & p_vVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::insert( p_itWhere, p_vVal);
			}
			template <class _Iter>
			void insert( _Iter p_itFirst, _Iter p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::insert( p_itFirst, p_itLast);
			}
			void erase( const_iterator p_itWhere)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::erase( p_itWhere);
			}
			size_t erase( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::erase( p_kKeyval);
			}
			void erase( const_iterator p_itFirst, const_iterator p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::erase( p_itFirst, p_itLast);
			}
			virtual iterator find( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::find( p_kKeyval);
			}
			virtual const_iterator find( const _key_type & p_kKeyval)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::find( p_kKeyval);
			}
			virtual size_t count( const _key_type & p_kKeyval)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::count( p_kKeyval);
			}
			virtual iterator lower_bound( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::lower_bound( p_kKeyval);
			}
			const_iterator lower_bound( const _key_type & p_kKeyval)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::lower_bound( p_kKeyval);
			}
			virtual iterator upper_bound( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::upper_bound( p_kKeyval);
			}
			const_iterator upper_bound( const _key_type & p_kKeyval)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::upper_bound( p_kKeyval);
			}
			_pair_ii equal_range( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::equal_range( p_kKeyval);
			}
			_pair_cc equal_range( const _key_type & p_kKeyval)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::equal_range( p_kKeyval);
			}
			void swap( _my_type & p_map)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::swap( p_map);
			}
			const RecursiveMutex & GetLocker()const
			{
				return m_mutex;
			}
		};
	}

	namespace Utils
	{
		//! std::multimap and Castor::MultiThreading::Multimap helper functions
		/*!
		Collection of functions for std::multimap
		*/
		class multimap
		{
		public:
			/*********************************************************************************
				Castor::MultiThreading::Multimap Version
			*********************************************************************************/
			/**
			 * Deletes all elements of a multimap
			 */
			template <typename T, typename U>
			static inline void deleteAll( Castor::MultiThreading::Multimap <T, U *> & p_map)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Multimap <T, U *> ::iterator i = p_map.begin();
				const typename Castor::MultiThreading::Multimap <T, U *> ::iterator & iend = p_map.end();

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
			static inline bool hasCouple( Castor::MultiThreading::Multimap <T, U> & p_map, const T & p_key, const U & p_subkey)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Multimap <T, U> ::const_iterator i = p_map.find( p_key);
				const typename Castor::MultiThreading::Multimap <T, U> ::const_iterator & iend = p_map.end();

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
			static inline unsigned int count( MultiThreading::Multimap <T, U> & p_map, const T & p_key)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Multimap <T, U> ::const_iterator i = p_map.find( p_key);
				const typename Castor::MultiThreading::Multimap <T, U> ::const_iterator & iend = p_map.end();

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
			static inline bool has( Castor::MultiThreading::Multimap <T, U> & p_map, const T & p_key)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				return p_map.find( p_key) != p_map.end();
			}
			/**
			 * Applies a function without argument to each argument of a multimap
			 */
			template <typename T, typename U>
			static inline void cycle( Castor::MultiThreading::Multimap <T, U *> & p_map, void( U::* p_func)( void))
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Multimap <T, U *> ::const_iterator i = p_map.begin();
				const typename Castor::MultiThreading::Multimap <T, U *> ::const_iterator & iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each argument of a multimap
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( Castor::MultiThreading::Multimap <T, U *> & p_map, void( U::* p_func)( const Z &), const Z & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Multimap <T, U *> :: const_iterator i = p_map.begin();
				const typename Castor::MultiThreading::Multimap <T, U *> :: const_iterator & iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each argument of a multimap
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( Castor::MultiThreading::Multimap <T, U *> & p_map, void( U::* p_func)( Z &), Z & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Multimap <T, U *> ::const_iterator i = p_map.begin();
				const typename Castor::MultiThreading::Multimap <T, U *> ::const_iterator & iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each argument of a multimap
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( Castor::MultiThreading::Multimap <T, U *> & p_map, void( U::* p_func)( Z), const Z & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Multimap <T, U *> ::const_iterator i = p_map.begin();
				const typename Castor::MultiThreading::Multimap <T, U *> ::const_iterator & iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}

			/*********************************************************************************
				std::multimap Version
			*********************************************************************************/
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
	}
}

#endif

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
#ifndef ___Castor_Map___
#define ___Castor_Map___

#include "Mutex.h"
#include <map>

namespace Castor
{
	namespace MultiThreading
	{
		//! Thread safe map
		/*!
		Overloads std::map and overloads all of it's function to replace them with thread safe ones
		\author Sylvain DOREMUS
		\version 0.1
		\date 09/02/2010
		*/
		template <typename Ky, typename Ty>
		class Map : public std::map <Ky, Ty>, public MemoryTraced< Map<Ky, Ty> >
		{
		private:
			template <typename _Ky, typename _Ty> friend class Castor::MultiThreading::Map;

		protected:
			typedef Ty									_mapped_type;
			typedef Ky									_key_type;
			typedef std::map <_key_type, _mapped_type>	_my_map;
			typedef _mapped_type						_element;
			typedef _mapped_type &						_reference;
			typedef const _mapped_type &				_const_reference;
			typedef Map <_key_type, _mapped_type>		_my_type;

		public:
			typedef typename _my_map::key_compare				key_compare;
			typedef typename _my_map::value_compare				value_compare;
			typedef typename _my_map::allocator_type			allocator_type;
			typedef typename _my_map::value_type				value_type;
			typedef typename _my_map::iterator					iterator;
			typedef typename _my_map::const_iterator			const_iterator;
			typedef typename _my_map::reverse_iterator			reverse_iterator;
			typedef typename _my_map::const_reverse_iterator	const_reverse_iterator;

		protected:
			typedef std::pair <iterator, bool>					_Pairib;
			typedef std::pair <iterator, iterator>				_Pairii;
			typedef std::pair <const_iterator, const_iterator>	_Paircc;

		private:
			RecursiveMutex m_mutex;

		public:
			Map()
			{
			}
			explicit Map( const key_compare & p_cPred)
				:	_my_map( p_cPred)
			{
			}
			Map( const key_compare & p_cPred, const allocator_type & p_aAl)
				:	_my_map( p_cPred, p_aAl)
			{
			}
			template <class _Iter>
			Map( _Iter p_itFirst, _Iter p_itLast)
				:	_my_map( p_itFirst, p_itLast)
			{
			}
			template <class _Iter>
			Map( _Iter p_itFirst, _Iter p_itLast, const key_compare & p_cPred)
				:	_my_map( p_itFirst, p_itLast, p_cPred)
			{
			}
			template <class _Iter>
			Map( _Iter p_itFirst, _Iter p_itLast, const key_compare & p_cPred, const allocator_type & p_aAl)
				:	_my_map( p_itFirst, p_itLast, p_cPred, p_aAl)
			{
			}
			Map( const value_type * p_pFirst, const value_type * p_pLast, const key_compare & p_cParg, const allocator_type & p_aAl)
				:	_my_map( p_pFirst, p_pLast, p_cParg, p_aAl)
			{
			}
			Map( const _my_type & p_mRight)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_mRight.GetLocker());
				_my_map::operator =( p_mRight);
			}
			~Map()
			{
				clear();
			}
			_my_type & operator =( const _my_type & p_mRight)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::clear();
				_my_map::operator =( p_mRight);
				return * this;
			}
			_mapped_type & operator []( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::operator []( p_kKeyval);
			}
			iterator begin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::begin();
			}
			const_iterator begin() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::begin();
			}
			iterator end()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::end();
			}
			const_iterator end() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::end();
			}
			iterator _Make_iter( const_iterator p_itWhere) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::_Make_iter( p_itWhere);
			}
			reverse_iterator rbegin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rbegin();
			}
			const_reverse_iterator rbegin() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rbegin();
			}
			reverse_iterator rend()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rend();
			}
			const_reverse_iterator rend() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::rend();
			}
			size_t size() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::size();
			}
			size_t max_size() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::max_size();
			}
			bool empty() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::empty();
			}
			allocator_type get_allocator() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::get_allocator();
			}
			key_compare key_comp() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::key_comp();
			}
			value_compare value_comp() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::value_comp();
			}
			_Pairib insert( const value_type & p_vVal)
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
			iterator erase( const_iterator p_itWhere)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::erase( p_itWhere);
			}
			iterator erase( const_iterator p_itFirst, const_iterator p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::erase( p_itFirst, p_itLast);
			}
			size_t erase( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::erase( p_kKeyval);
			}
			void erase( const _key_type * p_pFirst, const _key_type * p_pLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::erase( p_pFirst, p_pLast);
			}
			void clear()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::clear();
			}
			iterator find( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::find( p_kKeyval);
			}
			const_iterator find( const _key_type & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::find( p_kKeyval);
			}
			size_t count( const _key_type & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::count( p_kKeyval);
			}
			iterator lower_bound( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::lower_bound( p_kKeyval);
			}
			const_iterator lower_bound( const _key_type & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::lower_bound( p_kKeyval);
			}
			iterator upper_bound( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::upper_bound( p_kKeyval);
			}
			const_iterator upper_bound( const _key_type & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::upper_bound( p_kKeyval);
			}
			_Pairii equal_range( const _key_type & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::equal_range( p_kKeyval);
			}
			_Paircc equal_range( const _key_type & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::equal_range( p_kKeyval);
			}
			void swap( _my_type & p_mRight)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_map::swap( p_mRight);
			}
			_mapped_type findOrNull( const _key_type & p_key)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				const_iterator ifind = _my_map::find( p_key);
				_mapped_type tReturn;

				if (ifind != _my_map::end())
				{
					tReturn = ifind->second;
				}

				return tReturn;
			}
			bool has( const _key_type & p_key)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_map::find( p_key) != _my_map::end();
			}
			void eraseIterator( iterator & p_where)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				iterator l_erase = p_where;
				++ p_where;
				_my_map::erase( l_erase);
			}
			bool eraseValue( const _key_type & p_key, _mapped_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;
				const_iterator ifind = _my_map::find( p_key);

				if (ifind != _my_map::end())
				{
					tResult = ifind->second;
					bReturn = true;
					_my_map::erase( ifind);
				}

				return bReturn;
			}
			bool eraseFirstValue( _mapped_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_map::size() > 0)
				{
					tResult = _my_map::begin()->second;
					bReturn = true;
					_my_map::erase( _my_map::begin());
				}

				return bReturn;
			}
			bool eraseLastValue( _mapped_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_map::size() > 0)
				{
					reverse_iterator i = _my_map::rbegin();
					tResult = i->second;
					bReturn = true;
					i++;
					_my_map::erase( i.base());
				}

				return bReturn;
			}
			_my_type intersects( const _my_type & p_mapB)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_mapB.GetLocker());
				_my_type l_map;

				if (_my_map::empty() || static_cast<_my_map>( p_mapB).empty())
				{
					return l_map;
				}

				const_iterator i = begin();
				const_iterator j = static_cast<_my_map>( p_mapB).begin();

				const_iterator & iend = end();
				const_iterator & jend = static_cast<_my_map>( p_mapB).end();

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
			const RecursiveMutex & GetLocker()const
			{
				return m_mutex;
			}
		};
	}

	namespace Utils
	{
		//! std::map and Castor::MultiThreading::Map helper functions
		/*!
		Collection of functions for std::map 
		*/
		class map
		{
		public:
			/**********************************************************************************************
						Castor::MultiThreading::Map version
			**********************************************************************************************/
			/**
			 * Deletes all the members of this map
			 */
			template <typename T, typename U>
			static inline void deleteAll( Castor::MultiThreading::Map <T, U *> & p_map)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U *>::iterator iend = p_map.end();

				for ( ; i != iend ; ++ i)
				{
					delete i->second;
				}

				p_map.clear();
			}
			/**
			 * Deletes the first element and the value associated
			 */
			template <typename T, typename U>
			static inline bool deleteFirstValue( Castor::MultiThreading::Map <T, U *> & p_map)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				U * pResult;
				bool bReturn = p_map.eraseFirstValue( pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes the first element and the value associated
			 */
			template <typename T, typename U>
			static inline bool deleteLastValue( Castor::MultiThreading::Map <T, U *> & p_map)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				U * pResult;
				bool bReturn = p_map.eraseLastValue( pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes the element and the value at the given key
			 */
			template <typename T, typename U>
			static inline bool deleteValue( Castor::MultiThreading::Map <T, U *> & p_map, const T & p_key)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				U * pResult;
				bool bReturn = p_map.eraseValue( p_key, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Inserts a new element in the map, at the given key
			 */
			template <typename T, typename U>
			static inline U * insert( Castor::MultiThreading::Map <T, U *> & p_map, const T & p_key)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *>::const_iterator ifind = p_map.find( p_key);

				if (ifind != p_map.end())
				{
					return ifind->second;
				}
				
				U * l_object = new U();

				p_map.insert( typename Castor::MultiThreading::Map <T, U *>::value_type( p_key, l_object));
				return l_object;
			}
			/**
			 * Inserts a new element in the map, at the given key, p_param is a param of the element constructor
			 */
			template <typename T, typename U, typename V>
			static inline U * insert( Castor::MultiThreading::Map <T, U *> & p_map, const T & p_key, const V & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *>::const_iterator ifind = p_map.find( p_key);

				if (ifind != p_map.end())
				{
					return ifind->second;
				}
				
				U * l_object = new U( p_param);

				p_map.insert( typename Castor::MultiThreading::Map <T, U *> ::value_type( p_key, l_object));
				return l_object;
			}
			/**
			 * Inserts a new element in the map, at the given key, param1 and p_param2 are params of the element constructor
			 */
			template <typename T, typename U, typename V, typename W>
			static inline U * insert( Castor::MultiThreading::Map <T, U *> & p_map, const T & p_key, const V & p_param, const W & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *> ::const_iterator ifind = p_map.find( p_key);

				if (ifind != p_map.end())
				{
					return ifind->second;
				}
				
				U * l_object = new U( p_param, p_param2);

				p_map.insert( typename Castor::MultiThreading::Map <T, U *> ::value_type( p_key, l_object));
				return l_object;
			}
			/**
			 * Applies a function without argument to each element of the map, pointer version
			 */
			template <typename T, typename U>
			static inline void cycle( Castor::MultiThreading::Map <T, U *> & p_map, void( U::* p_func)( void))
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of the map
			 */
			template <typename T, typename U>
			static inline void cycle( Castor::MultiThreading::Map <T, U> & p_map, void( U::* p_func)( void))
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the map, pointer version
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( Castor::MultiThreading::Map <T, U *> & p_map, void( U::* p_func)( const Z &), const Z & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the map
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( Castor::MultiThreading::Map <T, U> & p_map, void( U::* p_func)( const Z &), const Z & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the map, pointer version
			 */
			template <typename T, typename U, typename V, typename W>
			static inline void cycle( Castor::MultiThreading::Map <T, U *> & p_map, void( U::* p_func)( const V &, const W &), const V & p_param, const W & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the map
			 */
			template <typename T, typename U, typename V, typename W>
			static inline void cycle( Castor::MultiThreading::Map <T, U> & p_map, void( U::* p_func)( const V &, const W &), const V & p_param, const W & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with one argument to each element of the map, pointer version
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( Castor::MultiThreading::Map <T, U *> & p_map, void( U::* p_func)( Z), const Z & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U *>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of the map
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( Castor::MultiThreading::Map <T, U> & p_map, void( U::* p_func)( Z), const Z & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_map.GetLocker());
				typename Castor::MultiThreading::Map <T, U>::iterator i = p_map.begin();
				typename Castor::MultiThreading::Map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)( p_param);
				}
			}

		
			/**********************************************************************************************
						std::map version
			**********************************************************************************************/
			/**
			 * Gives the element at the given key, NULL if it doesn't exist
			 */
			template <typename T, typename U>
			static inline U * findOrNull( const std::map <T, U *> & p_map, const T & p_key)
			{
				U * pReturn = NULL;
				const typename std::map <T, U *> ::const_iterator & ifind = p_map.find( p_key);

				if (ifind != p_map.end())
				{
					pReturn = ifind->second;
				}

				return pReturn;
			}
			/**
			 * Gives the element at the given key, default value if it doesn't exist
			 */		
			template <typename T, typename U>
			static inline U findOrNull( const std::map <T, U> & p_map, const T & p_key)
			{
				U uReturn;
				const typename std::map <T, U> ::const_iterator & ifind = p_map.find( p_key);

				if (ifind != p_map.end())
				{
					uReturn = ifind->second;
				}

				return uReturn;
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
			static inline bool eraseValue( std::map <T, U> & p_map, const T & p_key, U & uResult)
			{
				bool bReturn = false;
				typename std::map <T, U> ::iterator ifind = p_map.find( p_key);

				if (ifind != p_map.end())
				{
					bReturn = true;
					uResult = ifind->second;
					p_map.erase( ifind);
				}

				return bReturn;
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
			 * Erases the first element of the map, doesn't delete the data
			 */
			template <typename T, typename U>
			static inline bool eraseFirstValue( std::map <T, U> & p_map, U & uResult)
			{
				bool bReturn = false;

				if (p_map.size() > 0)
				{
					bReturn = true;
					uResult = p_map.begin()->second;
					p_map.erase( p_map.begin());
				}

				return bReturn;
			}
			/**
			 * Erases the last element of the map, doesn't delete the data
			 */
			template <typename T, typename U>
			static inline bool eraseLastValue( std::map <T, U> & p_map, U & uResult)
			{
				bool bReturn = false;

				if (p_map.size() > 0)
				{
					typename std::map <T, U>::reverse_iterator i = p_map.rbegin();
					bReturn = true;
					uResult = i->second;
					i++;
					p_map.erase( i.base());
				}

				return bReturn;
			}
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
			 * Deletes the first element and the value associated
			 */
			template <typename T, typename U>
			static inline bool deleteFirstValue( std::map <T, U *> & p_map)
			{
				U * pResult;
				bool bReturn = eraseFirstValue( p_map, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes the last element and the value associated
			 */
			template <typename T, typename U>
			static inline bool deleteLastValue( std::map <T, U *> & p_map)
			{
				U * pResult;
				bool bReturn = eraseLastValue( p_map, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes the element and the value at the given key
			 */
			template <typename T, typename U>
			static inline bool deleteValue( std::map <T, U *> & p_map, const T & p_key)
			{
				U * pResult;
				bool bReturn = eraseValue( p_map, p_key, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Inserts a new element in the map, at the given key
			 */
			template <typename T, typename U>
			static inline U * insert( std::map <T, U *> & p_map, const T & p_key)
			{
				typename std::map <T, U *>::iterator ifind = p_map.find( p_key);

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
				typename std::map <T, U *> ::iterator ifind = p_map.find( p_key);

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
				typename std::map <T, U *> ::iterator ifind = p_map.find( p_key);

				if (ifind != p_map.end())
				{
					return ifind->second;
				}
				
				U * l_object = new U( p_param, p_param2);

				p_map.insert( typename std::map <T, U *> ::value_type( p_key, l_object));
				return l_object;
			}
			/**
			 * Applies a function without argument to each element of the map, pointer version
			 */
			template <typename T, typename U>
			static inline void cycle( std::map <T, U *> & p_map, void( U::* p_func)( void))
			{
				typename std::map <T, U *>::iterator i = p_map.begin();
				typename std::map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of the map
			 */
			template <typename T, typename U>
			static inline void cycle( std::map <T, U> & p_map, void( U::* p_func)( void))
			{
				typename std::map <T, U>::iterator i = p_map.begin();
				typename std::map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of the map
			 *
			template <typename T, typename U>
			static inline void cycle( std::map <T, Templates::shared_ptr<U> > & p_map, void( U::* p_func)( void))
			{
				typename std::map <T, Templates::shared_ptr<U> >::iterator i = p_map.begin();
				typename std::map <T, Templates::shared_ptr<U> >::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the map, pointer version
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( std::map <T, U *> & p_map, void( U::* p_func)( const Z &), const Z & p_param)
			{
				typename std::map <T, U *>::iterator i = p_map.begin();
				typename std::map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the map, pointer version
			 *
			template <typename T, typename U, typename Z>
			static inline void cycle( std::map <T, Templates::shared_ptr<U> > & p_map, void( U::* p_func)( const Z &), const Z & p_param)
			{
				typename std::map <T, Templates::shared_ptr<U> >::iterator i = p_map.begin();
				typename std::map <T, Templates::shared_ptr<U> >::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the map
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( std::map <T, U> & p_map, void( U::* p_func)( const Z &), const Z & p_param)
			{
				typename std::map <T, U>::iterator i = p_map.begin();
				typename std::map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the map, pointer version
			 */
			template <typename T, typename U, typename V, typename W>
			static inline void cycle( std::map <T, U *> & p_map, void( U::* p_func)( const V &, const W &), const V & p_param, const W & p_param2)
			{
				typename std::map <T, U *>::iterator i = p_map.begin();
				typename std::map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the map, pointer version
			 *
			template <typename T, typename U, typename V, typename W>
			static inline void cycle( std::map <T, Templates::shared_ptr<U> > & p_map, void( U::* p_func)( const V &, const W &), const V & p_param, const W & p_param2)
			{
				typename std::map <T, Templates::shared_ptr<U> >::iterator i = p_map.begin();
				typename std::map <T, Templates::shared_ptr<U> >::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the map
			 */
			template <typename T, typename U, typename V, typename W>
			static inline void cycle( std::map <T, U> & p_map, void( U::* p_func)( const V &, const W &), const V & p_param, const W & p_param2)
			{
				typename std::map <T, U>::iterator i = p_map.begin();
				typename std::map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with one argument to each element of the map, pointer version
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( std::map <T, U *> & p_map, void( U::* p_func)( Z), const Z & p_param)
			{
				typename std::map <T, U *>::iterator i = p_map.begin();
				typename std::map <T, U *>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of the map, pointer version
			 *
			template <typename T, typename U, typename Z>
			static inline void cycle( std::map <T, Templates::shared_ptr<U> > & p_map, void( U::* p_func)( Z), const Z & p_param)
			{
				typename std::map <T, Templates::shared_ptr<U> >::iterator i = p_map.begin();
				typename std::map <T, Templates::shared_ptr<U> >::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of the map
			 */
			template <typename T, typename U, typename Z>
			static inline void cycle( std::map <T, U> & p_map, void( U::* p_func)( Z), const Z & p_param)
			{
				typename std::map <T, U>::iterator i = p_map.begin();
				typename std::map <T, U>::const_iterator iend = p_map.end();

				for ( ; i != iend; ++ i)
				{
					(i->second.* p_func)( p_param);
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

				typename std::map <T, U> ::const_iterator iend = p_mapA.end();
				typename std::map <T, U> ::const_iterator jend = p_mapB.end();

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

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

#include "Mutex.hpp"
#include "SmartPtr.hpp"
#include <set>

namespace Castor
{
	namespace MultiThreading
	{
		//! Thread safe set
		/*!
		Overloads std::set and overloads all of it's function to replace them with thread safe ones
		\author Sylvain DOREMUS
		\version 0.1
		\date 09/02/2010
		*/
		template <typename Ty>
		class Set : public std::set <Ty>, public MemoryTraced< Set<Ty> >
		{
		private:
			template <typename _Ty> friend class Castor::MultiThreading::Set;

		protected:
			typedef Ty						_set_type;
			typedef std::set <_set_type>	_my_set;
			typedef _set_type				_element;
			typedef _set_type &				_reference;
			typedef _set_type const &		_const_reference;
			typedef Set <_set_type>			_my_type;

		public:
			typedef typename _my_set::key_compare				key_compare;
			typedef typename _my_set::value_compare				value_compare;
			typedef typename _my_set::allocator_type			allocator_type;
			typedef typename _my_set::value_type				value_type;
			typedef typename _my_set::iterator					iterator;
			typedef typename _my_set::const_iterator			const_iterator;
			typedef typename _my_set::reverse_iterator			reverse_iterator;
			typedef typename _my_set::const_reverse_iterator	const_reverse_iterator;

		protected:
			typedef std::pair <iterator, bool>					_Pairib;
			typedef std::pair <iterator, iterator>				_Pairii;
			typedef std::pair <const_iterator, const_iterator>	_Paircc;

		private:
			RecursiveMutex m_mutex;

		public:
			Set()
			{
			}
			explicit Set( key_compare const & p_cPred)
				:	_my_set( p_cPred)
			{
			}
			Set( key_compare const & p_cPred, allocator_type const & p_aAl)
				:	_my_set( p_cPred, p_aAl)
			{
			}
			template <class _Iter>
			Set( _Iter p_itFirst, _Iter p_itLast)
				:	_my_set( p_itFirst, p_itLast)
			{
			}
			template <class _Iter>
			Set( _Iter p_itFirst, _Iter p_itLast, key_compare const & p_cPred)
				:	_my_set( p_itFirst, p_itLast, p_cPred)
			{
			}
			template <class _Iter>
			Set(_Iter p_itFirst, _Iter p_itLast, key_compare const & p_cPred, allocator_type const & p_aAl)
				:	_my_set( p_itFirst, p_itLast, p_cPred, p_aAl)
			{
			}
			Set( value_type const * p_pFirst, value_type const * p_pLast, key_compare const & p_cParg, allocator_type const & p_aAl)
				:	_my_set( p_pFirst, p_pLast, p_cParg, p_aAl)
			{
			}
			Set( _my_type const & p_mRight)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_mRight.GetLocker());
				assign( p_mRight.begin(), p_mRight.end());
			}
			~Set()
			{
				clear();
			}
			_my_type & operator =( _my_type const & p_mRight)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_set::clear();
				_my_set::operator =( p_mRight);
			}
			iterator begin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::begin();
			}
			const_iterator begin() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::begin();
			}
			iterator end()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::end();
			}
			const_iterator end() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::end();
			}
			iterator _Make_iter( const_iterator p_itWhere) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::_Make_iter( p_itWhere);
			}
			reverse_iterator rbegin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::rbegin();
			}
			const_reverse_iterator rbegin() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::rbegin();
			}
			reverse_iterator rend()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::rend();
			}
			const_reverse_iterator rend() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::rend();
			}
			size_t size() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::size();
			}
			size_t max_size() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::max_size();
			}
			bool empty() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::empty();
			}
			allocator_type get_allocator() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::get_allocator();
			}
			key_compare key_comp() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::key_comp();
			}
			value_compare value_comp() const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::value_comp();
			}
			_Pairib insert( value_type const & p_vVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::insert( p_vVal);
			}
			iterator insert( const_iterator p_itWhere, value_type const & p_vVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::insert( p_itWhere, p_vVal);
			}
			template <class _Iter>
			void insert( _Iter p_itFirst, _Iter p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_set::insert( p_itFirst, p_itLast);
			}
			iterator erase( const_iterator p_itWhere)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::erase( p_itWhere);
			}
			iterator erase( const_iterator p_itFirst, const_iterator p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::erase( p_itFirst, p_itLast);
			}
			size_t erase( _set_type const & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::erase( p_kKeyval);
			}
			void erase( _set_type const * p_pFirst, _set_type const * p_pLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_set::erase( p_pFirst, p_pLast);
			}
			void clear()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_set::clear();
			}
			iterator find( _set_type const & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::find( p_kKeyval);
			}
			const_iterator find( _set_type const & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::find( p_kKeyval);
			}
			size_t count( _set_type const & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::count( p_kKeyval);
			}
			iterator lower_bound( _set_type const & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::lower_bound( p_kKeyval);
			}
			const_iterator lower_bound( _set_type const & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::lower_bound( p_kKeyval);
			}
			iterator upper_bound( _set_type const & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::upper_bound( p_kKeyval);
			}
			const_iterator upper_bound( _set_type const & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::upper_bound( p_kKeyval);
			}
			_Pairii equal_range( _set_type const & p_kKeyval)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::equal_range( p_kKeyval);
			}
			_Paircc equal_range( _set_type const & p_kKeyval) const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::equal_range( p_kKeyval);
			}
			void swap( _my_type & p_mRight)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_set::swap( p_mRight);
			}
			_set_type findOrNull( _set_type const & p_key)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				const_iterator ifind = _my_set::find( p_key);
				_set_type tReturn;

				if (ifind != _my_set::end())
				{
					tReturn = * ifind;
				}

				return tReturn;
			}
			bool has( _set_type const & p_key)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_set::find( p_key) != _my_set::end();
			}
			void eraseIterator( iterator & p_where)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				iterator l_erase = p_where;
				++ p_where;
				_my_set::erase( l_erase);
			}
			bool eraseValue( _set_type const & p_key)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;
				const_iterator ifind = _my_set::find( p_key);

				if (ifind != _my_set::end())
				{
					bReturn = true;
					_my_set::erase( ifind);
				}

				return bReturn;
			}
			bool eraseFirstValue( _set_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_set::size() > 0)
				{
					tResult = * _my_set::begin();
					bReturn = true;
					_my_set::erase( _my_set::begin());
				}

				return bReturn;
			}
			bool eraseLastValue( _set_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_set::size() > 0)
				{
					reverse_iterator i = _my_set::rbegin();
					tResult = * i;
					bReturn = true;
					i++;
					_my_set::erase( i.base());
				}

				return bReturn;
			}
			_my_set intersects( _my_set const & p_set)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				_my_set l_set;

				if (_my_set::empty() || static_cast <_my_set>( p_set).empty())
				{
					return l_set;
				}

				const_iterator i = _my_set::begin();
				const_iterator j = static_cast <_my_set>( p_set).begin();

				const_iterator iend = _my_set::end();
				const_iterator jend = static_cast <_my_set>( p_set).end();

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
			RecursiveMutex const & GetLocker()const
			{
				return m_mutex;
			}
		};
	}

	namespace Utils
	{
		//! std::set and Castor::MultiThreading::Set helper functions
		/*!
		Collection of functions for std::set
		*/
		class set
		{
		public:
			/**********************************************************************************************
						Castor::MultiThreading::Set version
			**********************************************************************************************/
			/**
			 * Deletes all the members of this map
			 */
			template <typename T>
			static inline void deleteAll( Castor::MultiThreading::Set <T *> & p_set)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T *>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend ; ++ i)
				{
					delete * i;
				}

				p_set.clear();
			}
			/**
			 * Deletes the first element and the value associated
			 */
			template <typename T>
			static inline bool deleteFirstValue( Castor::MultiThreading::Set <T *> & p_set)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				T * pValue;
				bool bReturn = p_set.eraseFirstValue( pValue);

				if (bReturn)
				{
					delete pValue;
				}

				return bReturn;
			}
			/**
			 * Deletes the last element and the value associated
			 */
			template <typename T>
			static inline bool deleteLastValue( Castor::MultiThreading::Set <T *> & p_set)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				T * pValue;
				bool bReturn = p_set.eraseLastValue( pValue);

				if (bReturn)
				{
					delete pValue;
				}

				return bReturn;
			}
			/**
			 * Deletes the element and the value at the given key
			 */
			template <typename T>
			static inline bool deleteValue( Castor::MultiThreading::Set <T *> & p_set, T * p_key)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				T * pValue;
				bool bReturn = p_set.eraseValue( p_key, pValue);

				if (bReturn)
				{
					delete pValue;
				}

				return bReturn;
			}
			/**
			 * Applies a function without argument to each element of the set, pointer version
			 */
			template <typename T>
			static inline void cycle( Castor::MultiThreading::Set <T *> & p_set, void( T::* p_func)( void))
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T *>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of the set
			 */
			template <typename T>
			static inline void cycle( Castor::MultiThreading::Set <T> & p_set, void( T::* p_func)( void))
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i).* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the set, pointer version
			 */
			template <typename T, typename Z>
			static inline void cycle( Castor::MultiThreading::Set <T *> & p_set, void( T::* p_func)( Z const &), Z const & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T *>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the set
			 */
			template <typename T, typename Z>
			static inline void cycle( Castor::MultiThreading::Set <T> & p_set, void( T::* p_func)( Z const &), Z const & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the set, pointer version
			 */
			template <typename T, typename V, typename W>
			static inline void cycle( Castor::MultiThreading::Set <T *> & p_set, void( T::* p_func)( V const &, W const &), V const & p_param, W const & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T *>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the set
			 */
			template <typename T, typename V, typename W>
			static inline void cycle( Castor::MultiThreading::Set <T> & p_set, void( T::* p_func)( V const &, W const &), V const & p_param, W const & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i).* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with one argument to each element of the set, pointer version
			 */
			template <typename T, typename Z>
			static inline void cycle( Castor::MultiThreading::Set <T *> & p_set, void( T::* p_func)( Z), Z const & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T *>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of the set
			 */
			template <typename T, typename Z>
			static inline void cycle( Castor::MultiThreading::Set <T> & p_set, void( T::* p_func)( Z), Z const & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_set.GetLocker());
				typename Castor::MultiThreading::Set <T>::iterator i = p_set.begin();
				typename Castor::MultiThreading::Set <T>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i).* p_func)( p_param);
				}
			}


			/**********************************************************************************************
						std::set version
			**********************************************************************************************/
			/**
			 * Gives the element at the given key, nullptr if it doesn't exist
			 */
			template <typename T>
			static inline T * findOrNull( std::set <T *> const & p_set, T const & p_key)
			{
				typename std::set <T *>::const_iterator ifind = p_set.find( p_key);
				T * pReturn = nullptr;

				if (ifind != p_set.end())
				{
					pReturn = * ifind;
				}

				return pReturn;
			}
			/**
			 * Gives the element at the given key, default value if it doesn't exist
			 */
			template <typename T>
			static inline T findOrNull( std::set <T> const & p_set, T const & p_key)
			{
				typename std::set <T>::const_iterator ifind = p_set.find( p_key);
				T tReturn;

				if (ifind != p_set.end())
				{
					tReturn = * ifind;
				}

				return tReturn;
			}
			/**
			 * Tells whether or not the set has an element at the given key
			 */
			template <typename T>
			static inline bool has( std::set <T> const & p_set, T const & p_key)
			{
				return p_set.find( p_key) != p_set.end();
			}
			/**
			 * Erases the element at the given key, doesn't delete the data
			 */
			template <typename T>
			static inline bool eraseValue( std::set <T> & p_set, T const & p_key)
			{
				bool bReturn = false;
				typename std::set <T>::iterator ifind = p_set.find( p_key);

				if (ifind != p_set.end())
				{
					bReturn = true;
					p_set.erase( ifind);
				}

				return bReturn;
			}
			/**
			 * Erases the element pointed by the iterator, doesn't delete the data
			 */
			template <typename T>
			static inline void eraseIterator( std::set <T> & p_set, typename std::set <T> ::iterator & p_where)
			{
				typename std::set <T>::iterator l_erase = p_where;
				++ p_where;
				p_set.erase( l_erase);
			}
			/**
			 * Erases the first element of the list, doesn't delete the data
			 */
			template <typename T>
			static inline bool eraseFirstValue( std::set <T> & p_set, T & tResult)
			{
				bool bReturn = false;

				if (p_set.size() > 0)
				{
					bReturn = true;
					tResult = * p_set.begin();
					p_set.erase( p_set.begin());
				}

				return bReturn;
			}
			/**
			 * Erases the last element of the list, doesn't delete the data
			 */
			template <typename T>
			static inline bool eraseLastValue( std::set <T> & p_set, T & tResult)
			{
				bool bReturn = false;

				if (p_set.size() > 0)
				{
					bReturn = true;
					typename std::set <T>::reverse_iterator i = p_set.rbegin();
					tResult = * i;
					i++;
					p_set.erase( i.base());
				}

				return bReturn;
			}
			/**
			 * Deletes all the members of this set
			 */
			template <typename T>
			static inline void deleteAll( std::set <T *> & p_set)
			{
				typename std::set <T *>::iterator i = p_set.begin();
				typename std::set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend ; ++ i)
				{
					delete * i;
				}

				p_set.clear();
			}
			/**
			 * Deletes the first element and the value associated
			 */
			template <typename T>
			static inline bool deleteFirstValue( std::set <T *> & p_set)
			{
				T * pResult;
				bool bReturn = eraseFirstValue( p_set, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes the last element and the value associated
			 */
			template <typename T>
			static inline bool deleteLastValue( std::set <T *> & p_set)
			{
				T * pResult;
				bool bReturn = eraseLastValue( p_set, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes the element and the value at the given key
			 */
			template <typename T>
			static inline bool deleteValue( std::set <T *> & p_set, T * p_key)
			{
				bool bReturn = eraseValue( p_set, p_key);

				if (bReturn)
				{
					delete p_key;
				}

				return bReturn;
			}
			/**
			 * Applies a function without argument to each element of the set, pointer version
			 */
			template <typename T>
			static inline void cycle( std::set <T *> const & p_set, void( T::* p_func)( void))
			{
				typename std::set <T *>::const_iterator i = p_set.begin();
				typename std::set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of the set
			 */
			template <typename T>
			static inline void cycle( std::set <T> & p_set, void( T::* p_func)( void))
			{
				typename std::set <T> ::const_iterator i = p_set.begin();
				typename std::set <T> ::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i).* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the set, pointer version
			 */
			template <typename T, typename Z>
			static inline void cycle( std::set <T *> const & p_set, void( T::* p_func)( Z const &), Z const & p_param)
			{
				typename std::set <T *> :: const_iterator i = p_set.begin();
				typename std::set <T *> :: const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of the set
			 */
			template <typename T, typename Z>
			static inline void cycle( std::set <T> const & p_set, void( T::* p_func)( Z const &), Z const & p_param)
			{
				typename std::set <T> :: const_iterator i = p_set.begin();
				typename std::set <T> :: const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the set, pointer version
			 */
			template <typename T, typename V, typename W>
			static inline void cycle( std::set <T *> const & p_set, void( T::* p_func)( V const &, W const &), V const & p_param, W const & p_param2)
			{
				typename std::set <T *>::const_iterator i = p_set.begin();
				typename std::set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] argument to each element of the set
			 */
			template <typename T, typename V, typename W>
			static inline void cycle( std::set <T> const & p_set, void( T::* p_func)( V const &, W const &), V const & p_param, W const & p_param2)
			{
				typename std::set <T>::const_iterator i = p_set.begin();
				typename std::set <T>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)( p_param, p_param2);
				}
			}
			/**
			 * Applies a function with one argument to each element of the set, pointer version
			 */
			template <typename T, typename Z>
			static inline void cycle( std::set <T *> const & p_set, void( T::* p_func)( Z), Z const & p_param)
			{
				typename std::set <T *>::const_iterator i = p_set.begin();
				typename std::set <T *>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of the set
			 */
			template <typename T, typename Z>
			static inline void cycle( std::set <T> const & p_set, void( T::* p_func)( Z), Z const & p_param)
			{
				typename std::set <T>::const_iterator i = p_set.begin();
				typename std::set <T>::const_iterator iend = p_set.end();

				for ( ; i != iend; ++ i)
				{
					(( * i).* p_func)( p_param);
				}
			}
			/**
			 * Creates and returns the intersection between two sets
			 */
			template <typename T>
			static inline std::set <T> intersects( std::set <T> const & p_setA, std::set <T> const & p_setB)
			{
				std::set <T> l_set;

				if (p_setA.m_set.empty() || p_setB.m_set.empty())
				{
					return l_set;
				}

				typename std::set <T>::const_iterator i = p_setA.m_set.begin();
				typename std::set <T>::const_iterator j = p_setB.m_set.begin();

				typename std::set <T>::const_iterator iend = p_setA.m_set.end();
				typename std::set <T>::const_iterator jend = p_setB.m_set.end();

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
}

#endif

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
#ifndef ___Castor_List___
#define ___Castor_List___

#include "Mutex.h"
#include <list>

namespace Castor
{
	namespace MultiThreading
	{
		//! Thread safe list
		/*!
		Overloads std::list and overloads all of it's function to replace them with thread safe ones
		\author Sylvain DOREMUS
		\version 0.1
		\date 09/02/2010
		*/
		template <typename Ty>
		class List : public std::list <Ty>, public MemoryTraced< List<Ty> >
		{
		private:
			template <typename _Ty> friend class Castor::MultiThreading::List;

		protected:
			typedef Ty							_listed_type;
			typedef std::list <_listed_type>	_my_list;
			typedef _listed_type				_element;
			typedef _listed_type &				_reference;
			typedef const _listed_type &		_const_reference;
			typedef List <_listed_type>			_my_type;

		public:
			typedef typename _my_list::iterator					iterator;
			typedef typename _my_list::const_iterator			const_iterator;
			typedef typename _my_list::reverse_iterator			reverse_iterator;
			typedef typename _my_list::const_reverse_iterator	const_reverse_iterator;

		private:
			RecursiveMutex m_mutex;

		public:
			List()
			{
			}
			List( const _my_type & listOther)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listOther.GetLocker());
				assign( listOther.begin(), listOther.end());
			}
			explicit List( size_t p_stCount)
				:	_my_list( p_stCount)
			{
			}
			List( size_t p_stCount, _const_reference p_tVal)
				:	_my_list( p_stCount, p_tVal)
			{
			}
			List( _const_reference p_tRight)
				:	_my_list( p_tRight)
			{
			}
			template <class _Iter>
			List(_Iter p_itFirst, _Iter p_itLast)
				:	_my_list( p_itFirst, p_itLast)
			{
			}
			virtual ~List()
			{
				clear();
			}
			virtual iterator begin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::begin();
			}
			virtual const_iterator begin()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::begin();
			}
			virtual iterator end()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::end();
			}
			virtual const_iterator end()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::end();
			}
			virtual reverse_iterator rbegin()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::rbegin();
			}
			virtual const_reverse_iterator rbegin()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::rbegin();
			}
			virtual reverse_iterator rend()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::rend();
			}
			virtual const_reverse_iterator rend()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::rend();
			}
			virtual void resize( size_t p_stNewsize)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::resize( p_stNewsize);
			}
			virtual void resize( size_t p_stNewsize, _listed_type p_tVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::resize( p_stNewsize, p_tVal);
			}
			virtual size_t size()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::size();
			}
			virtual size_t max_size()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::max_size();
			}
			virtual bool empty()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::empty();
			}
			virtual _reference front()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::front();
			}
			virtual _const_reference front()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::front();
			}
			virtual _reference back()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::back();
			}
			virtual _const_reference back()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::back();
			}
			virtual void push_front( _const_reference p_tVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::push_front( p_tVal);
			}
			virtual void pop_front()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::pop_front();
			}
			virtual void push_back( _const_reference p_tVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::push_back( p_tVal);
			}
			virtual void pop_back()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::pop_back();
			}
			template <class _Iter>
			void assign( _Iter p_itFirst, _Iter p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::assign( p_itFirst, p_itLast);
			}
			void assign( size_t p_stCount, _const_reference p_tVal)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::assign( p_stCount, p_tVal);
			}
			void insert( const_iterator p_itWhere, _const_reference p_tElement)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::insert( p_itWhere, p_tElement);
			}
			void insert( const_iterator p_itWhere, size_t p_stCount, _const_reference p_tElement)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::insert( p_itWhere, p_stCount, p_tElement);
			}
			template <class _Iter>
			void insert( const_iterator p_itWhere, _Iter p_itFirst, _Iter p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::insert( p_itWhere, p_itFirst, p_itLast);
			}
			void erase( const_iterator p_itWhere)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::erase( p_itWhere);
			}
			iterator erase( const_iterator p_itFirst, const_iterator p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return _my_list::erase( p_itFirst, p_itLast);
			}
			void remove( _const_reference p_tElement)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::remove( p_tElement);
			}
			template <class _Pr1>
			void remove_if( _Pr1 p_predicate)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::remove_if( p_predicate);
			}
			void clear()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::clear();
			}
			void swap( _my_list lListOther)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::swap( lListOther);
			}
			void splice( const_iterator p_itWhere, _my_list lListOther)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::splice( p_itWhere, lListOther);
			}
			void splice( const_iterator p_itWhere, _my_list lListOther, const_iterator p_itFirst)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::splice( p_itWhere, lListOther, p_itFirst);
			}
			void splice( const_iterator p_itWhere, _my_list lListOther, const_iterator p_itFirst, const_iterator p_itLast)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::splice( p_itWhere, lListOther, p_itFirst, p_itLast);
			}
			void unique()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::unique();
			}
			template <class _Pr2>
			void unique( _Pr2 p_predicate)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::unique( p_predicate);
			}
			void merge( _my_list lListOther)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::merge( lListOther);
			}
			template <class _Pr3>
			void merge( _my_list lListOther, _Pr3 p_predicate)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::merge( lListOther, p_predicate);
			}
			void sort()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::sort();
			}
			template <class _Pr3>
			void sort( _Pr3 p_predicate)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::sort( p_predicate);
			}
			void reverse()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::reverse();
			}
			_my_type & operator =( const _my_type & listOther)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_my_list::clear();
				_my_list::operator =( listOther);
				return * this;
			}
			iterator find( const _listed_type & tKey)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bFound = false;
				iterator it = _my_list::begin();
				const_iterator & iend = _my_list::end();

				while (it != iend && ! bFound)
				{
					if ((* it) == tKey)
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
			const_iterator find( const _listed_type & tKey)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bFound = false;
				const_iterator it = _my_list::begin();
				const_iterator & iend = _my_list::end();

				while (it != iend && ! bFound)
				{
					if ((* it) == tKey)
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
			reverse_iterator rfind( const _listed_type & tKey)
			{
				return reverse_iterator( find( tKey));
			}
			const_reverse_iterator rfind( const _listed_type & tKey)const
			{
				return const_reverse_iterator( find( tKey));
			}
			bool eraseValue( const _listed_type & p_tKey)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool l_bReturn = false;
				size_t i = 0;
				iterator it = _my_list::begin();
				const_iterator iend = _my_list::end();

				while (it != iend && ! l_bReturn)
				{
					if ( * it == p_tKey)
					{
						_my_list::erase( it);
						l_bReturn = true;
					}
					else
					{
						++it;
					}
				}

				return l_bReturn;
			}
			bool eraseValueAt( size_t uiIndex, _listed_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (uiIndex < _my_list::size())
				{
					size_t i = 0;
					iterator it = _my_list::begin();
					const_iterator iend = _my_list::end();

					for ( ; it != iend && i < uiIndex ; ++it, ++i);
					tResult = ( * it);
					bReturn = true;
					_my_list::erase( it);
				}

				return bReturn;
			}
			bool eraseFirstValue( _listed_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_list::size() > 0)
				{
					tResult = * _my_list::begin();
					_my_list::erase( _my_list::begin());
					bReturn = true;
				}

				return bReturn;
			}
			bool eraseLastValue( _listed_type & tResult)
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_list::size() > 0)
				{
					reverse_iterator i = _my_list::rbegin();
					tResult = * i;
					i++;
					_my_list::erase( i.base());
					bReturn = true;
				}

				return bReturn;
			}
			bool has( const _listed_type & p_tValue)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				bool l_bReturn = false;
				const_iterator it = _my_list::begin();
				const_iterator iend = _my_list::end();

				for ( ; it != iend && ! l_bReturn; it++)
				{
					l_bReturn = ( * it == p_tValue);
				}

				return l_bReturn;
			}
			inline _listed_type getOrNull( const size_t p_index)const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				_listed_type tReturn;

				if (p_index < _my_list::size())
				{
					const_iterator it = _my_list::begin();
					const_iterator iend = _my_list::end();

					for ( ; it != iend && iCount < p_index ; ++it);

					tReturn = ( * it);
				}

				return tReturn;
			}
			const RecursiveMutex & GetLocker()const
			{
				return m_mutex;
			}
		};
	}

	namespace Utils
	{
		//! std::list and Castor::MultiThreading::List helper functions
		class list
		{
		public:
			/*********************************************************************************************************************
				Castor::MultiThreading::List Version
			*********************************************************************************************************************/
			/**
			 * Deletes all elements from a list
			 */
			template <typename T>
			static inline void deleteAll( Castor::MultiThreading::List <T *> & listList)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::iterator i = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator & iend = listList.end();

				for ( ; i != iend ; ++ i)
				{
					delete (* i);
				}

				listList.clear();
			}
			/**
			 * Deletes given element from a list
			 */
			template <typename T>
			static inline bool deleteValue( Castor::MultiThreading::List <T *> & listList, T * pKey)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				bool bReturn = listList.eraseValue( pKey);

				if (bReturn)
				{
					delete pKey;
				}

				return bReturn;
			}
			/**
			 * Deletes element at given index from a list
			 */
			template <typename T>
			static inline bool deleteValueAt( Castor::MultiThreading::List <T *> & listList, size_t uiIndex)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				T * pResult;
				bool bReturn = listList.eraseValueAt( uiIndex, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes first element from a list
			 */
			template <typename T>
			static inline bool deleteFirstValue( Castor::MultiThreading::List <T *> & listList)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				T * pResult;
				bool bReturn = listList.eraseFirstValue( pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes last element from a list
			 */
			template <typename T>
			static inline bool deleteLastValue( Castor::MultiThreading::List <T *> & listList)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				T * pResult;
				bool bReturn = listList.eraseLastValue( pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Applies a function without argument to each element of a vector
			 */
			template <typename T>
			static inline void cycle( const Castor::MultiThreading::List <T *> & listList, void( T::* p_func)( void))
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of a vector
			 */
			template <typename T>
			static inline void cycle( const Castor::MultiThreading::List <T> & listList, void( T::* p_func)( void))
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const Castor::MultiThreading::List <T *> & listList, void( T::* p_func)( const U &), const U & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const Castor::MultiThreading::List <T> & listList, void( T::* p_func)( const U &), const U & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const Castor::MultiThreading::List <T *> & listList, void( T::* p_func)( U &), U & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const Castor::MultiThreading::List <T> & listList, void( T::* p_func)( U &), U & p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const Castor::MultiThreading::List <T *> & listList, void( T::* p_func)( U), U p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const Castor::MultiThreading::List <T> & listList, void( T::* p_func)( U), U p_param)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const Castor::MultiThreading::List <T *> & listList, void( T::* p_func)( const U &, const V &), const U & p_param1, const V & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const Castor::MultiThreading::List <T> & listList, void( T::* p_func)( const U &, const V &), const U & p_param1, const V & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const Castor::MultiThreading::List <T *> & listList, void( T::* p_func)( U &, V &), U & p_param1, V & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const Castor::MultiThreading::List <T> & listList, void( T::* p_func)( U &, V &), U & p_param1, V & p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const Castor::MultiThreading::List <T *> & listList, void( T::* p_func)( U, V), U p_param1, V p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T *>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const Castor::MultiThreading::List <T> & listList, void( T::* p_func)( U, V), U p_param1, V p_param2)
			{
				CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( listList.GetLocker());
				typename Castor::MultiThreading::List <T>::const_iterator it = listList.begin();
				typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param1, p_param2);
				}
			}



			/*********************************************************************************************************************
				std::list Version
			*********************************************************************************************************************/
			/**
			 * Searches an element in the list returns it's iterator
			 */
			template <class T>
			typename std::list <T>::iterator find( std::list <T> & lList, const T & tKey)
			{
				bool bFound = false;
				typename std::list <T>::iterator it = lList.begin();

				while (it != lList.end() && ! bFound)
				{
					if ((* it) == tKey)
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
			 * Searches an element in the list returns it's const iterator
			 */
			template <class T>
			typename std::list <T>::const_iterator find( const std::list <T> & lList, const T & tKey)
			{
				bool bFound = false;
				typename std::list <T>::const_iterator it = lList.begin();

				while (it != lList.end() && ! bFound)
				{
					if ((* it) == tKey)
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
			 * Searches an element in the list returns it's reverse iterator
			 */
			template <class T>
			typename std::list <T>::reverse_iterator rfind( std::list <T> & lList, const T & tKey)
			{
				return typename std::list <T>::reverse_iterator( find( lList, tKey));
			}
			/**
			 * Searches an element in the list returns it's reverse iterator
			 */
			template <class T>
			typename std::list <T>::const_reverse_iterator rfind( const std::list <T> & lList, const T & tKey)
			{
				return typename std::list <T>::const_reverse_iterator( find( lList, tKey));
			}
			/**
			 * Erases the given value from the list
			 */
			template <typename T>
			static inline bool eraseValue( std::list <T> & listList, const T & p_tKey)
			{
				bool l_bReturn = false;
				size_t i = 0;
				typename std::list <T>::iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				while (it < iend && ! l_bReturn)
				{
					if ( * it == p_tKey)
					{
						listList.erase( it);
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
			 * Erases the value at the given index from the list
			 */
			template <typename T>
			static inline bool eraseValueAt( std::list <T> & listList, size_t uiIndex, T & tResult)
			{
				bool bReturn = false;

				if (uiIndex < listList.size())
				{
					size_t i = 0;
					typename Castor::MultiThreading::List <T>::iterator it = listList.begin();
					typename Castor::MultiThreading::List <T>::const_iterator iend = listList.end();

					for ( ; it != iend && i < uiIndex ; ++it, ++i);
					tResult = ( * it);
					listList.erase( it);
					bReturn = true;
				}

				return bReturn;
			}
			/**
			 * Erases the first value from the list
			 */
			template <typename T>
			static inline bool eraseFirstValue( std::list <T> & listList, T & tResult)
			{
				bool bReturn = false;

				if (listList.size() > 0)
				{
					bReturn = true;
					tResult = * listList.begin();
					listList.erase( listList.begin());
				}

				return bReturn;
			}
			/**
			 * Erases the last value from the list
			 */
			template <typename T>
			static inline bool eraseLastValue( std::list <T> & listList, T & tResult)
			{
				bool bReturn = false;

				if (listList.size() > 0)
				{
					bReturn = true;
					std::list <T>::reverse_iterator i = listList.rbegin();
					tResult = * i;
					i++;
					listList.erase( i.base());
				}

				return bReturn;
			}
			/**
			 * Deletes all elements from a list
			 */
			template <typename T>
			static inline void deleteAll( std::list <T *> & lList)
			{
				typename std::list <T *> ::iterator i = lList.begin();
				const typename std::list <T *> ::iterator & iend = lList.end();

				for ( ; i != iend ; ++ i)
				{
					delete (* i);
				}

				lList.clear();
			}
			/**
			 * Deletes given element from a list
			 */
			template <typename T>
			static inline bool deleteValue( std::list <T *> & lList, T * pKey)
			{
				bool bReturn = eraseValue( lList, pKey);

				if (bReturn)
				{
					delete pKey;
				}

				return bReturn;
			}
			/**
			 * Deletes element at given index from a list
			 */
			template <typename T>
			static inline bool deleteValueAt( std::list <T *> & lList, size_t uiIndex)
			{
				T * pResult;
				bool bReturn = eraseValueAt( lList, uiIndex, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes first element from a list
			 */
			template <typename T>
			static inline bool deleteFirstValue( std::list <T *> & lList)
			{
				T * pResult;
				bool bReturn = eraseFirstValue( lList, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes last element from a list
			 */
			template <typename T>
			static inline bool deleteLastValue( std::list <T *> & lList)
			{
				T * pResult;
				bool bReturn = eraseLastValue( lList, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Tells if the list contains a given element
			 */
			template <typename T>
			static inline bool has( const std::list <T> & listList, const T & p_tValue)
			{
				bool l_bReturn = false;
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend && ! l_bReturn; it++)
				{
					l_bReturn = ( * it == p_tValue);
				}

				return l_bReturn;
			}
			/**
			 * Gets the value at given index from a vector, NULL if not found
			 */
			template <typename T>
			static inline T * getOrNull( const std::list <T *> & listList, const size_t p_index)
			{
				T * pReturn = NULL;

				if (p_index < listList.size())
				{
					typename std::list <T *>::const_iterator it = listList.begin();
					typename std::list <T *>::const_iterator iend = listList.end();

					for ( ; it != iend && iCount < p_index ; ++it);

					pReturn = ( * it);
				}

				return pReturn;
			}
			/**
			 * Applies a function without argument to each element of a vector, pointer version
			 */
			template <typename T>
			static inline void cycle( const std::list <T *> & listList, void( T::* p_func)( void))
			{
				typename std::list <T *>::const_iterator it = listList.begin();
				typename std::list <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of a vector
			 */
			template <typename T>
			static inline void cycle( const std::list <T> & listList, void( T::* p_func)( void))
			{
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector, pointer version
			 */
			template <typename T, typename U>
			static inline void cycle( const std::list <T *> & listList, void( T::* p_func)( const U &), const U & p_param)
			{
				typename std::list <T *>::const_iterator it = listList.begin();
				typename std::list <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const std::list <T> & listList, void( T::* p_func)( const U &), const U & p_param)
			{
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector, pointer version
			 */
			template <typename T, typename U>
			static inline void cycle( const std::list <T *> & listList, void( T::* p_func)( U &), U & p_param)
			{
				typename std::list <T *>::const_iterator it = listList.begin();
				typename std::list <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const std::list <T> & listList, void( T::* p_func)( U &), U & p_param)
			{
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector, pointer version
			 */
			template <typename T, typename U>
			static inline void cycle( const std::list <T *> & listList, void( T::* p_func)( U), U p_param)
			{
				typename std::list <T *>::const_iterator it = listList.begin();
				typename std::list <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector
			 */
			template <typename T, typename U>
			static inline void cycle( const std::list <T> & listList, void( T::* p_func)( U), U p_param)
			{
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector, pointer version
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const std::list <T *> & listList, void( T::* p_func)( const U &, const V &), const U & p_param1, const V & p_param2)
			{
				typename std::list <T *>::const_iterator it = listList.begin();
				typename std::list <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const std::list <T> & listList, void( T::* p_func)( const U &, const V &), const U & p_param1, const V & p_param2)
			{
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector, pointer version
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const std::list <T *> & listList, void( T::* p_func)( U &, V &), U & p_param1, V & p_param2)
			{
				typename std::list <T *>::const_iterator it = listList.begin();
				typename std::list <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const std::list <T> & listList, void( T::* p_func)( U &, V &), U & p_param1, V & p_param2)
			{
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector, pointer version
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const std::list <T *> & listList, void( T::* p_func)( U, V), U p_param1, V p_param2)
			{
				typename std::list <T *>::const_iterator it = listList.begin();
				typename std::list <T *>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it)->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector
			 */
			template <typename T, typename U, typename V>
			static inline void cycle( const std::list <T> & listList, void( T::* p_func)( U, V), U p_param1, V p_param2)
			{
				typename std::list <T>::const_iterator it = listList.begin();
				typename std::list <T>::const_iterator iend = listList.end();

				for ( ; it != iend ; ++it)
				{
					(( * it).* p_func)( p_param1, p_param2);
				}
			}
		};
	}
}

#endif
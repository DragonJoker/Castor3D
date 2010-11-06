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
#ifndef ___Castor_Vector___
#define ___Castor_Vector___

#include "Mutex.h"
#include <vector>

namespace Castor
{
	namespace MultiThreading
	{
		template <typename Ty, class Locker=MultiThreading::Mutex>
		class Vector : public std::vector <Ty>
		{
		private:
			template <typename _Ty, class _Locker> friend class Castor::MultiThreading::Vector;

		protected:
			typedef Ty										_arrayed_type;
			typedef _arrayed_type &							_reference;
			typedef const _arrayed_type &					_const_reference;
			typedef std::vector<_arrayed_type>				_my_vector;
			typedef Castor::MultiThreading::Vector<_arrayed_type, Locker>	_my_type;

		public:
			typedef typename _my_vector::iterator				iterator;
			typedef typename _my_vector::const_iterator			const_iterator;
			typedef typename _my_vector::reverse_iterator		reverse_iterator;
			typedef typename _my_vector::const_reverse_iterator	const_reverse_iterator;

		private:
			Locker m_locker;
			Locker m_externalLock;

		public:
			Vector()
			{
			}
			Vector( const _my_type & arrayOther)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				assign( arrayOther.begin(), arrayOther.end());
			}
			Vector( size_t p_stCount, _arrayed_type p_tVal)
				:	_my_vector( p_stCount, p_tVal)
			{
			}
			template<class _Iter>
			Vector( _Iter p_itFirst, _Iter p_itLast)
				:	_my_vector( p_itFirst, p_itLast)
			{
			}
			virtual ~Vector()
			{
				clear();
			}
			virtual void reserve( size_t p_stCount)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::reserve( p_stCount);
			}
			virtual size_t capacity()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::capacity();
			}
			virtual iterator begin()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::begin();
			}
			virtual const_iterator begin()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::begin();
			}
			virtual iterator end()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::end();
			}
			virtual const_iterator end()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::end();
			}
			virtual reverse_iterator rbegin()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::rbegin();
			}
			virtual const_reverse_iterator rbegin()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::rbegin();
			}
			virtual reverse_iterator rend()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::rend();
			}
			virtual const_reverse_iterator rend()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::rend();
			}
			virtual void resize( size_t p_stNewsize)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::resize( p_stNewsize);
			}
			virtual void resize( size_t p_stNewsize, _arrayed_type p_tVal)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::resize( p_stNewsize, p_tVal);
			}
			virtual size_t size()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::size();
			}
			virtual size_t max_size()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::max_size();
			}
			virtual bool empty()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::empty();
			}
			virtual _const_reference at( size_t p_stOff)const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::at( p_stOff);
			}
			virtual _reference at( size_t p_stOff)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::at( p_stOff);
			}
			virtual _const_reference operator []( size_t p_stOff)const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::operator []( p_stOff);
			}
			virtual _reference operator []( size_t p_stOff)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::operator []( p_stOff);
			}
			template <typename _Ty>
			bool operator ==( const Castor::MultiThreading::Vector <_Ty> & arrayVector)const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				CASTOR_SCOPED_LOCK( arrayVector.m_criticalSection);
				size_t l_uiSize = _my_vector::size();
				bool l_bReturn = (typeid( _arrayed_type) == typeid( _Ty)) && (l_uiSize == _my_vector( arrayVector).size());
				size_t i = 0;

				while (i < l_uiSize && l_bReturn)
				{
					l_bReturn = (_my_vector::at( i) == _my_vector( arrayVector).at( i));
					i++;
				}

				return l_bReturn;
			}
			template <typename _Ty>
			bool operator !=( const Castor::MultiThreading::Vector <_Ty> & arrayVector)const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				CASTOR_SCOPED_LOCK( arrayVector.m_criticalSection);
				size_t l_uiSize = _my_vector::size();
				bool l_bReturn = (typeid( _arrayed_type) != typeid( _Ty)) || (l_uiSize != _my_vector( arrayVector).size());
				size_t i = 0;

				while (i < l_uiSize && ! l_bReturn)
				{
					l_bReturn = (_my_vector::at( i) != _my_vector( arrayVector).at( i));
					i++;
				}

				return l_bReturn;
			}
			_my_type & operator =( const _my_type & arrayOther)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				CASTOR_SCOPED_LOCK( arrayVector.m_criticalSection);
				_my_vector::operator =( _my_vector( arrayVector));
				return * this;
			}
			virtual _reference front()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::front();
			}
			virtual _const_reference front()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::front();
			}
			virtual _reference back()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::back();
			}
			virtual _const_reference back()const
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::back();
			}
			virtual void push_back( _arrayed_type p_tVal)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::push_back( p_tVal);
			}
			virtual void pop_back()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::pop_back();
			}
			template <class _Iter>
			void assign( _Iter p_itFirst, _Iter p_itLast)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::assign( p_itFirst, p_itLast);
			}
			virtual void assign( size_t p_itCount, _arrayed_type p_tVal)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::assign( p_itCount, p_tVal);
			}
			virtual iterator insert( const_iterator p_itWhere, _arrayed_type p_tVal)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::insert( p_itWhere, p_tVal);
			}
			virtual void insert( const_iterator p_itWhere, size_t p_stCount, _arrayed_type p_tVal)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::insert( p_itWhere, p_stCount, p_tVal);
			}
			template <class _Iter>
			void insert( const_iterator p_itWhere, _Iter p_itFirst, _Iter p_itLast)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::insert( p_itWhere, p_itFirst, p_itLast);
			}
			virtual iterator erase( const_iterator p_itWhere)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::erase( p_itWhere);
			}
			virtual iterator erase( const_iterator p_itFirst, const_iterator p_itLast)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				return _my_vector::erase( p_itFirst, p_itLast);
			}
			virtual void clear()
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::clear();
			}
			virtual void swap( _my_vector & p_vRight)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_my_vector::swap( p_vRight);
			}
			bool eraseValue( const _arrayed_type & p_tKey)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				bool l_bReturn = false;
				size_t i = 0;

				while (i < _my_vector::size() && ! l_bReturn)
				{
					if (_my_vector::at( i) == p_tKey)
					{
						_my_vector::erase( _my_vector::begin() + i);
						l_bReturn = true;
					}
					else
					{
						++i;
					}
				}

				return l_bReturn;
			}
			bool eraseValueAt( size_t uiIndex, _arrayed_type & tResult)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (uiIndex < _my_vector::size())
				{
					tResult = _my_vector::at( uiIndex);
					bReturn = true;
					_my_vector::erase( _my_vector::begin() + uiIndex);
				}

				return bReturn;
			}
			bool eraseFirstValue( _arrayed_type & tResult)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_vector::size() > 0)
				{
					tResult = * _my_vector::begin();
					bReturn = true;
					_my_vector::erase( _my_vector::begin());
				}

				return bReturn;
			}
			bool eraseLastValue( _arrayed_type & tResult)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				bool bReturn = false;

				if (_my_vector::size() > 0)
				{
					reverse_iterator i = _my_vector::rbegin();
					tResult = * i;
					i++;
					_my_vector::erase( i.base());
					bReturn = true;
				}

				return bReturn;
			}
			bool has( const _arrayed_type & p_tValue)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				bool l_bReturn = false;

				for (size_t i = 0 ; i < _my_vector::size() && ! l_bReturn; i++)
				{
					l_bReturn = (_my_vector::at( i) == p_tValue);
				}

				return l_bReturn;
			}
			_arrayed_type getOrNull( const size_t p_index)
			{
				CASTOR_AUTO_SCOPED_LOCK();
				_arrayed_type pReturn;

				if (p_index < _my_vector::size())
				{
					pReturn = _my_vector::at( p_index);
				}

				return pReturn;
			}
			const Locker & GetLocker()const
			{
				return m_externalLock;
			}
		};
	}

	namespace Utils
	{
		class vector
		{
		public:
			/**********************************************************************************************
						Castor::Vector version
			**********************************************************************************************/
			/**
			 * Deletes all element in the vector
			 */
			template <typename T, class Locker>
			static inline void deleteAll( Castor::MultiThreading::Vector <T *, Locker> & arrayVector)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t uiMax = arrayVector.size();

				for (size_t i = 0 ; i < uiMax ; ++i)
				{
					delete arrayVector[i];
				}

				arrayVector.clear();
			}
			/**
			 * Deletes given element from a vector
			 */
			template <typename T, class Locker>
			static inline bool deleteValue( Castor::MultiThreading::Vector <T *, Locker> & arrayVector, T * pKey)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				bool bReturn = false;

				if (arrayVector.eraseValue( pKey))
				{
					bReturn = true;
					delete pKey;
				}

				return bReturn;
			}
			/**
			 * Deletes element at given index from a vector
			 */
			template <typename T, class Locker>
			static inline bool deleteValueAt( Castor::MultiThreading::Vector <T *, Locker> & arrayVector, size_t uiIndex)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				T * pResult;
				bool bReturn = false;

				if (arrayVector.eraseValueAt( uiIndex, pResult))
				{
					bReturn = true;
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes first element from a vector
			 */
			template <typename T, class Locker>
			static inline bool deleteFirstValue( Castor::MultiThreading::Vector <T *, Locker> & arrayVector)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				T * pResult;
				bool bReturn = false;

				if (arrayVector.eraseFirstValue( pResult))
				{
					bReturn = true;
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes last element from a vector
			 */
			template <typename T, class Locker>
			static inline bool deleteLastValue( Castor::MultiThreading::Vector <T *, Locker> & arrayVector)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				T * pResult;
				bool bReturn = false;

				if (arrayVector.eraseLastValue( pResult))
				{
					bReturn = true;
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Applies a function without argument to each element of a vector
			 */
			template <typename Type, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type *, Locker> & arrayVector, void( Type::* p_func)( void))
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of a vector
			 */
			template <typename Type, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type, Locker> & arrayVector, void( Type::* p_func)( void))
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector
			 */
			template <typename Type, typename Param, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type *, Locker> & arrayVector, void( Type::* p_func)( const Param &), const Param & p_param)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector
			 */
			template <typename Type, typename Param, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type, Locker> & arrayVector, void( Type::* p_func)( const Param &), const Param & p_param)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector
			 */
			template <typename Type, typename Param, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type *, Locker> & arrayVector, void( Type::* p_func)( Param &), Param & p_param)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector
			 */
			template <typename Type, typename Param, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type, Locker> & arrayVector, void( Type::* p_func)( Param &), Param & p_param)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector
			 */
			template <typename Type, typename Param, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type *, Locker> & arrayVector, void( Type::* p_func)( Param), Param p_param)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector
			 */
			template <typename Type, typename Param, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type, Locker> & arrayVector, void( Type::* p_func)( Param), Param p_param)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type *, Locker> & arrayVector, void( Type::* p_func)( const Param1 &, const Param2 &), const Param1 & p_param1, const Param2 & p_param2)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type, Locker> & arrayVector, void( Type::* p_func)( const Param1 &, const Param2 &), const Param1 & p_param1, const Param2 & p_param2)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type *, Locker> & arrayVector, void( Type::* p_func)( Param1 &, Param2 &), Param1 & p_param1, Param2 & p_param2)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type, Locker> & arrayVector, void( Type::* p_func)( Param1 &, Param2 &), Param1 & p_param1, Param2 & p_param2)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type *, Locker> & arrayVector, void( Type::* p_func)( Param1, Param2), Param1 p_param1, Param2 p_param2)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2, class Locker>
			static inline void cycle( const Castor::MultiThreading::Vector <Type, Locker> & arrayVector, void( Type::* p_func)( Param1, Param2), Param1 p_param1, Param2 p_param2)
			{
				CASTOR_SCOPED_LOCK( arrayVector.GetLocker());
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param1, p_param2);
				}
			}


			/**********************************************************************************************
						std::vector version
			**********************************************************************************************/
			/**
			 * Tells if the vector contains a given element
			 */
			template <typename T>
			static inline bool has( const std::vector <T> & arrayVector, const T & p_tValue)
			{
				bool l_bReturn = false;

				for (size_t i = 0 ; i < arrayVector.size() && ! l_bReturn; i++)
				{
					l_bReturn = (arrayVector[i] == p_tValue);
				}

				return l_bReturn;
			}
			/**
			 * Gets the value at given index from a vector, NULL if not found
			 */
			template <typename T>
			static inline T * getOrNull( const std::vector <T *> & arrayVector, const size_t p_index)
			{
				T * pReturn = NULL;

				if (p_index < arrayVector.size())
				{
					pReturn = arrayVector[p_index];
				}

				return pReturn;
			}
			/**
			 * Erases the given value from the vector
			 */
			template <typename T>
			static inline bool eraseValue( std::vector <T> & arrayVector, const T & p_tKey)
			{
				bool l_bReturn = false;
				size_t i = 0;

				while (i < arrayVector.size() && ! l_bReturn)
				{
					if (arrayVector[i] == p_tKey)
					{
						arrayVector.erase( arrayVector.begin() + i);
						l_bReturn = true;
					}
					else
					{
						++i;
					}
				}

				return l_bReturn;
			}
			/**
			 * Erases the given value from the vector
			 */
			template <typename T>
			static inline bool eraseValueAt( std::vector <T> & arrayVector, size_t uiIndex, T & tResult)
			{
				bool l_bReturn = false;

				if (uiIndex < arrayVector.size())
				{
					l_bReturn = true;
					tResult = arrayVector[uiIndex];
					arrayVector.erase( arrayVector.begin() + uiIndex);
				}

				return l_bReturn;
			}
			/**
			 * Erases the first value from the vector
			 */
			template <typename T>
			static inline bool eraseFirstValue( std::vector <T> & arrayVector, T & tResult)
			{
				bool bReturn = false;

				if (arrayVector.size() > 0)
				{
					bReturn = true;
					tResult = * arrayVector.begin();
					arrayVector.erase( arrayVector.begin());
				}

				return bReturn;
			}
			/**
			 * Erases the last value from the vector
			 */
			template <typename T>
			static inline bool eraseLastValue( std::vector <T> & arrayVector, T & tResult)
			{
				bool bReturn = false;

				if (arrayVector.size() > 0)
				{
					typename std::vector <T>::reverse_iterator i = arrayVector.rbegin();
					bReturn = true;
					tResult = * i;
					i++;
					arrayVector.erase( i.base());
				}

				return bReturn;
			}
			/**
			 * Deletes all element in the vector
			 */
			template <typename T>
			static inline void deleteAll( std::vector <T *> & arrayVector)
			{
				std::vector <T *>::iterator l_it;
				std::vector <T *>::const_iterator l_itEnd = arrayVector.end();

				for (l_it = arrayVector.begin() ; l_it != l_itEnd ; ++l_it)
				{
					delete ( * l_it);
				}

				arrayVector.clear();
			}
			/**
			 * Deletes given element from a vector
			 */
			template <typename T>
			static inline bool deleteValue( std::vector <T *> & arrayVector, T * pKey)
			{
				bool bReturn = eraseValue( arrayVector, pKey);

				if (bReturn)
				{
					delete pKey;
				}

				return bReturn;
			}
			/**
			 * Deletes element at given index from a vector
			 */
			template <typename T>
			static inline bool deleteValueAt( std::vector <T *> & arrayVector, size_t uiIndex)
			{
				T * pResult;
				bool bReturn = eraseValueAt( arrayVector, uiIndex, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes first element from a vector
			 */
			template <typename T>
			static inline bool deleteFirstValue( std::vector <T *> & arrayVector)
			{
				T * pResult;
				bool bReturn = eraseFirstValue( arrayVector, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Deletes last element from a vector
			 */
			template <typename T>
			static inline bool deleteLastValue( std::vector <T *> & arrayVector)
			{
				T * pResult;
				bool bReturn = eraseLastValue( arrayVector, pResult);

				if (bReturn)
				{
					delete pResult;
				}

				return bReturn;
			}
			/**
			 * Applies a function without argument to each element of a vector
			 */
			template <typename Type>
			static inline void cycle( const std::vector <Type *> & arrayVector, void( Type::* p_func)( void))
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)();
				}
			}
			/**
			 * Applies a function without argument to each element of a vector
			 */
			template <typename Type>
			static inline void cycle( const std::vector <Type> & arrayVector, void( Type::* p_func)( void))
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)();
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector
			 */
			template <typename Type, typename Param>
			static inline void cycle( const std::vector <Type *> & arrayVector, void( Type::* p_func)( const Param &), const Param & p_param)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in] argument to each element of a vector
			 */
			template <typename Type, typename Param>
			static inline void cycle( const std::vector <Type> & arrayVector, void( Type::* p_func)( const Param &), const Param & p_param)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector
			 */
			template <typename Type, typename Param>
			static inline void cycle( const std::vector <Type *> & arrayVector, void( Type::* p_func)( Param &), Param & p_param)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one [in/out] argument to each element of a vector
			 */
			template <typename Type, typename Param>
			static inline void cycle( const std::vector <Type> & arrayVector, void( Type::* p_func)( Param &), Param & p_param)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector
			 */
			template <typename Type, typename Param>
			static inline void cycle( const std::vector <Type *> & arrayVector, void( Type::* p_func)( Param), Param p_param)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with one argument to each element of a vector
			 */
			template <typename Type, typename Param>
			static inline void cycle( const std::vector <Type> & arrayVector, void( Type::* p_func)( Param), Param p_param)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2>
			static inline void cycle( const std::vector <Type *> & arrayVector, void( Type::* p_func)( const Param1 &, const Param2 &), const Param1 & p_param1, const Param2 & p_param2)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2>
			static inline void cycle( const std::vector <Type> & arrayVector, void( Type::* p_func)( const Param1 &, const Param2 &), const Param1 & p_param1, const Param2 & p_param2)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2>
			static inline void cycle( const std::vector <Type *> & arrayVector, void( Type::* p_func)( Param1 &, Param2 &), Param1 & p_param1, Param2 & p_param2)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two [in/out] arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2>
			static inline void cycle( const std::vector <Type> & arrayVector, void( Type::* p_func)( Param1 &, Param2 &), Param1 & p_param1, Param2 & p_param2)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2>
			static inline void cycle( const std::vector <Type *> & arrayVector, void( Type::* p_func)( Param1, Param2), Param1 p_param1, Param2 p_param2)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i]->* p_func)( p_param1, p_param2);
				}
			}
			/**
			 * Applies a function with two arguments to each element of a vector
			 */
			template <typename Type, typename Param1, typename Param2>
			static inline void cycle( const std::vector <Type> & arrayVector, void( Type::* p_func)( Param1, Param2), Param1 p_param1, Param2 p_param2)
			{
				size_t imax = arrayVector.size();

				for (size_t i = 0 ; i < imax ; i ++)
				{
					(arrayVector[i].* p_func)( p_param1, p_param2);
				}
			}
		};
	}
}

#endif
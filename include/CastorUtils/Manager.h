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
#ifndef ___Castor_Manager___
#define ___Castor_Manager___

#include "SmartPtr.h"
#include "Value.h"

namespace Castor
{ namespace Templates
{
	//! Eleement manager class
	/*!
	A collection class, allowing you to store named objects, removing, finding or adding them as you wish.
	Autodeletion of all the contained items on deletion of the manager.
	*/
	template <typename TObj>
	class Manager
	{
	public:
		typedef shared_ptr<TObj> TObjPtr;
		typedef typename KeyedContainer<String, TObjPtr>::Map TypeMap;
		TypeMap m_objectMap;
		Castor::MultiThreading::RecursiveMutex m_mutex;

	public:
		Manager()
		{
		}
		~Manager()
		{
			Clear();
		}

	private:
		Manager( const Manager &)
		{}
		const Manager & operator =( const Manager &)
		{ return * this; }

	public:
		void Clear() throw()
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			ClearContainer( m_objectMap);
		}
		TObjPtr GetElement( const String & p_key)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			TObjPtr l_pReturn;

			if (HasElement( p_key))
			{
				l_pReturn = m_objectMap.find( p_key)->second;
			}

			return l_pReturn;
		}
/*
		TObjPtr CreateElement( const String & p_key)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			TObjPtr l_pReturn;
			typename TypeMap::iterator ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( this, p_key));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam>
		TObjPtr CreateElement( const String & p_key, typename CallTraits<TParam>::ParamType p_param)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			TObjPtr l_pReturn;
			typename TypeMap::iterator ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( this, p_key, p_param));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam1, typename TParam2>
		TObjPtr CreateElement( const String & p_key, typename CallTraits<TParam1>::ParamType p_param1, typename CallTraits<TParam2>::ParamType p_param2)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			TObjPtr l_pReturn;
			typename TypeMap::iterator ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( this, p_key, p_param1, p_param2));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam1, typename TParam2, typename TParam3>
		TObjPtr CreateElement( const String & p_key, typename CallTraits<TParam1>::ParamType p_param1, typename CallTraits<TParam2>::ParamType p_param2, typename CallTraits<TParam3>::ParamType p_param3)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			TObjPtr l_pReturn;
			typename TypeMap::iterator ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( this, p_key, p_param1, p_param2, p_param3));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam1, typename TParam2, typename TParam3, typename TParam4>
		TObjPtr CreateElement( const String & p_key, typename CallTraits<TParam1>::ParamType p_param1, typename CallTraits<TParam2>::ParamType p_param2, typename CallTraits<TParam3>::ParamType p_param3, typename CallTraits<TParam4>::ParamType p_param4)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			TObjPtr l_pReturn;
			typename TypeMap::iterator ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( this, p_key, p_param1, p_param2, p_param3, p_param4));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
*/
		bool AddElement( TObjPtr p_element)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			typename TypeMap::iterator ifind = m_objectMap.find( p_element->GetName());

			if (ifind != m_objectMap.end())
			{
				return false;
			}

			m_objectMap.insert( typename TypeMap::value_type( p_element->GetName(), p_element));
			return true;
		}
		bool HasElement( const String & p_key)const
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			return m_objectMap.find( p_key) != m_objectMap.end();
		}
		bool HasElement( TObjPtr p_element)const
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			return m_objectMap.find( p_element->GetName()) != m_objectMap.end();
		}
		TObjPtr RemoveElement( const String & p_key)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			TObjPtr l_ret;
			typename TypeMap::const_iterator ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_ret = ifind->second;
				m_objectMap.erase( p_key);
			}

			return l_ret;
		}
		TObjPtr RemoveElement( TObjPtr p_element)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			return RemoveElement( p_element->GetName());
		}
		TObjPtr GetElementByName( const String & p_key)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			return Utils::map::findOrNull( m_objectMap, p_key);
		}
		TObjPtr DestroyElement( TObjPtr & p_element)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			return RemoveElement( p_element);
		}
	};
}
}

#endif

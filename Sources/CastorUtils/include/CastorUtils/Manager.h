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

#include "SharedPtr.h"
#include "Value.h"

namespace Castor
{ namespace Templates
{
	/*!
	A collection class, allowing you to store named objects, removing, finding or adding them as you wish.
	Autodeletion of all the contained items on deletion of the manager.
	*/
	template <typename Key, typename TObj>
	class Manager
	{
	public:
		typedef SharedPtr<TObj> TObjPtr;
		typedef typename CallTraits<Key>::ParamType KeyParamType;
		typedef C3DMap( Key, TObjPtr) TypeMap;
		TypeMap m_objectMap;

	public:
		Manager(){}
		~Manager()
		{
			Clear();
		}

	private:
		Manager( const Manager &);
		const Manager & operator =( const Manager &);

	public:
		void Clear() throw()
		{
			m_objectMap.clear();
		}
		TObjPtr CreateElement( KeyParamType p_key)
		{
			TObjPtr l_pReturn;
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( p_key));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam>
		TObjPtr CreateElement( KeyParamType p_key, typename CallTraits<TParam>::ParamType p_param)
		{
			TObjPtr l_pReturn;
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( p_key, p_param));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam1, typename TParam2>
		TObjPtr CreateElement( KeyParamType p_key, typename CallTraits<TParam1>::ParamType p_param1, typename CallTraits<TParam2>::ParamType p_param2)
		{
			TObjPtr l_pReturn;
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( p_key, p_param1, p_param2));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam1, typename TParam2, typename TParam3>
		TObjPtr CreateElement( KeyParamType p_key, typename CallTraits<TParam1>::ParamType p_param1, typename CallTraits<TParam2>::ParamType p_param2, typename CallTraits<TParam3>::ParamType p_param3)
		{
			TObjPtr l_pReturn;
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( p_key, p_param1, p_param2, p_param3));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		template<typename TParam1, typename TParam2, typename TParam3, typename TParam4>
		TObjPtr CreateElement( KeyParamType p_key, typename CallTraits<TParam1>::ParamType p_param1, typename CallTraits<TParam2>::ParamType p_param2, typename CallTraits<TParam3>::ParamType p_param3, typename CallTraits<TParam4>::ParamType p_param4)
		{
			TObjPtr l_pReturn;
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_pReturn = ifind->second;
			}
			else
			{
				l_pReturn = TObjPtr( new TObj( p_key, p_param1, p_param2, p_param3, p_param4));
				m_objectMap.insert( typename TypeMap::value_type( p_key, l_pReturn));
			}

			return l_pReturn;
		}
		bool AddElement( TObjPtr p_element)
		{
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_element->GetKey());

			if (ifind != m_objectMap.end())
			{
				return false;
			}

			m_objectMap.insert( typename TypeMap::value_type( p_element->GetKey(), p_element));
			return true;
		}
		bool HasElement( KeyParamType p_key)const
		{
			return m_objectMap.find( p_key) != m_objectMap.end();
		}
		bool HasElement( TObjPtr p_element)const
		{
			return m_objectMap.find( p_element->GetKey()) != m_objectMap.end();
		}
		TObjPtr RemoveElement( KeyParamType p_key)
		{
			TObjPtr l_ret;
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_key);

			if (ifind != m_objectMap.end())
			{
				l_ret = ifind->second;
				m_objectMap.erase( ifind);
			}

			return l_ret;
		}
		TObjPtr RemoveElement( TObjPtr p_element)
		{
			return RemoveElement( p_element->GetKey());
		}
		TObjPtr GetElementByName( KeyParamType p_key)
		{
			return Utils::map::findOrNull( m_objectMap, p_key);
		}
		TObjPtr DestroyElement( TObjPtr & p_element)
		{
			return RemoveElement( p_element);
		}
	};

	template <typename Key, typename TObj>
	class Managed
	{
	public:
		typedef typename CallTraits<Key>::ParamType KeyParamType;
		friend class Manager <Key, TObj>;

	protected:
		Key m_key;

	public:
		Managed( KeyParamType p_key)
			:	m_key( p_key)
		{}
		~Managed()
		{}
		inline KeyParamType GetKey()const { return m_key; }
	};
}
}

#endif

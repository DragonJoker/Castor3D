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

namespace Castor
{ namespace Templates
{
	/*!
	A collection class, allowing you to store named objects, removing, finding or adding them as you wish.
	Autodeletion of all the contained items on deletion of the manager.
	*/
	template <typename TObj>
	class Manager
	{
	public:
		typedef SharedPtr<TObj> TObjPtr;
		typedef C3DMap( String, TObjPtr) TypeMap;
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
//			Castor::map::deleteAll( m_objectMap);
		}
		bool AddElement( TObjPtr p_element)
		{
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_element->GetName());

			if (ifind != m_objectMap.end())
			{
				return false;
			}

			TObjPtr l_sharedPtr( p_element);
			m_objectMap.insert( typename TypeMap::value_type( p_element->GetName(), l_sharedPtr));
			return true;
		}
/*
		TObj * CreateElement( const String & p_elementName)
		{
			return Castor::map::insert( m_objectMap, p_elementName, p_elementName);
		}
		template<typename TParam>
		TObj * CreateElement( const String & p_elementName, const TParam & p_param)
		{
			return Castor::map::insert( m_objectMap, p_elementName, p_elementName, p_param);
		}
*/
		bool HasElement( const String & p_key)const
		{
			return m_objectMap.find( p_key) != m_objectMap.end();
		}
		bool HasElement( TObjPtr p_element)const
		{
			return m_objectMap.find( p_element->GetName()) != m_objectMap.end();
		}
		TObjPtr RemoveElement( const String & p_key)
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
			return RemoveElement( p_element->GetName());
		}
		TObjPtr GetElementByName( const String & p_key)
		{
			return Utils::map::findOrNull( m_objectMap, p_key);
		}
		TObjPtr DestroyElement( TObjPtr & p_element)
		{
			return RemoveElement( p_element);
/*
			if (RemoveElement( p_element))
			{
				delete p_element;
				return true;
			}

			return false;
*/
		}
	};
}
}

#endif

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
#ifndef ___MANAGER_H___
#define ___MANAGER_H___

#include <map>
#include <string>

#include "Module_Utils.h"
#include "STLMapMacros.h"


namespace General
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
		typedef std::map <String, TObj *> TypeMap;
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
			General::Utils::map::deleteAll( m_objectMap);
		}
		bool AddElement( TObj * p_element)
		{
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_element->GetName());

			if (ifind != m_objectMap.end())
			{
				return false;
			}

			m_objectMap.insert( typename TypeMap::value_type( p_element->GetName(), p_element));
			return true;
		}
/*
		TObj * CreateElement( const String & p_elementName)
		{
			return General::Utils::map::insert( m_objectMap, p_elementName, p_elementName);
		}
		template<typename TParam>
		TObj * CreateElement( const String & p_elementName, const TParam & p_param)
		{
			return General::Utils::map::insert( m_objectMap, p_elementName, p_elementName, p_param);
		}
*/
		bool HasElement( const String & p_key)const
		{
			return General::Utils::map::has( m_objectMap, p_key);
		}
		bool HasElement( TObj * p_element)const
		{
			return General::Utils::map::has( m_objectMap, p_element->GetName());
		}
		TObj * RemoveElement( const String & p_key)
		{
			const typename TypeMap::iterator & ifind = m_objectMap.find( p_key);

			if (ifind == m_objectMap.end())
			{
				return NULL;
			}

			TObj * l_ret = ifind->second;
			m_objectMap.erase( ifind);
			return l_ret;
		}
		bool RemoveElement( TObj * p_element)
		{
			return (RemoveElement( p_element->GetName()) != NULL);
		}
		TObj * GetElementByName( const String & p_key)
		{
			return General::Utils::map::findOrNull( m_objectMap, p_key);
		}
		bool DestroyElement( TObj * p_element)
		{
			if (RemoveElement( p_element))
			{
				delete p_element;
				return true;
			}
			return false;
		}
	};
}
}

#endif

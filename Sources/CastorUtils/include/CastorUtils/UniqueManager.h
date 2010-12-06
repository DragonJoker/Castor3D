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
#ifndef ___Castor_UniqueManager___
#define ___Castor_UniqueManager___

#include "Module_Utils.h"
#include "SharedPtr.h"

#include "Manager.h"

namespace Castor
{ namespace Templates
{
	/*!
	A collection class, allowing you to store named objects, removing, finding or adding them as you wish.
	Autodeletion of all the contained items on deletion of the manager.
	*/
	template <typename Key, typename TObj, class T>
	class UniqueManager : public Manager<Key, TObj>
	{
	protected:
		Castor::MultiThreading::RecursiveMutex m_mutex;

	protected:
		UniqueManager(){}
		~UniqueManager()
		{
			Clear();
		}

	private:
		UniqueManager( const UniqueManager &);
		const UniqueManager & operator =( const UniqueManager &);

	public:
		static T & GetSingleton()
		{
			static T m_singleton;
			return m_singleton;
		}

		static T * GetSingletonPtr()
		{
			return & GetSingleton();
		}

		static void Clear() throw()
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			((Manager *)l_pThis)->Clear();
		}
		static bool AddElement( TObjPtr p_element)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			return ((Manager *)l_pThis)->AddElement( p_element);
		}
		static TObjPtr CreateElement( const String & p_elementName)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			return ((Manager *)l_pThis)->CreateElement( p_elementName);
		}
		template<typename TParam>
		TObjPtr CreateElement( const String & p_elementName, const TParam & p_param)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			return ((Manager *)l_pThis)->CreateElement( p_elementName, p_param);
		}
		template<typename TParam1, typename TParam2>
		TObjPtr CreateElement( const String & p_elementName, const TParam1 & p_param1, const TParam2 & p_param2)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			return ((Manager *)l_pThis)->CreateElement( p_elementName, p_param1, p_param2);
		}
		template<typename TParam1, typename TParam2, typename TParam3>
		TObjPtr CreateElement( const String & p_elementName, const TParam1 & p_param1, const TParam2 & p_param2, const TParam3 & p_param3)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			return ((Manager *)l_pThis)->CreateElement( p_elementName, p_param1, p_param2, p_param3);
		}
		template<typename TParam1, typename TParam2, typename TParam3, typename TParam4>
		TObjPtr CreateElement( const String & p_elementName, const TParam1 & p_param1, const TParam2 & p_param2, const TParam3 & p_param3, const TParam4 & p_param4)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			return ((Manager *)l_pThis)->CreateElement( p_elementName, p_param1, p_param2, p_param3, p_param4);
		}
		static bool HasElement( const String & p_key)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			return ((Manager *)l_pThis)->HasElement( p_key);
		}
		static bool HasElement( TObjPtr p_element)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			return ((Manager *)l_pThis)->HasElement( p_element);
		}
		static TObjPtr RemoveElement( const String & p_key)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			return ((Manager *)l_pThis)->RemoveElement( p_key);
		}
		static TObjPtr RemoveElement( TObjPtr p_element)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			return ((Manager *)l_pThis)->RemoveElement( p_element);
		}
		static TObjPtr GetElementByName( const String & p_key)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			return ((Manager *)l_pThis)->GetElementByName( p_key);
		}
		static TObjPtr DestroyElement( TObjPtr & p_element)
		{
			UniqueManager * l_pThis = GetSingletonPtr();
			CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
			return ((Manager *)l_pThis)->DestroyElement( p_element);
		}
	};
}
}

#endif

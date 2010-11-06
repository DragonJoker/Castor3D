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
#ifndef ___Castor_Singleton___
#define ___Castor_Singleton___

#include "NonCopyable.h"

namespace Castor
{ namespace Theory
{
	template <class T>
	class Singleton : d_noncopyable
	{
	protected:
		static T * sm_singleton;

	protected:
		Singleton()
		{
			sm_singleton = reinterpret_cast <T*>( this);
		}

		~Singleton()
		{
		}

	public:
		static inline T * GetSingletonPtr()	{ return sm_singleton; }
		static inline T & GetSingleton()	{ return *sm_singleton; }
		static inline T * Create()
		{
			if (sm_singleton == NULL)
			{
				sm_singleton = new T();
			}

			return sm_singleton;
		}

		static inline void Destroy()
		{
			if (sm_singleton != NULL)
			{
				delete sm_singleton;
			}

			sm_singleton = NULL;
		}
		static inline void SetSingleton( T * p_singleton){ sm_singleton = p_singleton; }
	};
}
}
	
#define CASTOR_INIT_SINGLETON( p_name) template <> p_name * Castor::Theory::Singleton <p_name> ::sm_singleton = NULL
#define CASTOR_SINGLETON_FOR_DLL( p_name) static p_name & GetSingleton( void); static p_name * GetSingletonPtr( void)
#define CASTOR_INIT_SINGLETON_FOR_DLL( p_name) p_name * p_name::sm_singleton = NULL; p_name * p_name :: GetSingletonPtr()	{ return sm_singleton; } p_name & p_name :: GetSingleton()	{ return *sm_singleton; }
#define CASTOR_SET_SINGLETON()  sm_singleton = this

#endif

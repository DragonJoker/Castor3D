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
#ifndef ___AUTO_SINGLETON_H___
#define ___AUTO_SINGLETON_H___

#include "NonCopyable.h"

/**
 * Autosingleton is very simple :
 * When you'll need it, it'll be there.
 * If you never use it, it won't ever be created.
**/

namespace General
{ namespace Theory
{
	template <class T>
	class AutoSingleton : d_noncopyable
	{
	protected:
		AutoSingleton(){}
		~AutoSingleton(){}

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
	};
}
}

#endif

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
#ifndef ___Castor_Named___
#define ___Castor_Named___

#include "CastorString.h"

namespace Castor
{ namespace Theory
{
	template<typename T = String>
	class namedBase
	{
	protected:
		T m_name;

	public:
		namedBase( const T & p_name)
			:	m_name (p_name)
		{
		}
		~namedBase(){}

	public:
		inline const T & GetName()const { return m_name; }
	};

	typedef namedBase<String> named;
}
}

#endif

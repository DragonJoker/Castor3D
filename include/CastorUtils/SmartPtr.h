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
#ifndef ___Castor_shared_ptr___
#define ___Castor_shared_ptr___

#if ! _HAS_TR1 && ! _HAS_0X
#	include <boost/weak_ptr.hpp>
#	include <boost/shared_ptr.hpp>
#endif
#include <boost/scoped_ptr.hpp>

namespace Castor
{	namespace Templates
{
#if _HAS_TR1
	using std::tr1::weak_ptr;
	using std::tr1::shared_ptr;
	using boost::scoped_ptr;

	using std::tr1::static_pointer_cast;
	using std::tr1::dynamic_pointer_cast;
	using std::tr1::const_pointer_cast;
#elif _HAS_0X
	using std::weak_ptr;
	using std::shared_ptr;
	using boost::scoped_ptr;

	using std::static_pointer_cast;
	using std::dynamic_pointer_cast;
	using std::const_pointer_cast;
#else
	using boost::weak_ptr;
	using boost::shared_ptr;
	using boost::scoped_ptr;

	using boost::static_pointer_cast;
	using boost::dynamic_pointer_cast;
	using boost::const_pointer_cast;
#endif
}
}

#endif

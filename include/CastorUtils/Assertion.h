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
#ifndef ___Castor_Assertion___
#define ___Castor_Assertion___

#include "Module_Utils.h"

#ifndef CASTOR_USE_ASSERT
#	if _DEBUG
#		define CASTOR_USE_ASSERT 1
#	else
#		define CASTOR_USE_ASSERT 0
#	endif
#endif

#if CASTOR_USE_ASSERT
#	ifdef CASTOR_ASSERT
#		undef CASTOR_ASSERT
#	endif
#	include <assert.h>
#	define CASTOR_ASSERT( X) assert( X)
#else
#	define CASTOR_ASSERT( X)
#endif

#endif
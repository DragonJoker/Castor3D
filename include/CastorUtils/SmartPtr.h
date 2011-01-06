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
#ifndef ___Castor_SmartPtr___
#define ___Castor_SmartPtr___

#if ! _HAS_TR1
#	include <boost/weak_ptr.hpp>
#	include <boost/shared_ptr.hpp>
#endif
#include <boost/scoped_ptr.hpp>

namespace Castor
{	namespace Templates
{
#if _HAS_TR1
	//! Holds the 3 smart pointer classes : shared, weak and scoped
	template <typename T>
	struct SmartPtr
	{
		typedef std::tr1::weak_ptr<T>	Weak;
		typedef std::tr1::shared_ptr<T> Shared;
		typedef boost::scoped_ptr<T>	Scoped;
	};

	template <typename T, typename U>
	typename SmartPtr<T>::Shared static_pointer_cast( const typename SmartPtr<U>::Shared & p_pShared)
	{
		return std::tr1::static_pointer_cast<T, U>( p_pShared);
	}

	template <typename T, typename U>
	typename SmartPtr<T>::Shared dynamic_pointer_cast( const typename SmartPtr<U>::Shared & p_pShared)
	{
		return std::tr1::dynamic_pointer_cast<T, U>( p_pShared);
	}

	template <typename T, typename U>
	typename SmartPtr<T>::Shared const_pointer_cast( const typename SmartPtr<U>::Shared & p_pShared)
	{
		return std::tr1::const_pointer_cast<T, U>( p_pShared);
	}
#else
	//! Holds the 3 smart pointer classes : shared, weak and scoped
	template <typename T>
	struct SmartPtr
	{
		typedef boost::weak_ptr<T>		Weak;
		typedef boost::shared_ptr<T>	Shared;
		typedef boost::scoped_ptr<T>	Scoped;
	};

	template <typename T, typename U>
	typename SmartPtr<T>::Shared static_pointer_cast( const typename SmartPtr<U>::Shared & p_pShared)
	{
		return boost::static_pointer_cast<T, U>( p_pShared);
	}

	template <typename T, typename U>
	typename SmartPtr<T>::Shared dynamic_pointer_cast( const typename SmartPtr<U>::Shared & p_pShared)
	{
		return boost::dynamic_pointer_cast<T, U>( p_pShared);
	}

	template <typename T, typename U>
	typename SmartPtr<T>::Shared const_pointer_cast( const typename SmartPtr<U>::Shared & p_pShared)
	{
		return boost::const_pointer_cast<T, U>( p_pShared);
	}
#endif
}
}

#endif
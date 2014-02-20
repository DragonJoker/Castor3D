/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "Macros.hpp"

#include <functional>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		08/12/2011
	\~english
	\brief		Dummy destructor
	\remark		Used as a parameter to shared_ptr, to make deallocation dummy (only sets pointer to NULL)
	\~french
	\brief		Destructeur zombie
	\remark		Utilisé en tant que paramètre à shared_ptr, afin d'avoir une désallocation zombie (ne fait que mettre le pointeur à NULL, sans le désallouer)
	*/
	struct dummy_dtor
	{
		template <typename T>
		inline void operator ()( T * p_pPointer)throw()
		{
			p_pPointer = NULL;
		}
	};
	extern dummy_dtor g_dummyDtor;
}

namespace std
{
#if ! CASTOR_HAS_STDSMARTPTR
#	include <boost/make_shared.hpp>
#	if CASTOR_HAS_TR1SMARTPTR
	using std::tr1::weak_ptr;
	using std::tr1::shared_ptr;

	using std::tr1::static_pointer_cast;
	using std::tr1::dynamic_pointer_cast;
	using std::tr1::const_pointer_cast;
#	else
	using boost::weak_ptr;
	using boost::shared_ptr;

	using boost::static_pointer_cast;
	using boost::dynamic_pointer_cast;
	using boost::const_pointer_cast;
#	endif

	using boost::make_shared;
#endif
}

#if ! CASTOR_HAS_NULLPTR
class nullptr_t
{
public:
	template<class T>
	operator T 		*	() const { return NULL; }// convertible to any type of null non-member pointer
	template<class C, class T>
	operator T C:: 	*	() const { return NULL; } // or to any type of null member pointer...
	template<class T>
	operator std::shared_ptr< T >		() const { return std::shared_ptr< T >(); }// convertible to any type of null non-member shared_ptr

private:
	void operator &() const;    // whose address can't be taken
};

const nullptr_t nullptr = {};
template <typename T>
inline bool operator ==( T const * p_pPointer, nullptr_t const & p_pNull )
{
	return p_pPointer == NULL;
}
template <typename T>
inline bool operator !=( T const * p_pPointer, nullptr_t const & p_pNull )
{
	return p_pPointer;
}
template <typename T>
inline bool operator ==( nullptr_t p_pNull, T const * p_pPointer )
{
	return p_pPointer == NULL;
}
template <typename T>
inline bool operator !=( nullptr_t p_pNull, T const * p_pPointer )
{
	return p_pPointer;
}
template <typename T>
inline bool operator ==( T * p_pPointer, nullptr_t const & p_pNull )
{
	return p_pPointer == NULL;
}
template <typename T>
inline bool operator !=( T * p_pPointer, nullptr_t const & p_pNull )
{
	return p_pPointer;
}
template <typename T>
inline bool operator ==( nullptr_t p_pNull, T * p_pPointer)
{
	return p_pPointer == NULL;
}
template <typename T>
inline bool operator !=( nullptr_t p_pNull, T * p_pPointer)
{
	return p_pPointer;
}
template <typename T>
inline bool operator ==( std::shared_ptr<T> const & p_pPointer, nullptr_t const & p_pNull )
{
	return p_pPointer.use_count() == 0;
}
template <typename T>
inline bool operator !=( std::shared_ptr<T> const & p_pPointer, nullptr_t const & p_pNull )
{
	return ! p_pPointer.use_count() > 0;
}
template <typename T>
inline bool operator ==( nullptr_t p_pNull, std::shared_ptr<T> const & p_pPointer )
{
	return p_pPointer.use_count() == 0;
}
template <typename T>
inline bool operator !=( nullptr_t p_pNull, std::shared_ptr<T> const & p_pPointer )
{
	return ! p_pPointer.use_count() > 0;
}
template <typename T>
inline bool operator ==( std::shared_ptr<T> & p_pPointer, nullptr_t const & p_pNull )
{
	return p_pPointer.use_count() == 0;
}
template <typename T>
inline bool operator !=( std::shared_ptr<T> & p_pPointer, nullptr_t const & p_pNull )
{
	return ! p_pPointer.use_count() > 0;
}
template <typename T>
inline bool operator ==( nullptr_t p_pNull, std::shared_ptr<T> & p_pPointer )
{
	return p_pPointer.use_count() == 0;
}
template <typename T>
inline bool operator !=( nullptr_t p_pNull, std::shared_ptr<T> & p_pPointer )
{
	return ! p_pPointer.use_count() > 0;
}
#endif

#endif

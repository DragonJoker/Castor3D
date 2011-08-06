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

#include <boost/scoped_ptr.hpp>

namespace Castor
{	namespace Templates
{
#if CASTOR_HAS_STDSMARTPTR
	using std::weak_ptr;
	using std::shared_ptr;
	using boost::scoped_ptr;

	using std::static_pointer_cast;
	using std::dynamic_pointer_cast;
	using std::const_pointer_cast;
#elif CASTOR_HAS_TR1SMARTPTR
	using std::tr1::weak_ptr;
	using std::tr1::shared_ptr;
	using boost::scoped_ptr;

	using std::tr1::static_pointer_cast;
	using std::tr1::dynamic_pointer_cast;
	using std::tr1::const_pointer_cast;
#else
	using boost::weak_ptr;
	using boost::shared_ptr;
	using boost::scoped_ptr;

	using boost::static_pointer_cast;
	using boost::dynamic_pointer_cast;
	using boost::const_pointer_cast;

#endif

#if CASTOR_HAS_NULLPTR
	using std::nullptr_t;
#else
	class nullptr_t
	{
	public:
		template<class T>
		operator T 		*	() const { return 0; }// convertible to any type of null non-member pointer
		template<class C, class T>
		operator T C:: 	*	() const { return 0; } // or to any type of null member pointer...

	private:
		void operator &() const;    // whose address can't be taken
	};
#endif
}
}

#if ! CASTOR_HAS_NULLPTR
	const Castor::Templates::nullptr_t nullptr = {};
	template <typename T>
	inline bool operator ==( T const * p_pPointer, Castor::Templates::nullptr_t p_pNull)
	{
		return p_pPointer == NULL;
	}
	template <typename T>
	inline bool operator !=( T const * p_pPointer, Castor::Templates::nullptr_t p_pNull)
	{
		return p_pPointer;
	}
	template <typename T>
	inline bool operator ==( Castor::Templates::nullptr_t p_pNull, T const * p_pPointer)
	{
		return p_pPointer == NULL;
	}
	template <typename T>
	inline bool operator !=( Castor::Templates::nullptr_t p_pNull, T const * p_pPointer)
	{
		return p_pPointer;
	}
	template <typename T>
	inline bool operator ==( const Castor::Templates::shared_ptr<T> & p_pPointer, Castor::Templates::nullptr_t p_pNull)
	{
		return p_pPointer.use_count() == 0;
	}
	template <typename T>
	inline bool operator !=( const Castor::Templates::shared_ptr<T> & p_pPointer, Castor::Templates::nullptr_t p_pNull)
	{
		return ! p_pPointer.use_count() > 0;
	}
	template <typename T>
	inline bool operator ==( Castor::Templates::nullptr_t p_pNull, const Castor::Templates::shared_ptr<T> & p_pPointer)
	{
		return p_pPointer.use_count() == 0;
	}
	template <typename T>
	inline bool operator !=( Castor::Templates::nullptr_t p_pNull, const Castor::Templates::shared_ptr<T> & p_pPointer)
	{
		return ! p_pPointer.use_count() > 0;
	}
#endif

#endif

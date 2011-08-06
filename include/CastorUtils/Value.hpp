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
#ifndef ___Castor_Value___
#define ___Castor_Value___

#include <limits>
#include <algorithm>

namespace Castor
{ namespace Templates
{
	//! Functor used to delete an object with for_each
	template <class Object>
	struct ObjectDeleter
	{
		void operator()( Object * p_pPointer) { delete p_pPointer; }
	};
	//! Clears a container of it's objects
	template <class Container, class Object>
	void ClearContent( Container & p_container, Object p_object)
	{
	}
	//! Clears a container it's objects, which are deleted
	template <class Container, class Object>
	void ClearContent( Container & p_container, Object * p_object)
	{
		std::for_each( p_container.begin(), p_container.end(), ObjectDeleter<Object>());
	}
	//! Clears a container using previous functions
	template <class Container>
	void ClearContainer( Container & p_container)
	{
		typedef typename Container::value_type value_type;
		ClearContent( p_container, value_type());
		Container().swap( p_container);
	}
	//! Tells that T and U classes are of different types
	template<class T, class U> struct is_same
	{
		static const bool value = false;
	};
	//! Tells that T and T classes are of same types
	template<class T> struct is_same<T,T>
	{
		static const bool value = true;
	};
	//! Tells either T is a floating point basic type or not
	template<typename T> struct is_floating
	{
		static const bool value = is_same< T, float>::value || is_same< T, double>::value || is_same< T, long double>::value;
	};
	//! Tells that T is an unsigned basic type
	template<typename T> struct is_unsigned
	{
		static const bool value = is_same<unsigned( T), T>::value;
	};
	template<typename Ty> struct base_type
	{
	private:
		template<typename T> struct base_type_helper
		{
			typedef T type;
		};
		template<typename T> struct base_type_helper<T const>
		{
			typedef T type;
		};
		template<typename T> struct base_type_helper<T const &>
		{
			typedef T type;
		};
		template<typename T> struct base_type_helper<T &>
		{
			typedef T type;
		};
		template<typename T> struct base_type_helper<T const *>
		{
			typedef T type;
		};
		template<typename T> struct base_type_helper<T *>
		{
			typedef T type;
		};
	public:
		typedef typename base_type_helper<Ty>::type type;
	};
	//! Struct used to select best way to put type in parameter : 'value' or 'const reference'
	template <typename T>
	struct call_traits
	{
	private:
		template <typename U, bool Big> struct call_traits_helper;
		//! Struct used to select 'const reference' parameter type
		template <typename U>
		struct call_traits_helper<U, true>
		{
			typedef U const & type;
		};
		//! Struct used to select 'value' parameter type
		template <typename U>
		struct call_traits_helper<U, false>
		{
			typedef U type;
		};

	public:
		typedef typename call_traits_helper<T, (sizeof( T) > 8)>::type param_type;
	};

	//! Policy for various types
	/*!
	Policy created to use various types in template classes such as Point, Matrix, and others.
	Holds  operators, initialisation functions, conversion functions, ...
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <typename T> class Policy
	{
	private:
		typedef T value_type;
		typedef typename call_traits<T>::param_type param_type;

	public:
		static value_type	zero			()							{ return 0; }
		static value_type	unit			()							{ return 1; }
		static void			init			( value_type & p_a)			{ p_a = zero(); }
		static bool			is_null			( param_type p_a)			{ return equals<T>( p_a, zero()); }
		static value_type	negate			( param_type p_a)			{ return is_unsigned<value_type>::value ? p_a : -p_a; }
		static value_type	ass_negate		( value_type & p_a)			{ return assign<T>( p_a, negate( p_a)); }
		static value_type	parse			( String const & p_str)		{ value_type l_tReturn;p_str.parse( l_tReturn);return l_tReturn; }
		static value_type	stick			( value_type & p_a)
		{
			if (is_floating<value_type>::value)
			{
				if (is_null( p_a))
				{
					init( p_a);
				}
				else if (p_a > 0)
				{
					if (equals( p_a, float( int( p_a + 0.5f))))
					{
						assign( p_a, float( int( p_a + 0.5f)));
					}
				}
				else
				{
					if (equals( p_a, float( int( p_a - 0.5f))))
					{
						assign( p_a, float( int( p_a - 0.5f)));
					}
				}
			}
		}

		template <typename Ty> static value_type	convert			( Ty const & p_value)				{ return static_cast<value_type>( p_value); }
		template <typename Ty> static bool			equals			( param_type p_a,	Ty const & p_b)	{ return p_a == convert<Ty>( p_b); }
		template <typename Ty> static value_type	add				( param_type p_a,	Ty const & p_b)	{ return p_a + convert<Ty>( p_b); }
		template <typename Ty> static value_type	substract		( param_type p_a,	Ty const & p_b)	{ return p_a - convert<Ty>( p_b); }
		template <typename Ty> static value_type	multiply		( param_type p_a,	Ty const & p_b)	{ return p_a * convert<Ty>( p_b); }
		template <typename Ty> static value_type	divide			( param_type p_a,	Ty const & p_b)	{ return p_a / convert<Ty>( p_b); }
		template <typename Ty> static value_type	ass_add			( value_type & p_a,	Ty const & p_b)	{ return assign( p_a, add<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_substract	( value_type & p_a,	Ty const & p_b)	{ return assign( p_a, substract<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_multiply	( value_type & p_a,	Ty const & p_b)	{ return assign( p_a, multiply<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_divide		( value_type & p_a,	Ty const & p_b)	{ return assign( p_a, divide<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	assign			( value_type & p_a,	Ty const & p_b)	{ return p_a = convert<Ty>( p_b); }
	};
	//! 'bool' Policy
	template <> class Policy<bool>
	{
	private:
		typedef bool value_type;

	public:
		static value_type	zero			()							{ return false; }
		static value_type	unit			()							{ return true; }
		static void			init			( value_type & p_a)			{ p_a = zero(); }
		static bool			is_null			( value_type p_a)			{ return equals<value_type>( p_a, zero()); }
		static value_type	negate			( value_type p_a)			{ return ! p_a; }
		static value_type	ass_negate		( value_type & p_a)			{ return assign<value_type>( p_a, negate( p_a)); }
		static value_type	parse			( String const & p_strVal)	{ return p_strVal == cuT( "true") || p_strVal == cuT( "TRUE") || p_strVal == cuT( "1") || p_strVal.to_int() != 0; }
		static value_type	stick			( value_type & p_a)			{}

		template <typename Ty> static value_type	convert			( Ty const & p_value)				{ return p_value != Policy<Ty>::zero();}
		template <typename Ty> static bool			equals			( value_type p_a,	Ty const & p_b)	{ return p_a == convert<Ty>( p_b); }
		template <typename Ty> static value_type	add				( value_type p_a,	Ty const & p_b)	{ return p_a || convert<Ty>( p_b); }
		template <typename Ty> static value_type	substract		( value_type p_a,	Ty const & p_b)	{ return p_a || convert<Ty>( p_b); }
		template <typename Ty> static value_type	multiply		( value_type p_a,	Ty const & p_b)	{ return p_a && convert<Ty>( p_b); }
		template <typename Ty> static value_type	divide			( value_type p_a,	Ty const & p_b)	{ return p_a && convert<Ty>( p_b); }
		template <typename Ty> static value_type	ass_add			( value_type p_a,	Ty const & p_b)	{ return assign( p_a, add<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_substract	( value_type & p_a,	Ty const & p_b)	{ return assign( p_a, substract<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_multiply	( value_type & p_a,	Ty const & p_b)	{ return assign( p_a, multiply<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	ass_divide		( value_type & p_a,	Ty const & p_b)	{ return assign( p_a, divide<Ty>( p_a, p_b)); }
		template <typename Ty> static value_type	assign			( value_type & p_a,	Ty const & p_b)	{ return p_a = convert<Ty>( p_b); }
	};
}
}

#endif

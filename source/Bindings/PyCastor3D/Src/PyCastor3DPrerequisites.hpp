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
#ifndef ___C3DPY_PREREQUISITES_H___
#define ___C3DPY_PREREQUISITES_H___

#include <boost/python.hpp>

namespace py = boost::python;

namespace cpy
{
	template< typename Value, typename Class, typename Index >
	struct IndexedMemberGetter
	{
		typedef Value const & ( Class::*Function )( Index )const;
		Index m_index;
		Function m_function;
		IndexedMemberGetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		Value const & operator()( Class * p_value )
		{
			return ( p_value->*( this->m_function ) )( m_index );
		}
	};

	template< typename Value, typename Class, typename Index >
	struct IndexedMemberSetter
	{
		typedef Value & ( Class::*Function )( Index );
		Index m_index;
		Function m_function;
		IndexedMemberSetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		void operator()( Class * p_value, Value const & p_arg )
		{
			( p_value->*( this->m_function ) )( m_index ) = p_arg;
		}
	};

	template< typename Value, typename Class, typename Index >
	struct ParameteredMemberRefSetter
	{
		typedef void ( Class::*Function )( Index, Value const & );
		Index m_index;
		Function m_function;
		ParameteredMemberRefSetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		void operator()( Class * p_value, Value const & p_arg )
		{
			( p_value->*( this->m_function ) )( m_index, p_arg );
		}
	};

	template< typename Value, typename Class, typename Index >
	struct ParameteredMemberGetter
	{
		typedef Value( Class::*Function )( Index )const;
		Index m_index;
		Function m_function;
		ParameteredMemberGetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		Value operator()( Class * p_value )
		{
			return ( p_value->*( this->m_function ) )( m_index );
		}
	};

	template< typename Value, typename Class, typename Index >
	struct ParameteredMemberSetter
	{
		typedef void ( Class::*Function )( Index, Value );
		Index m_index;
		Function m_function;
		ParameteredMemberSetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		void operator()( Class * p_value, Value p_arg )
		{
			( p_value->*( this->m_function ) )( m_index, p_arg );
		}
	};

	template< typename Value, class Class >
	struct MemberValueGetter
	{
		typedef Value( Class::*Function )()const;
		Function m_function;
		MemberValueGetter( Function p_function )
			: m_function( p_function )
		{
		}
		Value operator()( Class * p_instance )
		{
			return ( p_instance->*( this->m_function ) )();
		}
	};

	template< typename Value, class Class >
	struct MemberValueSetter
	{
		typedef void ( Class::*Function )( Value );
		Function m_function;
		MemberValueSetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value p_value )
		{
			( p_instance->*( this->m_function ) )( p_value );
		}
	};

	template< typename Value, class Class >
	struct MemberRefValueGetter
	{
		typedef Value const & ( Class::*Function )()const;
		Function m_function;
		MemberRefValueGetter( Function p_function )
			: m_function( p_function )
		{
		}
		Value const & operator()( Class * p_instance )
		{
			return ( p_instance->*( this->m_function ) )();
		}
	};

	template< typename Value, class Class >
	struct MemberRefValueSetter
	{
		typedef void ( Class::*Function )( Value const & );
		Function m_function;
		MemberRefValueSetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value const & p_value )
		{
			( p_instance->*( this->m_function ) )( p_value );
		}
	};

	template< typename Value, class Class >
	struct MemberRetValueSetter
	{
		typedef Value & ( Class::*Function )();
		Function m_function;
		MemberRetValueSetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value p_value )
		{
			( p_instance->*( this->m_function ) )() = p_value;
		}
	};
}

namespace boost
{
	namespace python
	{
		namespace detail
		{
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< Value const &, Class * >
			get_signature( cpy::IndexedMemberGetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< Value const &, Class * >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::IndexedMemberSetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::ParameteredMemberRefSetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< Value, Class * >
			get_signature( cpy::ParameteredMemberGetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< Value, Class * >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::ParameteredMemberSetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< Value const &, Class * >
			get_signature( cpy::MemberRefValueGetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< Value const &, Class * >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::MemberRefValueSetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< Value, Class * >
			get_signature( cpy::MemberValueGetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< Value, Class * >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::MemberValueSetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::MemberRetValueSetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
		}
	}
}

namespace cpy
{
	template< typename Value, class Class, typename Index, typename _Index >
	inline py::object
	make_getter( Value const & ( Class::*p_function )( Index )const, _Index p_index )
	{
		return py::make_function( IndexedMemberGetter< Value, Class, Index >( p_function, Index( p_index ) ), py::return_value_policy< py::copy_const_reference >() );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMemberGetter< Value, Class, Index >
	make_getter( Value( Class::*p_function )( Index )const, _Index p_index )
	{
		return ParameteredMemberGetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename CallPolicies >
	inline py::object
	make_getter( Value const & ( Class::*p_function )()const, CallPolicies policies )
	{
		return py::make_function( MemberRefValueGetter< Value, Class >( p_function ), policies );
	}
	template< typename Value, class Class >
	inline MemberValueGetter< Value, Class >
	make_getter( Value( Class::*p_function )()const )
	{
		return MemberValueGetter< Value, Class >( p_function );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline IndexedMemberSetter< Value, Class, Index >
	make_setter( Value & ( Class::*p_function )( Index ), _Index p_index )
	{
		return IndexedMemberSetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMemberSetter< Value, Class, Index >
	make_setter( void ( Class::*p_function )( Index, Value ), _Index p_index )
	{
		return ParameteredMemberSetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMemberRefSetter< Value, Class, Index >
	make_setter( void ( Class::*p_function )( Index, Value const & ), _Index p_index )
	{
		return ParameteredMemberRefSetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class >
	inline MemberRefValueSetter< Value, Class >
	make_setter( void ( Class::*p_function )( Value const & ) )
	{
		return MemberRefValueSetter< Value, Class >( p_function );
	}
	template< typename Value, class Class >
	inline MemberValueSetter< Value, Class >
	make_setter( void ( Class::*p_function )( Value ) )
	{
		return MemberValueSetter< Value, Class >( p_function );
	}
	template< typename Value, class Class >
	inline MemberRetValueSetter< Value, Class >
	make_setter( Value & ( Class::*p_function )() )
	{
		return MemberRetValueSetter< Value, Class >( p_function );
	}
}

#endif

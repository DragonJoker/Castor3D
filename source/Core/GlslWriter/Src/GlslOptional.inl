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

namespace GLSL
{
	template< typename T >
	void WriteAssign( GlslWriter * p_writer, Type & p_lhs, T const & p_rhs );

	template< typename T1, typename T2 >
	void WriteAssign( GlslWriter * p_writer, Optional< T1 > const & p_lhs, T2 const & p_rhs );

	template< typename TypeT >
	Optional< TypeT >::Optional( GlslWriter * p_writer, Castor::String const & p_name, bool p_enabled )
		: TypeT( p_writer, p_name )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	Optional< TypeT >::Optional( TypeT const & p_other, bool p_enabled )
		: TypeT( p_other )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	template< typename T >
	Optional< TypeT >::Optional( Castor::String const & p_name, T const & p_rhs, bool p_enabled )
		: TypeT( p_rhs.m_writer, p_name )
		, m_enabled( p_enabled )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}
	}

	template< typename TypeT >
	Optional< TypeT > Optional< TypeT >::operator=( Optional< TypeT > const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< TypeT > Optional< TypeT >::operator=( T const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	bool Optional< TypeT >::IsEnabled()const
	{
		return m_enabled;
	}

	template< typename TypeT >
	Optional< TypeT >::operator Optional< Type >()const
	{
		return Optional< Type >( *this, IsEnabled() );
	}

	template< typename TypeT >
	Castor::String ParamToString( Castor::String & p_sep, Optional< TypeT > const & p_value )
	{
		Castor::String l_return;

		if ( p_value.IsEnabled() )
		{
			l_return = ParamToString( p_sep, static_cast< Type const & >( p_value ) );
		}

		return l_return;
	}
}

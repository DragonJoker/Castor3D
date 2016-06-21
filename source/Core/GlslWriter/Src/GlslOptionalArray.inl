/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
	template< typename T1, typename T2 >
	void WriteAssign( GlslWriter * p_writer, Optional< Array< T1 > > const & p_lhs, T2 const & p_rhs );

	template< typename TypeT >
	Optional< Array< TypeT > >::Optional( GlslWriter * p_writer, Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
		: Array< TypeT >( p_writer, p_name, p_dimension )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	Optional< Array< TypeT > >::Optional( Array< TypeT > const & p_other, bool p_enabled )
		: Array< TypeT >( p_other, m_dimension )
		, m_enabled( p_enabled )
	{
	}

	template< typename TypeT >
	template< typename T >
	Optional< Array< TypeT > >::Optional( Castor::String const & p_name, uint32_t p_dimension, T const & p_rhs, bool p_enabled )
		: Array< TypeT >( p_rhs.m_writer, p_name, p_dimension )
		, m_enabled( p_enabled )
	{
		if ( m_enabled )
		{
			WriteAssign( TypeT::m_writer, *this, p_rhs );
		}
	}

	template< typename TypeT >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( Optional< Array< TypeT > > const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( Array< TypeT >::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	template< typename T >
	Optional< Array< TypeT > > Optional< Array< TypeT > >::operator=( T const & p_rhs )
	{
		if ( m_enabled )
		{
			WriteAssign( Array< TypeT >::m_writer, *this, p_rhs );
		}

		return *this;
	}

	template< typename TypeT >
	bool Optional< Array< TypeT > >::IsEnabled()const
	{
		return m_enabled;
	}

	template< typename TypeT >
	Optional< Array< TypeT > >::operator Optional< Array< TypeT > >()const
	{
		return Optional< Array< TypeT > >( *this, IsEnabled() );
	}

	template< typename TypeT >
	Castor::String ParamToString( Castor::String & p_sep, Optional< Array< TypeT > > const & p_value )
	{
		Castor::String l_return;

		if ( p_value.IsEnabled() )
		{
			l_return = ParamToString( p_sep, static_cast< Type const & >( p_value ) );
		}

		return l_return;
	}
}

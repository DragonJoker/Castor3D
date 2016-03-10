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
	//*****************************************************************************************

	template< typename TypeT >
	InParam< TypeT >::InParam( GlslWriter * p_writer, Castor::String const & p_name )
		: TypeT( p_writer, p_name )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	InParam< TypeT >::InParam( TypeT const & p_other )
		: TypeT( p_other.m_writer, ToString( p_other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	//*****************************************************************************************

	template< typename TypeT >
	OutParam< TypeT >::OutParam( GlslWriter * p_writer, Castor::String const & p_name )
		: TypeT( p_writer, p_name )
	{
		TypeT::m_type = cuT( "out " ) + TypeT::m_type;
	}

	template< typename TypeT >
	OutParam< TypeT >::OutParam( TypeT const & p_other )
		: TypeT( p_other.m_writer, ToString( p_other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	template< typename T >
	OutParam< TypeT > OutParam< TypeT >::operator=( T const & p_rhs )
	{
		TypeT::m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	//*****************************************************************************************

	template< typename TypeT >
	InOutParam< TypeT >::InOutParam( GlslWriter * p_writer, Castor::String const & p_name )
		: TypeT( p_writer, p_name )
	{
		TypeT::m_type = cuT( "inout " ) + TypeT::m_type;
	}

	template< typename TypeT >
	InOutParam< TypeT >::InOutParam( TypeT const & p_other )
		: TypeT( p_other.m_writer, ToString( p_other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	template< typename T >
	InOutParam< TypeT > InOutParam< TypeT >::operator=( T const & p_rhs )
	{
		TypeT::m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	//*****************************************************************************************
}

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
	//*****************************************************************************************

	Mat3::Mat3()
		: Type( cuT( "mat3 " ) )
	{
	}

	Mat3::Mat3( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "mat3 " ), p_writer, p_name )
	{
	}

	Mat3 & Mat3::operator=( Mat3 const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	Mat3 & Mat3::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Mat3 & Mat3::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	Mat4::Mat4()
		: Type( cuT( "mat4 " ) )
	{
	}

	Mat4::Mat4( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "mat4 " ), p_writer, p_name )
	{
	}

	Mat4 & Mat4::operator=( Mat4 const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	Mat4 & Mat4::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Mat4 & Mat4::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************
}

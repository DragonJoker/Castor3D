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

	Vec2::Vec2()
		: Type( cuT( "vec2 " ) )
	{
	}

	Vec2::Vec2( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec2 " ), p_writer, p_name )
	{
	}

	Vec2::~Vec2()
	{
	}

	Vec2 & Vec2::operator=( Vec2 const & p_rhs )
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
	Vec2 & Vec2::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Vec2 & Vec2::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	Vec3::Vec3()
		: Type( cuT( "vec3 " ) )
	{
	}

	Vec3::Vec3( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec3 " ), p_writer, p_name )
	{
	}

	Vec3::~Vec3()
	{
	}

	Vec3 & Vec3::operator=( Vec3 const & p_rhs )
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
	Vec3 & Vec3::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Vec3 & Vec3::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************

	Vec4::Vec4()
		: Type( cuT( "vec4 " ) )
	{
	}

	Vec4::Vec4( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "vec4 " ), p_writer, p_name )
	{
	}

	Vec4::~Vec4()
	{
	}

	Vec4 & Vec4::operator=( Vec4 const & p_rhs )
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
	Vec4 & Vec4::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Vec4 & Vec4::operator[]( T const & p_rhs )
	{
		m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );
		return *this;
	}

	//*****************************************************************************************
}

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

	Int::Int( )
		: Type( cuT( "int " ) )
	{
	}

	Int::Int( Int && p_value )
		: Type( std::move( p_value ) )
	{
	}

	Int::Int( Int const & p_value )
		: Type( cuT( "int " ), p_value.m_writer )
	{
		m_value << Castor::String( p_value );
	}

	Int::Int( Type const & p_value )
		: Type( cuT( "int " ), p_value.m_writer )
	{
		m_value << Castor::String( p_value );
	}

	Int::Int( int p_value )
		: Type( cuT( "int " ) )
	{
		m_value << p_value;
	}

	Int::Int( float p_value )
		: Type( cuT( "int " ) )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, int p_value )
		: Type( cuT( "int " ), p_writer, Castor::String() )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, float p_value )
		: Type( cuT( "int " ), p_writer, Castor::String() )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "int " ), p_writer, p_name )
	{
	}

	Int & Int::operator=( Int const & p_rhs )
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
	Int & Int::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Int & Int::operator=( int p_rhs )
	{
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Int::operator uint32_t()
	{
		return 0u;
	}

	Int & Int::operator++()
	{
		m_value << cuT( "++" ) << Castor::String( *this );
		return *this;
	}

	Int Int::operator++( int )
	{
		Int l_return;
		l_return.m_value << Castor::String( *this ) << cuT( "++" );
		return l_return;
	}

	//*****************************************************************************************

	Float::Float()
		: Type( cuT( "float " ) )
	{
	}

	Float::Float( Float && p_value )
		: Type( std::move( p_value ) )
	{
	}

	Float::Float( Float const & p_value )
		: Type( cuT( "float " ) )
	{
		m_value << Castor::String( p_value );
	}

	Float::Float( Type const & p_value )
		: Type( cuT( "float " ) )
	{
		m_value << Castor::String( p_value );
	}

	Float::Float( int p_value )
		: Type( cuT( "float " ) )
	{
		m_value << p_value << cuT( ".0f" );
	}

	Float::Float( float p_value )
		: Type( cuT( "float " ) )
	{
		m_value << p_value;

		if ( p_value - int( p_value ) <= std::numeric_limits< float >::epsilon() )
		{
			m_value << cuT( ".0" );
		}

		m_value << cuT( "f" );
	}

	Float::Float( double p_value )
		: Type( cuT( "float " ) )
	{
		m_value << p_value;

		if ( p_value - int( p_value ) <= std::numeric_limits< double >::epsilon() )
		{
			m_value << cuT( ".0" );
		}

		m_value << cuT( "f" );
	}

	Float::Float( GlslWriter * p_writer, int p_value )
		: Type( cuT( "float " ), p_writer, Castor::String() )
	{
		m_value << p_value << cuT( ".0f" );
	}

	Float::Float( GlslWriter * p_writer, float p_value )
		: Type( cuT( "float " ), p_writer, Castor::String() )
	{
		m_value << p_value;

		if ( p_value - int( p_value ) <= std::numeric_limits< float >::epsilon() )
		{
			m_value << cuT( ".0" );
		}

		m_value << cuT( "f" );
	}

	Float::Float( GlslWriter * p_writer, double p_value )
		: Type( cuT( "float " ), p_writer, Castor::String() )
	{
		m_value << p_value;

		if ( p_value - int( p_value ) <= std::numeric_limits< double >::epsilon() )
		{
			m_value << cuT( ".0" );
		}

		m_value << cuT( "f" );
	}

	Float::Float( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "float " ), p_writer, p_name )
	{
	}

	Float & Float::operator=( Float const & p_rhs )
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
	Float & Float::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Float & Float::operator=( float p_rhs )
	{
		*m_writer << Castor::String( *this ) << cuT( " = " ) << Castor::string::to_string( p_rhs ) << cuT( ";" ) << Endl();
		return *this;
	}

	Float::operator float()
	{
		return 0.0f;
	}

	//*****************************************************************************************
}

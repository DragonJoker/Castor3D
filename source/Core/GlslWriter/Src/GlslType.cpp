#include "GlslType.hpp"

#include "GlslWriter.hpp"

using namespace castor;

namespace glsl
{
	//*****************************************************************************************

	Type::Type( String const & p_type )
		: Expr()
		, m_type( p_type )
	{
	}

	Type::Type( String const & p_type, GlslWriter * writer, String const & p_name )
		: Expr( writer )
		, m_name( p_name )
		, m_type( p_type )
	{
	}

	Type::Type( Type const & p_rhs )
		: Expr( p_rhs )
		, m_name( p_rhs.m_name )
		, m_type( p_rhs.m_type )
	{
	}

	Type::Type( Type && p_value )
		: Expr( std::move( p_value ) )
		, m_name( std::move( p_value.m_name ) )
		, m_type( std::move( p_value.m_type ) )
	{
	}

	Type::~Type()
	{
	}

	Type & Type::operator=( Type const & p_rhs )
	{
		Expr::operator=( p_rhs );

		if ( m_name.empty() )
		{
			m_name = p_rhs.m_name;
		}

		return *this;
	}

	Type::operator String()const
	{
		String result = m_value.str();

		if ( result.empty() )
		{
			if ( m_name.empty() )
			{
				result = m_type;
			}
			else
			{
				result = m_name;
			}
		}
		else
		{
			m_value.str( String() );
		}

		return result;
	}

	Type & Type::operator+=( Type const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " += " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator-=( Type const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator*=( Type const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator/=( Type const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator+=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " += " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator-=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator*=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator/=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << String( p_type ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator+=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " += " ) << string::toString( p_type, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator-=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << string::toString( p_type, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator*=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << string::toString( p_type, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator/=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << string::toString( p_type, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator+=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " += " ) << string::toString( p_type, 10, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator-=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << string::toString( p_type, 10, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator*=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << string::toString( p_type, 10, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	Type & Type::operator/=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << string::toString( p_type, 10, Expr::getLocale() ) << cuT( ";" ) << glsl::endl;
		return *this;
	}

	//*****************************************************************************************

	String toString( Type const & p_value )
	{
		return String( p_value );
	}

	String paramToString( castor::String & p_sep, Type const & p_value )
	{
		StringStream stream;
		stream.imbue( Expr::getLocale() );
		stream << p_sep << p_value.m_type << p_value.m_name;
		p_sep = cuT( ",\n    " );
		return String( stream.str() );
	}

	//*****************************************************************************************
}

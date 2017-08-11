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

	Type::Type( String const & p_type, GlslWriter * p_writer, String const & p_name )
		: Expr( p_writer )
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
		*m_writer << castor::String{ *this } << cuT( " += " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator-=( Type const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator*=( Type const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator/=( Type const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator+=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " += " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator-=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator*=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator/=( Optional< Type > const & p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << String( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator+=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " += " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator-=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator*=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator/=( float p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator+=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " += " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator-=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " -= " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator*=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " *= " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
		return *this;
	}

	Type & Type::operator/=( int p_type )
	{
		*m_writer << castor::String{ *this } << cuT( " /= " ) << string::toString( p_type ) << cuT( ";" ) << Endl();
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
		stream << p_sep << p_value.m_type << p_value.m_name;
		p_sep = cuT( ",\n\t" );
		return String( stream.str() );
	}

	//*****************************************************************************************
}

#include "GlslType.hpp"

#include "GlslWriter.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace GLSL
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
			String l_return = m_value.str();

			if ( l_return.empty() )
			{
				if ( m_name.empty() )
				{
					l_return = m_type;
				}
				else
				{
					l_return = m_name;
				}
			}
			else
			{
				m_value.str( String() );
			}

			return l_return;
		}

		Type & Type::operator+=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " += " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator-=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " -= " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator*=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " *= " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator/=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " /= " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator+=( Optional< Type > const & p_type )
		{
			*m_writer << m_name << cuT( " += " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator-=( Optional< Type > const & p_type )
		{
			*m_writer << m_name << cuT( " -= " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator*=( Optional< Type > const & p_type )
		{
			*m_writer << m_name << cuT( " *= " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator/=( Optional< Type > const & p_type )
		{
			*m_writer << m_name << cuT( " /= " ) << String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator+=( float p_type )
		{
			*m_writer << m_name << cuT( " += " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator-=( float p_type )
		{
			*m_writer << m_name << cuT( " -= " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator*=( float p_type )
		{
			*m_writer << m_name << cuT( " *= " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator/=( float p_type )
		{
			*m_writer << m_name << cuT( " /= " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator+=( int p_type )
		{
			*m_writer << m_name << cuT( " += " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator-=( int p_type )
		{
			*m_writer << m_name << cuT( " -= " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator*=( int p_type )
		{
			*m_writer << m_name << cuT( " *= " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator/=( int p_type )
		{
			*m_writer << m_name << cuT( " /= " ) << string::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		//*****************************************************************************************

		Type operator+( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " + " ) << String( p_b );
			return l_return;
		}

		Type operator-( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " - " ) << String( p_b );
			return l_return;
		}

		Type operator*( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
			return l_return;
		}

		Type operator/( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " / " ) << String( p_b );
			return l_return;
		}

		Optional< Type > operator-( Optional< Type > const & p_value )
		{
			Type l_return( p_value.m_type, p_value.m_writer );
			l_return.m_value << cuT( "-( " ) + String( p_value ) << cuT( " )" );
			return Optional< Type >( l_return, p_value.IsEnabled() );
		}

		Optional< Type > operator+( Optional< Type > const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " + " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() );
		}

		Optional< Type > operator-( Optional< Type > const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " - " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() );
		}

		Optional< Type > operator*( Optional< Type > const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() );
		}

		Optional< Type > operator/( Optional< Type > const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " / " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() );
		}

		Optional< Type > operator+( Type const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " + " ) << String( p_b );
			return Optional< Type >( l_return, p_b.IsEnabled() );
		}

		Optional< Type > operator-( Type const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " - " ) << String( p_b );
			return Optional< Type >( l_return, p_b.IsEnabled() );
		}

		Optional< Type > operator*( Type const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
			return Optional< Type >( l_return, p_b.IsEnabled() );
		}

		Optional< Type > operator/( Type const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " / " ) << String( p_b );
			return Optional< Type >( l_return, p_b.IsEnabled() );
		}

		Optional< Type > operator+( Optional< Type > const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " + " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() && p_b.IsEnabled() );
		}

		Optional< Type > operator-( Optional< Type > const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " - " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() && p_b.IsEnabled() );
		}

		Optional< Type > operator*( Optional< Type > const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " * " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() && p_b.IsEnabled() );
		}

		Optional< Type > operator/( Optional< Type > const & p_a, Optional< Type > const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " / " ) << String( p_b );
			return Optional< Type >( l_return, p_a.IsEnabled() && p_b.IsEnabled() );
		}

		Type operator+( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " + " ) << string::to_string( p_b );
			return l_return;
		}

		Type operator-( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " - " ) << string::to_string( p_b );
			return l_return;
		}

		Type operator*( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " * " ) << string::to_string( p_b );
			return l_return;
		}

		Type operator/( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " / " ) << string::to_string( p_b );
			return l_return;
		}

		Type operator+( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " + " ) << string::to_string( p_b );
			return l_return;
		}

		Type operator-( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " - " ) << string::to_string( p_b );
			return l_return;
		}

		Type operator*( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " * " ) << string::to_string( p_b );
			return l_return;
		}

		Type operator/( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << String( p_a ) << cuT( " / " ) << string::to_string( p_b );
			return l_return;
		}

		String ToString( Type const & p_value )
		{
			return String( p_value );
		}

		String ParamToString( Castor::String & p_sep, Type const & p_value )
		{
			StringStream l_stream;
			l_stream << p_sep << p_value.m_type << p_value.m_name;
			p_sep = cuT( ",\n\t" );
			return String( l_stream.str() );
		}

		//*****************************************************************************************
	}
}

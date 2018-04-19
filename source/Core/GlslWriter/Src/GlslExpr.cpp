#include "GlslExpr.hpp"

#include "GlslBaseTypes.hpp"

using namespace castor;

namespace glsl
{
	Expr::Expr()
		: m_writer( nullptr )
	{
		m_value.imbue( getLocale() );
	}

	Expr::Expr( int p_value )
		: m_writer( nullptr )
	{
		m_value.imbue( getLocale() );
		m_value << p_value;
	}

	Expr::Expr( float p_value )
		: m_writer( nullptr )
	{
		m_value.imbue( getLocale() );
		m_value << p_value;
	}

	Expr::Expr( double p_value )
		: m_writer( nullptr )
	{
		m_value.imbue( getLocale() );
		m_value << p_value;
	}

	Expr::Expr( GlslWriter * writer )
		: m_writer( writer )
	{
		m_value.imbue( getLocale() );
	}

	Expr::Expr( GlslWriter * writer, String const & p_init )
		: m_writer( writer )
	{
		m_value.imbue( getLocale() );
		m_value << p_init;
	}

	Expr::Expr( Expr const & p_rhs )
		: m_writer( p_rhs.m_writer )
	{
		m_value.imbue( getLocale() );
		m_value << p_rhs.m_value.rdbuf();
	}

	Expr::Expr( Expr && p_rhs )
		: m_writer( std::move( p_rhs.m_writer ) )
	{
		m_value.imbue( getLocale() );
		m_value << p_rhs.m_value.rdbuf();
	}

	Expr::~Expr()
	{
	}

	Expr & Expr::operator=( Expr const & p_rhs )
	{
		if ( !m_writer )
		{
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	void Expr::updateWriter( Expr const & p_expr )
	{
		if ( !m_writer )
		{
			m_writer = p_expr.m_writer;
		}
	}

	std::locale const & Expr::getLocale()
	{
		static std::locale const result = std::locale{ "C" };
		return result;
	}

	String toString( Expr const & p_value )
	{
		return String( p_value.m_value.str() );
	}

	String toString( int const & p_value )
	{
		return String( Int( p_value ) );
	}

	String toString( uint32_t const & p_value )
	{
		return String( Int( int( p_value ) ) );
	}

	String toString( double const & p_value )
	{
		return String( Float( p_value ) );
	}

	String toString( float const & p_value )
	{
		return String( Float( p_value ) );
	}
}

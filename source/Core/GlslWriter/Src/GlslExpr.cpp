#include "GlslExpr.hpp"

#include "GlslBaseTypes.hpp"

using namespace Castor;

namespace GLSL
{
	Expr::Expr()
		: m_writer( nullptr )
	{
	}

	Expr::Expr( int p_value )
		: m_writer( nullptr )
	{
		m_value << p_value;
	}

	Expr::Expr( float p_value )
		: m_writer( nullptr )
	{
		m_value << p_value;
	}

	Expr::Expr( double p_value )
		: m_writer( nullptr )
	{
		m_value << p_value;
	}

	Expr::Expr( GlslWriter * p_writer )
		: m_writer( p_writer )
	{
	}

	Expr::Expr( GlslWriter * p_writer, String const & p_init )
		: m_writer( p_writer )
	{
		m_value << p_init;
	}

	Expr::Expr( Expr const & p_rhs )
		: m_writer( p_rhs.m_writer )
	{
		m_value << p_rhs.m_value.rdbuf();
	}

	Expr::Expr( Expr && p_rhs )
		: m_writer( std::move( p_rhs.m_writer ) )
	{
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

	void Expr::UpdateWriter( Expr const & p_expr )
	{
		if ( !m_writer )
		{
			m_writer = p_expr.m_writer;
		}
	}

	String ToString( Expr const & p_value )
	{
		return String( p_value.m_value.str() );
	}

	String ToString( int const & p_value )
	{
		return String( Int( p_value ) );
	}

	String ToString( double const & p_value )
	{
		return String( Float( p_value ) );
	}

	String ToString( float const & p_value )
	{
		return String( Float( p_value ) );
	}
}

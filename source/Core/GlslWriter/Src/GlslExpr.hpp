/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_EXPR_H___
#define ___GLSL_EXPR_H___

#include "GlslKeywords.hpp"

namespace glsl
{
	struct Expr
	{
		GlslWriter_API Expr();
		GlslWriter_API Expr( int p_value );
		GlslWriter_API Expr( float p_value );
		GlslWriter_API Expr( double p_value );
		GlslWriter_API Expr( GlslWriter * p_writer );
		GlslWriter_API Expr( GlslWriter * p_writer, castor::String const & p_init );
		GlslWriter_API Expr( Expr const & p_rhs );
		GlslWriter_API Expr( Expr && p_rhs );
		GlslWriter_API virtual ~Expr();
		GlslWriter_API Expr & operator=( Expr const & p_rhs );
		GlslWriter_API void updateWriter( Expr const & p_expr );

		GlslWriter * m_writer;
		mutable castor::StringStream m_value;
	};

	GlslWriter_API castor::String toString( Expr const & p_value );
	GlslWriter_API castor::String toString( int const & p_value );
	GlslWriter_API castor::String toString( uint32_t const & p_value );
	GlslWriter_API castor::String toString( double const & p_value );
	GlslWriter_API castor::String toString( float const & p_value );
}

#endif

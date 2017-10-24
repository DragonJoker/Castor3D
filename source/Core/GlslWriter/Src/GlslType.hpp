/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TYPE_H___
#define ___GLSL_TYPE_H___

#include "GlslExpr.hpp"

namespace glsl
{
	template< typename T > struct is_type : public std::false_type {};

	struct Type
		: public Expr
	{
		GlslWriter_API Type( castor::String const & p_type );
		GlslWriter_API Type( castor::String const & p_type, GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		GlslWriter_API Type( Type && p_rhs );
		GlslWriter_API Type( Type const & p_rhs );
		GlslWriter_API virtual ~Type();
		GlslWriter_API Type & operator=( Type const & p_rhs );
		GlslWriter_API operator castor::String()const;
		GlslWriter_API Type & operator+=( Type const & p_type );
		GlslWriter_API Type & operator-=( Type const & p_type );
		GlslWriter_API Type & operator*=( Type const & p_type );
		GlslWriter_API Type & operator/=( Type const & p_type );
		GlslWriter_API Type & operator+=( Optional< Type > const & p_type );
		GlslWriter_API Type & operator-=( Optional< Type > const & p_type );
		GlslWriter_API Type & operator*=( Optional< Type > const & p_type );
		GlslWriter_API Type & operator/=( Optional< Type > const & p_type );
		GlslWriter_API Type & operator+=( float p_type );
		GlslWriter_API Type & operator-=( float p_type );
		GlslWriter_API Type & operator*=( float p_type );
		GlslWriter_API Type & operator/=( float p_type );
		GlslWriter_API Type & operator+=( int p_type );
		GlslWriter_API Type & operator-=( int p_type );
		GlslWriter_API Type & operator*=( int p_type );
		GlslWriter_API Type & operator/=( int p_type );

		castor::String m_type;
		castor::String m_name;
	};

	template< typename T >
	T operator-( T const & p_value )
	{
		T result( p_value.m_writer );
		result.m_value << cuT( "-( " ) + castor::String( p_value ) << cuT( " )" );
		return result;
	}

	GlslWriter_API castor::String toString( Type const & p_value );
	GlslWriter_API castor::String paramToString( castor::String & p_sep, Type const & p_value );
}

#endif

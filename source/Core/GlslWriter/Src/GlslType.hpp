/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GLSL_TYPE_H___
#define ___GLSL_TYPE_H___

#include "GlslExpr.hpp"

namespace GLSL
{
	template< typename T > struct is_type : public std::false_type {};

	struct Type
		: public Expr
	{
		GlslWriter_API Type( Castor::String const & p_type );
		GlslWriter_API Type( Castor::String const & p_type, GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		GlslWriter_API Type( Type && p_rhs );
		GlslWriter_API Type( Type const & p_rhs );
		GlslWriter_API virtual ~Type();
		GlslWriter_API Type & operator=( Type const & p_rhs );
		GlslWriter_API operator Castor::String()const;
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

		Castor::String m_type;
		Castor::String m_name;
	};

	template< typename T >
	T operator-( T const & p_value )
	{
		T l_return( p_value.m_writer );
		l_return.m_value << cuT( "-( " ) + Castor::String( p_value ) << cuT( " )" );
		return l_return;
	}

	GlslWriter_API Castor::String ToString( Type const & p_value );
	GlslWriter_API Castor::String ParamToString( Castor::String & p_sep, Type const & p_value );
}

#endif

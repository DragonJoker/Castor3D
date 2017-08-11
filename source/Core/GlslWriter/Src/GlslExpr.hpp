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

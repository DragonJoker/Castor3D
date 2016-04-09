/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GLSL_EXPR_H___
#define ___GLSL_EXPR_H___

#include "GlslVariables.hpp"

namespace GLSL
{
	struct Expr
	{
		GlslWriter_API Expr();
		GlslWriter_API Expr( int p_value );
		GlslWriter_API Expr( float p_value );
		GlslWriter_API Expr( double p_value );
		GlslWriter_API Expr( GlslWriter * p_writer );
		GlslWriter_API Expr( GlslWriter * p_writer, Castor::String const & p_init );
		GlslWriter_API Expr( Expr const & p_rhs );
		GlslWriter_API Expr( Expr && p_rhs );
		GlslWriter_API virtual ~Expr();
		GlslWriter_API Expr & operator=( Expr const & p_rhs );
		GlslWriter_API void UpdateWriter( Expr const & p_expr );

		GlslWriter * m_writer;
		mutable Castor::StringStream m_value;
	};

	GlslWriter_API Castor::String ToString( Expr const & p_value );
	GlslWriter_API Castor::String ToString( int const & p_value );
	GlslWriter_API Castor::String ToString( double const & p_value );
	GlslWriter_API Castor::String ToString( float const & p_value );
}

#endif

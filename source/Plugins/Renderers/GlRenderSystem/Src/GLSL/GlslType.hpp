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
#ifndef ___GLSL_TYPE_H___
#define ___GLSL_TYPE_H___

#include "GlslExpr.hpp"

namespace GlRender
{
	namespace GLSL
	{
		template< typename T > struct is_type : public std::false_type {};

		struct Type
			: public Expr
		{
			C3D_Gl_API Type( Castor::String const & p_type );
			C3D_Gl_API Type( Castor::String const & p_type, GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
			C3D_Gl_API Type( Type const & p_rhs );
			C3D_Gl_API virtual ~Type();
			C3D_Gl_API Type & operator=( Type const & p_rhs );
			C3D_Gl_API operator Castor::String()const;
			C3D_Gl_API Type & operator+=( Type const & p_type );
			C3D_Gl_API Type & operator-=( Type const & p_type );
			C3D_Gl_API Type & operator*=( Type const & p_type );
			C3D_Gl_API Type & operator/=( Type const & p_type );
			C3D_Gl_API Type & operator+=( float p_type );
			C3D_Gl_API Type & operator-=( float p_type );
			C3D_Gl_API Type & operator*=( float p_type );
			C3D_Gl_API Type & operator/=( float p_type );
			C3D_Gl_API Type & operator+=( int p_type );
			C3D_Gl_API Type & operator-=( int p_type );
			C3D_Gl_API Type & operator*=( int p_type );
			C3D_Gl_API Type & operator/=( int p_type );

			Castor::String m_type;
			Castor::String m_name;
		};

		C3D_Gl_API Type operator+( Type const & p_a, Type const & p_b );
		C3D_Gl_API Type operator-( Type const & p_a, Type const & p_b );
		C3D_Gl_API Type operator*( Type const & p_a, Type const & p_b );
		C3D_Gl_API Type operator/( Type const & p_a, Type const & p_b );
		C3D_Gl_API Type operator+( Type const & p_a, float p_b );
		C3D_Gl_API Type operator-( Type const & p_a, float p_b );
		C3D_Gl_API Type operator*( Type const & p_a, float p_b );
		C3D_Gl_API Type operator/( Type const & p_a, float p_b );
		C3D_Gl_API Type operator+( Type const & p_a, int p_b );
		C3D_Gl_API Type operator-( Type const & p_a, int p_b );
		C3D_Gl_API Type operator*( Type const & p_a, int p_b );
		C3D_Gl_API Type operator/( Type const & p_a, int p_b );

		C3D_Gl_API Castor::String ToString( Type const & p_value );
	}
}

#endif

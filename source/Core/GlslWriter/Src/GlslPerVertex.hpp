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
#ifndef ___GLSL_PER_VERTEX_H___
#define ___GLSL_PER_VERTEX_H___

#include "GlslLight.hpp"

namespace GLSL
{
	struct gl_PerVertex
		: public Type
	{
		inline gl_PerVertex();
		inline gl_PerVertex( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline gl_PerVertex & operator=( gl_PerVertex const & p_rhs );
		template< typename T > inline gl_PerVertex & operator=( T const & p_rhs );
		inline Vec4 gl_Position()const;
		inline Float gl_PointSize()const;
		inline Float gl_ClipDistance()const;
	};
}

#include "GlslPerVertex.inl"

#endif

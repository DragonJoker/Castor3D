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
#ifndef ___GLSL_LIGHT_H___
#define ___GLSL_LIGHT_H___

#include "GlslMat.hpp"

namespace GlRender
{
	namespace GLSL
	{
		struct Light
			: public Type
		{
			inline Light();
			inline Light( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
			inline Light & operator=( Light const & p_rhs );
			template< typename T > inline Light & operator=( T const & p_rhs );
			inline Vec3 m_v3Colour()const;
			inline Vec3 m_v3Intensity()const;
			inline Vec3 m_v3Position()const;
			inline Int m_iType()const;
			inline Vec3 m_v3Attenuation()const;
			inline Vec3 m_v3Direction()const;
			inline Float m_fExponent()const;
			inline Float m_fCutOff()const;
		};
	}
}

#include "GlslLight.inl"

#endif

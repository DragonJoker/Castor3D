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
#ifndef ___GLSL_SAMPLER_H___
#define ___GLSL_SAMPLER_H___

#include "GlslWriter.hpp"

namespace GlRender
{
	namespace GLSL
	{
		struct SamplerBuffer
				: public Type
		{
			inline SamplerBuffer();
			inline SamplerBuffer( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
			template< typename T > inline SamplerBuffer & operator=( T const & p_rhs );
			inline operator uint32_t();
		};

		struct Sampler1D
				: public Type
		{
			inline Sampler1D();
			inline Sampler1D( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
			template< typename T > inline Sampler1D & operator=( T const & p_rhs );
			inline operator uint32_t();
		};

		struct Sampler2D
				: public Type
		{
			inline Sampler2D();
			inline Sampler2D( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
			template< typename T > inline Sampler2D & operator=( T const & p_rhs );
			inline operator uint32_t();
		};

		struct Sampler3D
				: public Type
		{
			inline Sampler3D();
			inline Sampler3D( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
			template< typename T > inline Sampler3D & operator=( T const & p_rhs );
			inline operator uint32_t();
		};
	}
}

#include "GlslSampler.inl"

#endif

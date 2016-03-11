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
#ifndef ___GLSL_BASE_TYPES_H___
#define ___GLSL_BASE_TYPES_H___

#include "GlslSampler.hpp"

namespace GLSL
{
	struct Int
			: public Type
	{
		inline Int();
		inline Int( Type const & p_value );
		inline Int( int p_value );
		inline Int( float p_value );
		inline Int( GlslWriter * p_writer, int p_value );
		inline Int( GlslWriter * p_writer, float p_value );
		inline Int( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline Int & operator=( Int const & p_rhs );
		template< typename T > inline Int & operator=( T const & p_rhs );
		template< typename T > inline Int & operator=( int p_rhs );
		inline explicit operator uint32_t();
		inline Int & operator++();
		inline Int operator++( int );
	};

	struct Float
			: public Type
	{
		inline Float();
		inline Float( Type const & p_value );
		inline Float( int p_value );
		inline Float( float p_value );
		inline Float( GlslWriter * p_writer, int p_value );
		inline Float( GlslWriter * p_writer, float p_value );
		inline Float( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline Float & operator=( Float const & p_rhs );
		template< typename T > inline Float & operator=( T const & p_rhs );
		template< typename T > inline Float & operator=( float p_rhs );
		inline explicit operator float();
	};
}

#include "GlslBaseTypes.inl"

#endif

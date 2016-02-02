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
#ifndef ___GLSL_FUNCTION_PARAM_H___
#define ___GLSL_FUNCTION_PARAM_H___

#include "GlslPerVertex.hpp"

namespace GlRender
{
	namespace GLSL
	{
		template< typename TypeT >
		struct InParam
				: public TypeT
		{
			InParam( GlslWriter * p_writer, Castor::String const & p_name );
			InParam( TypeT const & p_other );
		};

		template< typename TypeT >
		struct OutParam
				: public TypeT
		{
			OutParam( GlslWriter * p_writer, Castor::String const & p_name );
			OutParam( TypeT const & p_other );
			template< typename T > inline OutParam< TypeT > operator=( T const & p_rhs );
		};

		template< typename TypeT >
		struct InOutParam
				: public TypeT
		{
			InOutParam( GlslWriter * p_writer, Castor::String const & p_name );
			InOutParam( TypeT const & p_other );
			template< typename T > inline InOutParam< TypeT > operator=( T const & p_rhs );
		};
	}
}

#include "GlslFunctionParam.inl"

#endif

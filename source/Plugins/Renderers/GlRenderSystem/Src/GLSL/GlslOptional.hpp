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
#ifndef ___GLSL_OPTIONAL_H___
#define ___GLSL_OPTIONAL_H___

#include "GlslVoid.hpp"

namespace GlRender
{
	namespace GLSL
	{
		template< typename TypeT >
		struct Optional
			: public TypeT
		{
			Optional( GlslWriter * p_writer, Castor::String const & p_name, bool p_enabled );
			Optional( TypeT const & p_other, bool p_enabled );
			template< typename T > Optional( Castor::String const & p_name, T const & p_rhs, bool p_enabled );
			template< typename T > inline Optional< TypeT > operator=( T const & p_rhs );
			inline bool IsEnabled()const;

			inline operator Optional< Type >()const;

		private:
			bool m_enabled;
		};

		template< typename TypeT >
		inline Castor::String ParamToString( Castor::String & p_sep, Optional< TypeT > const & p_value );

		template< typename T >
		struct is_optional
			: public std::false_type
		{
		};

		template< typename T >
		struct is_optional< Optional< T > >
			: public std::true_type
		{
		};
	}
}

#include "GlslOptional.inl"

#endif

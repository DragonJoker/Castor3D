/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_PER_VERTEX_H___
#define ___GLSL_PER_VERTEX_H___

#include "GlslMat.hpp"

namespace glsl
{
	struct gl_PerVertex
		: public Type
	{
		inline gl_PerVertex();
		inline gl_PerVertex( GlslWriter & writer, castor::String const & p_name = castor::String() );
		inline gl_PerVertex & operator=( gl_PerVertex const & p_rhs );
		template< typename T > inline gl_PerVertex & operator=( T const & p_rhs );
		inline Vec4 gl_Position()const;
		inline Float gl_PointSize()const;
		inline Float gl_ClipDistance()const;
	};
}

#include "GlslPerVertex.inl"

#endif

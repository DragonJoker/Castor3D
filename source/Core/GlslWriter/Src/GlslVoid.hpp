/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_VOID_H___
#define ___GLSL_VOID_H___

#include "GlslType.hpp"

namespace glsl
{
	struct Void
		: public Type
	{
		inline Void();
		inline Void( GlslWriter * writer );
	};
}

#include "GlslVoid.inl"

#endif

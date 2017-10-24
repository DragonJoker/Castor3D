/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_InputLayout_H___
#define ___GLSL_InputLayout_H___

#include "GlslWriterPrerequisites.hpp"

namespace glsl
{
	struct InputLayout
	{
		enum class Kind
		{
			ePoints,
			eLines,
			eLinesAdjacency,
			eTriangles,
			eTrianglesAdjacency
		};

		explicit InputLayout( Kind p_kind )
			: m_kind{ p_kind }
		{
		}

		Kind m_kind;
	};
}

#endif

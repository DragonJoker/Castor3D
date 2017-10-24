/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_OutputLayout_H___
#define ___GLSL_OutputLayout_H___

#include "GlslWriterPrerequisites.hpp"

namespace glsl
{
	struct OutputLayout
	{
		enum class Kind
		{
			ePoints,
			eLineStrip,
			eTriangleStrip,
		};

		OutputLayout( Kind p_kind, uint32_t p_count )
			: m_kind{ p_kind }
			, m_count{ p_count }
		{
		}

		Kind m_kind;
		uint32_t m_count;
	};
}

#endif

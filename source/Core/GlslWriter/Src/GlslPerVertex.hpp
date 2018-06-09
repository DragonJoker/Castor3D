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
		enum Source
		{
			eVertexOutput,
			eTessellationControlInput,
			eTessellationControlOutput,
			eTessellationEvaluationInput,
			eTessellationEvaluationOutput,
			eGeometryInput,
			eGeometryOutput,
		};
		GlslWriter_API gl_PerVertex();
		GlslWriter_API gl_PerVertex( GlslWriter & writer, Source source = eVertexOutput );
		GlslWriter_API Vec4 gl_Position()const;
		GlslWriter_API Float gl_PointSize()const;
		GlslWriter_API Float gl_ClipDistance()const;
	};
}

#endif

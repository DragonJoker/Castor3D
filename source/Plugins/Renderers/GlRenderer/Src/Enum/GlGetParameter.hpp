/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlGetParameter
	{
		GL_SMOOTH_LINE_WIDTH_RANGE = 0x0B22,
		GL_ALIASED_LINE_WIDTH_RANGE = 0x846E,
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34,
	};
	std::string getName( GlGetParameter value );
}

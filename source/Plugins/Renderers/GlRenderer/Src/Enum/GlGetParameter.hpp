/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlGetParameter
	{
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34,
	};
	std::string getName( GlGetParameter value );
}

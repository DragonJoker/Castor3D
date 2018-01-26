/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlTweak
	{
		GL_CULL_FACE = 0x0B44,
		GL_DEPTH_TEST = 0x0B71,
		GL_STENCIL_TEST = 0x0B90,
		GL_BLEND = 0x0BE2,
		GL_POLYGON_OFFSET_POINT = 0x2A01,
		GL_POLYGON_OFFSET_LINE = 0x2A02,
		GL_MULTISAMPLE = 0x809D,
		GL_SAMPLE_ALPHA_TO_COVERAGE = 0x809E,
		GL_SAMPLE_ALPHA_TO_ONE = 0x809F,
		GL_POLYGON_OFFSET_FILL = 0x8037,
		GL_DEPTH_CLAMP = 0x864F,
		GL_RASTERIZER_DISCARD = 0x8C89,
	};
	std::string getName( GlTweak value );
}

/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/BufferTarget.hpp>

namespace gl_renderer
{
	enum GlClipOrigin
	{
		GL_LOWER_LEFT = 0x8CA1,
		GL_UPPER_LEFT = 0x8CA2,
	};

	enum GlClipDepth
	{
		GL_NEGATIVE_ONE_TO_ONE = 0x935E,
		GL_ZERO_TO_ONE = 0x935F,
	};
	std::string getName( GlClipOrigin value );
	std::string getName( GlClipDepth value );
}

/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace gl_renderer
{
	enum GlShaderBinaryFormat
		: GLenum
	{
		GL_SHADER_BINARY_FORMAT_SPIR_V = 0x9551
	};

	inline std::string getName( GlShaderBinaryFormat value )
	{
		switch ( value )
		{
		case GL_SHADER_BINARY_FORMAT_SPIR_V:
			return "GL_SHADER_BINARY_FORMAT_SPIR_V";

		default:
			return "GlShaderBinaryFormat_UNKNOWN";
		}
	}
}

/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/AccessFlag.hpp>

namespace gl_renderer
{
	enum GlTexParameter
	{
		GL_TEXTURE_VIEW_MIN_LEVEL = 0x82DB,
		GL_TEXTURE_VIEW_NUM_LEVELS = 0x82DC,
		GL_TEXTURE_VIEW_MIN_LAYER = 0x82DD,
		GL_TEXTURE_VIEW_NUM_LAYERS = 0x82DE,
		GL_TEXTURE_IMMUTABLE_LEVELS = 0x82DF,
		GL_TEXTURE_IMMUTABLE_FORMAT = 0x912F,
	};
	inline std::string getName( GlTexParameter value )
	{
		switch ( value )
		{
		case GL_TEXTURE_VIEW_MIN_LEVEL:
			return "GL_TEXTURE_VIEW_MIN_LEVEL";

		case GL_TEXTURE_VIEW_NUM_LEVELS:
			return "GL_TEXTURE_VIEW_NUM_LEVELS";

		case GL_TEXTURE_VIEW_MIN_LAYER:
			return "GL_TEXTURE_VIEW_MIN_LAYER";

		case GL_TEXTURE_VIEW_NUM_LAYERS:
			return "GL_TEXTURE_VIEW_NUM_LAYERS";

		case GL_TEXTURE_IMMUTABLE_LEVELS:
			return "GL_TEXTURE_IMMUTABLE_LEVELS";

		case GL_TEXTURE_IMMUTABLE_FORMAT:
			return "GL_TEXTURE_IMMUTABLE_FORMAT";

		default:
			assert( false && "Unsupported GlTexParameter" );
			return "GlTexParameter_UNKNOWN";
		}

		return std::string{};
	}
}

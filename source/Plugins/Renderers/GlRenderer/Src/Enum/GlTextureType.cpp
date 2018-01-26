#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlTextureType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_TEXTURE_1D:
			return "GL_TEXTURE_1D";

		case gl_renderer::GL_TEXTURE_2D:
			return "GL_TEXTURE_2D";

		case gl_renderer::GL_TEXTURE_3D:
			return "GL_TEXTURE_3D";

		case gl_renderer::GL_TEXTURE_CUBE_MAP:
			return "GL_TEXTURE_CUBE_MAP";

		case gl_renderer::GL_TEXTURE_1D_ARRAY:
			return "GL_TEXTURE_1D_ARRAY";

		case gl_renderer::GL_TEXTURE_2D_ARRAY:
			return "GL_TEXTURE_2D_ARRAY";

		case gl_renderer::GL_TEXTURE_CUBE_MAP_ARRAY:
			return "GL_TEXTURE_CUBE_MAP_ARRAY";

		case gl_renderer::GL_TEXTURE_2D_MULTISAMPLE:
			return "GL_TEXTURE_2D_MULTISAMPLE";

		case gl_renderer::GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			return "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";

		case gl_renderer::GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";

		case gl_renderer::GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";

		case gl_renderer::GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";

		case gl_renderer::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";

		case gl_renderer::GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";

		case gl_renderer::GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";

		default:
			assert( false && "Unsupported GlTextureType" );
			return "GlTextureType_UNKNOWN";
		}
	}

	GlTextureType convert( renderer::TextureType const & value )
	{
		switch ( value )
		{
		case renderer::TextureType::e1D:
			return GL_TEXTURE_1D;

		case renderer::TextureType::e2D:
			return GL_TEXTURE_2D;

		case renderer::TextureType::e3D:
			return GL_TEXTURE_3D;

		case renderer::TextureType::eCube:
			return GL_TEXTURE_CUBE_MAP;

		case renderer::TextureType::e1DArray:
			return GL_TEXTURE_1D_ARRAY;

		case renderer::TextureType::e2DArray:
			return GL_TEXTURE_2D_ARRAY;

		case renderer::TextureType::eCubeArray:
			return GL_TEXTURE_CUBE_MAP_ARRAY;

		default:
			assert( false && "Unsupported texture type" );
			return GL_TEXTURE_2D;
		}
	}
}

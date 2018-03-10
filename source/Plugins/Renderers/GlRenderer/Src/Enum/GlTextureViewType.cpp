#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlTextureViewType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_TEXTURE_VIEW_1D:
			return "GL_TEXTURE_VIEW_1D";

		case gl_renderer::GL_TEXTURE_VIEW_2D:
			return "GL_TEXTURE_VIEW_2D";

		case gl_renderer::GL_TEXTURE_VIEW_3D:
			return "GL_TEXTURE_VIEW_3D";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP:
			return "GL_TEXTURE_VIEW_CUBE_MAP";

		case gl_renderer::GL_TEXTURE_VIEW_1D_ARRAY:
			return "GL_TEXTURE_VIEW_1D_ARRAY";

		case gl_renderer::GL_TEXTURE_VIEW_2D_ARRAY:
			return "GL_TEXTURE_VIEW_2D_ARRAY";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP_ARRAY:
			return "GL_TEXTURE_VIEW_CUBE_MAP_ARRAY";

		case gl_renderer::GL_TEXTURE_VIEW_2D_MULTISAMPLE:
			return "GL_TEXTURE_VIEW_2D_MULTISAMPLE";

		case gl_renderer::GL_TEXTURE_VIEW_2D_MULTISAMPLE_ARRAY:
			return "GL_TEXTURE_VIEW_2D_MULTISAMPLE_ARRAY";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_X:
			return "GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_X";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_X:
			return "GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_X";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_Y:
			return "GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_Y";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_Y:
			return "GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_Y";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_Z:
			return "GL_TEXTURE_VIEW_CUBE_MAP_POSITIVE_Z";

		case gl_renderer::GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_Z:
			return "GL_TEXTURE_VIEW_CUBE_MAP_NEGATIVE_Z";

		default:
			assert( false && "Unsupported GlTextureViewType" );
			return "GlTextureViewType_UNKNOWN";
		}
	}

	GlTextureViewType convert( renderer::TextureViewType const & value )
	{
		switch ( value )
		{
		case renderer::TextureViewType::e1D:
			return GL_TEXTURE_VIEW_1D;

		case renderer::TextureViewType::e2D:
			return GL_TEXTURE_VIEW_2D;

		case renderer::TextureViewType::e3D:
			return GL_TEXTURE_VIEW_3D;

		case renderer::TextureViewType::eCube:
			return GL_TEXTURE_VIEW_CUBE_MAP;

		case renderer::TextureViewType::e1DArray:
			return GL_TEXTURE_VIEW_1D_ARRAY;

		case renderer::TextureViewType::e2DArray:
			return GL_TEXTURE_VIEW_2D_ARRAY;

		case renderer::TextureViewType::eCubeArray:
			return GL_TEXTURE_VIEW_CUBE_MAP_ARRAY;

		default:
			assert( false && "Unsupported TextureViewType" );
			return GL_TEXTURE_VIEW_2D;
		}
	}
}

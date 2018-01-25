#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
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

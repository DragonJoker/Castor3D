#include "Texture/GlTextureStorage.hpp"

#include "Common/OpenGl.hpp"

#include <Texture/TextureImage.hpp>

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureStorage::GlTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, TextureStorageType p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: TextureStorage{ p_type, p_image, p_cpuAccess, p_gpuAccess }
		, Holder{ p_gl }
		, m_glRenderSystem{ &p_renderSystem }
		, m_glType{ p_gl.Get( p_type ) }
	{
	}

	GlTextureStorage::~GlTextureStorage()
	{
	}

	void GlTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( p_format );

		switch ( m_glType )
		{
		case eGL_TEXTURE_STORAGE_1D:
			GetOpenGl().TexImage1D( m_glType, 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2D:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGX:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSY:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGY:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSZ:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGZ:
			GetOpenGl().TexImage2D( m_glType, 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			GetOpenGl().TexImage2DMultisample( m_glType, 0, l_glPixelFmt.Internal, p_size, true );
			break;

		case eGL_TEXTURE_STORAGE_3D:
		case eGL_TEXTURE_STORAGE_2DARRAY:
			GetOpenGl().TexImage3D( m_glType, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / GetOwner()->GetDepth(), GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;
		}
	}

	void GlTextureStorage::DoUploadImage( uint32_t p_width, uint32_t p_height, ePIXEL_FORMAT p_format, uint8_t const * p_buffer )
	{
		OpenGl::PixelFmt l_format = GetOpenGl().Get( p_format );

		switch ( m_glType )
		{
		case eGL_TEXTURE_STORAGE_1D:
			GetOpenGl().TexSubImage1D( m_glType, 0, 0, p_width, l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2D:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGX:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSY:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGY:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSZ:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGZ:
			GetOpenGl().TexSubImage2D( m_glType, 0, 0, 0, p_width, p_height, l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			GetOpenGl().TexSubImage2D( m_glType, 0, 0, 0, p_width, p_height, l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_3D:
		case eGL_TEXTURE_STORAGE_2DARRAY:
			GetOpenGl().TexSubImage3D( m_glType, 0, 0, 0, 0, p_width, p_height / GetOwner()->GetDepth(), GetOwner()->GetDepth(), l_format.Format, l_format.Type, p_buffer );
			break;
		}
	}
}

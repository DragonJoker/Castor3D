#include "Texture/GlGpuOnlyTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGpuOnlyTextureStorageTraits::GlGpuOnlyTextureStorageTraits( TextureStorage & p_storage )
	{
		auto l_buffer = p_storage.GetOwner()->GetBuffer();
		Fill( p_storage, nullptr, l_buffer->dimensions(), l_buffer->format() );
	}

	GlGpuOnlyTextureStorageTraits::~GlGpuOnlyTextureStorageTraits()
	{
	}

	bool GlGpuOnlyTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		return true;
	}

	void GlGpuOnlyTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlGpuOnlyTextureStorageTraits::Lock( TextureStorage & p_storage, AccessType p_lock )
	{
		FAILURE( "GPU only, you can't lock this storage" );
		return nullptr;
	}

	void GlGpuOnlyTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified )
	{
	}

	void GlGpuOnlyTextureStorageTraits::Fill( TextureStorage & p_storage, uint8_t const * p_buffer, Castor::Size const & p_size, Castor::PixelFormat p_format )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlGpuOnlyTextureStorageTraits > & >( p_storage );
		OpenGl::PixelFmt l_glPixelFmt = l_storage.GetOpenGl().Get( p_format );

		switch ( l_storage.GetGlType() )
		{
		case eGL_TEXTURE_STORAGE_1D:
			l_storage.GetOpenGl().TexImage1D( l_storage.GetGlType(), 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2D:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGX:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSY:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGY:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSZ:
		case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGZ:
			switch ( l_storage.GetType() )
			{
			case TextureStorageType::CubeMapArrayPositiveX:
			case TextureStorageType::CubeMapArrayNegativeX:
			case TextureStorageType::CubeMapArrayPositiveY:
			case TextureStorageType::CubeMapArrayNegativeY:
			case TextureStorageType::CubeMapArrayPositiveZ:
			case TextureStorageType::CubeMapArrayNegativeZ:
				l_storage.GetOpenGl().TexImage3D( eGL_TEXTURE_STORAGE_2DARRAY, 1, l_glPixelFmt.Internal, p_size.width(), p_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
				break;

			default:
				l_storage.GetOpenGl().TexImage2D( l_storage.GetGlType(), 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
				break;
			}
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			l_storage.GetOpenGl().TexImage2DMultisample( l_storage.GetGlType(), 0, l_glPixelFmt.Internal, p_size, true );
			break;

		case eGL_TEXTURE_STORAGE_3D:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2DARRAY:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 1, l_glPixelFmt.Internal, p_size.width(), p_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;
		}
	}
}

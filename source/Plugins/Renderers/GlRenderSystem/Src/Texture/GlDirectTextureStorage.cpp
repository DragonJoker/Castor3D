#include "Texture/GlDirectTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDirectTextureStorageTraits::GlDirectTextureStorageTraits( TextureStorage & p_storage )
	{
		auto l_buffer = p_storage.GetOwner()->GetBuffer();
		Fill( p_storage, l_buffer->const_ptr(), l_buffer->dimensions(), l_buffer->format() );
	}

	GlDirectTextureStorageTraits::~GlDirectTextureStorageTraits()
	{
	}

	bool GlDirectTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		return true;
	}

	void GlDirectTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlDirectTextureStorageTraits::Lock( TextureStorage & p_storage, AccessType p_lock )
	{
		uint8_t * l_return = nullptr;
		auto l_buffer = p_storage.GetOwner()->GetBuffer();

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Read )
			 && CheckFlag( p_lock, AccessType::Read ) )
		{
			auto & l_storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
			OpenGl::PixelFmt l_glPixelFmt = l_storage.GetOpenGl().Get( l_buffer->format() );
			l_storage.GetOpenGl().GetTexImage( l_storage.GetGlType(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, l_buffer->ptr() );
		}

		if ( CheckFlag( p_lock, AccessType::Read )
			 || CheckFlag( p_lock, AccessType::Write ) )
		{
			l_return = l_buffer->ptr();
		}

		return l_return;
	}

	void GlDirectTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified )
	{
		if ( p_modified && CheckFlag( p_storage.GetCPUAccess(), AccessType::Write ) )
		{
			auto & l_storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_storage.GetOwner()->GetBuffer();
			OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( l_buffer->format() );

			switch ( l_storage.GetGlType() )
			{
			case eGL_TEXTURE_STORAGE_1D:
				l_storage.GetOpenGl().TexSubImage1D( l_storage.GetGlType(), 0, 0, l_buffer->dimensions().width(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXTURE_STORAGE_2D:
			case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX:
			case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGX:
			case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSY:
			case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGY:
			case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSZ:
			case eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGZ:
				l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_buffer->dimensions().width(), l_buffer->dimensions().height(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXTURE_STORAGE_2DMS:
				l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_buffer->dimensions().width(), l_buffer->dimensions().height(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXTURE_STORAGE_3D:
			case eGL_TEXTURE_STORAGE_2DARRAY:
				l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, 0, l_buffer->dimensions().width(), l_buffer->dimensions().height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;
			}
		}
	}

	void GlDirectTextureStorageTraits::Fill( TextureStorage & p_storage, uint8_t const * p_buffer, Castor::Size const & p_size, Castor::PixelFormat p_format )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
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
			l_storage.GetOpenGl().TexImage2D( l_storage.GetGlType(), 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			l_storage.GetOpenGl().TexImage2DMultisample( l_storage.GetGlType(), 0, l_glPixelFmt.Internal, p_size, true );
			break;

		case eGL_TEXTURE_STORAGE_3D:
		case eGL_TEXTURE_STORAGE_2DARRAY:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;
		}
	}
}

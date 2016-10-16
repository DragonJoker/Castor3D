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
		auto & l_storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlType() )
		{
		case eGL_TEXTURE_STORAGE_1D:
			l_storage.GetOpenGl().TexImage1D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_2D:
			l_storage.GetOpenGl().TexImage2D( l_storage.GetGlType(), 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			l_storage.GetOpenGl().TexImage2DMultisample( l_storage.GetGlType(), 0, l_format.Internal, l_size, true );
			break;

		case eGL_TEXTURE_STORAGE_2DARRAY:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_3D:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP:
			l_storage.GetOpenGl().TexImage2D( l_storage.GetGlType(), 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP_ARRAY:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 1, l_format.Internal, l_size.width(), l_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth() * 6, 0, l_format.Format, l_format.Type, nullptr );
			break;
		}
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

	uint8_t * GlDirectTextureStorageTraits::Lock( TextureStorage & p_storage, AccessType p_lock, uint32_t p_index )
	{
		uint8_t * l_return = nullptr;
		auto l_buffer = p_storage.GetOwner()->GetImage( p_index ).GetBuffer();

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

	void GlDirectTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		if ( p_modified && CheckFlag( p_storage.GetCPUAccess(), AccessType::Write ) )
		{
			Fill( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}
	}

	void GlDirectTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlType() )
		{
		case eGL_TEXTURE_STORAGE_1D:
			l_storage.GetOpenGl().TexSubImage1D( l_storage.GetGlType(), 0, 0, l_size.width(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_2D:
			l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_2DARRAY:
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height() / p_storage.GetOwner()->GetLayersCount(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_3D:
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height() / p_storage.GetOwner()->GetDepth(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP:
			l_storage.GetOpenGl().TexSubImage2D( eGL_TEXTURE_STORAGE( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX + p_image.GetIndex() ), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP_ARRAY:
			l_storage.GetOpenGl().TexSubImage3D( eGL_TEXTURE_STORAGE( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX + p_image.GetIndex() ), 1, 0, 0, 0 , l_size.width(), l_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;
		}
	}
}

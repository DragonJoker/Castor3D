#include "GlImmutableTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlImmutableTextureStorageTraits::GlImmutableTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlImmutableTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlType() )
		{
		case eGL_TEXTURE_STORAGE_1D:
			l_storage.GetOpenGl().TexStorage1D( l_storage.GetGlType(), 4, l_format.Internal, l_size.width() );
			break;

		case eGL_TEXTURE_STORAGE_2D:
			l_storage.GetOpenGl().TexStorage2D( l_storage.GetGlType(), 4, l_format.Internal, l_size.width(), l_size.height() );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP:
			l_storage.GetOpenGl().TexStorage2D( l_storage.GetGlType(), 4, l_format.Internal, l_size.width(), l_size.height() );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			l_storage.GetOpenGl().TexStorage2DMultisample( l_storage.GetGlType(), 8, l_format.Internal, l_size.width(), l_size.height(), true );
			break;

		case eGL_TEXTURE_STORAGE_2DARRAY:
			l_storage.GetOpenGl().TexStorage3D( l_storage.GetGlType(), 4, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP_ARRAY:
			l_storage.GetOpenGl().TexStorage3D( l_storage.GetGlType(), 4, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case eGL_TEXTURE_STORAGE_3D:
			l_storage.GetOpenGl().TexStorage3D( l_storage.GetGlType(), 4, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		default:
			FAILURE( "Storage type unsupported for immutable storages" );
			CASTOR_EXCEPTION( cuT( "Storage type unsupported for immutable storages" ) );
			break;
		}
	}

	GlImmutableTextureStorageTraits::~GlImmutableTextureStorageTraits()
	{
	}

	bool GlImmutableTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		return true;
	}

	void GlImmutableTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlImmutableTextureStorageTraits::Lock( TextureStorage & p_storage, AccessType p_lock, uint32_t p_index )
	{
		uint8_t * l_return = nullptr;

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Read )
			 && CheckFlag( p_lock, AccessType::Read ) )
		{
			l_return = p_storage.GetOwner()->GetImage( p_index ).GetBuffer()->ptr();
		}

		return l_return;
	}

	void GlImmutableTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlImmutableTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlImmutableTextureStorageTraits > & >( p_storage );
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
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_3D:
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP:
			l_storage.GetOpenGl().TexSubImage2D( eGL_TEXTURE_STORAGE( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX + p_image.GetIndex() ), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP_ARRAY:
			l_storage.GetOpenGl().TexSubImage3D( eGL_TEXTURE_STORAGE( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX + p_image.GetIndex() % 6 ), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;
		}
	}
}

#include "GlES3ImmutableTextureStorage.hpp"

#include "Common/OpenGlES3.hpp"
#include "Texture/GlES3Texture.hpp"
#include "Texture/GlES3TextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3ImmutableTextureStorageTraits::GlES3ImmutableTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3ImmutableTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGlES3::PixelFmt l_format = l_storage.GetOpenGlES3().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlES3Type() )
		{
		case GlES3TextureStorageType::e1D:
			l_storage.GetOpenGlES3().TexStorage1D( l_storage.GetGlES3Type(), 4, l_format.Internal, l_size.width() );
			break;

		case GlES3TextureStorageType::e2D:
			l_storage.GetOpenGlES3().TexStorage2D( l_storage.GetGlES3Type(), 4, l_format.Internal, l_size.width(), l_size.height() );
			break;

		case GlES3TextureStorageType::eCubeMap:
			l_storage.GetOpenGlES3().TexStorage2D( l_storage.GetGlES3Type(), 4, l_format.Internal, l_size.width(), l_size.height() );
			break;

		case GlES3TextureStorageType::e2DMS:
			l_storage.GetOpenGlES3().TexStorage2DMultisample( l_storage.GetGlES3Type(), 8, l_format.Internal, l_size.width(), l_size.height(), true );
			break;

		case GlES3TextureStorageType::e2DArray:
			l_storage.GetOpenGlES3().TexStorage3D( l_storage.GetGlES3Type(), 4, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case GlES3TextureStorageType::eCubeMapArray:
			l_storage.GetOpenGlES3().TexStorage3D( l_storage.GetGlES3Type(), 4, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case GlES3TextureStorageType::e3D:
			l_storage.GetOpenGlES3().TexStorage3D( l_storage.GetGlES3Type(), 4, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		default:
			FAILURE( "Storage type unsupported for immutable storages" );
			CASTOR_EXCEPTION( cuT( "Storage type unsupported for immutable storages" ) );
			break;
		}
	}

	GlES3ImmutableTextureStorageTraits::~GlES3ImmutableTextureStorageTraits()
	{
	}

	void GlES3ImmutableTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlES3ImmutableTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlES3ImmutableTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * l_return = nullptr;

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead )
			 && CheckFlag( p_lock, AccessType::eRead ) )
		{
			l_return = p_storage.GetOwner()->GetImage( p_index ).GetBuffer()->ptr();
		}

		return l_return;
	}

	void GlES3ImmutableTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlES3ImmutableTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3ImmutableTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGlES3::PixelFmt l_format = l_storage.GetOpenGlES3().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlES3Type() )
		{
		case GlES3TextureStorageType::e1D:
			l_storage.GetOpenGlES3().TexSubImage1D( l_storage.GetGlES3Type(), 0, 0, l_size.width(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlES3TextureStorageType::e2D:
			l_storage.GetOpenGlES3().TexSubImage2D( l_storage.GetGlES3Type(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlES3TextureStorageType::e2DMS:
			l_storage.GetOpenGlES3().TexSubImage2D( l_storage.GetGlES3Type(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlES3TextureStorageType::e2DArray:
			l_storage.GetOpenGlES3().TexSubImage3D( l_storage.GetGlES3Type(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlES3TextureStorageType::e3D:
			l_storage.GetOpenGlES3().TexSubImage3D( l_storage.GetGlES3Type(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlES3TextureStorageType::eCubeMap:
			l_storage.GetOpenGlES3().TexSubImage2D( GlES3TextureStorageType( uint32_t( GlES3TextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() ), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlES3TextureStorageType::eCubeMapArray:
			l_storage.GetOpenGlES3().TexSubImage3D( GlES3TextureStorageType( uint32_t( GlES3TextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() % 6 ), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;
		}
	}
}

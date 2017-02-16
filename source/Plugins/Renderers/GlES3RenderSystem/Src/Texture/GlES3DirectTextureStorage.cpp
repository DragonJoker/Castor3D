#include "Texture/GlES3DirectTextureStorage.hpp"

#include "Common/OpenGlES3.hpp"
#include "Texture/GlES3Texture.hpp"
#include "Texture/GlES3TextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3DirectTextureStorageTraits::GlES3DirectTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3DirectTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGlES3::PixelFmt l_format = l_storage.GetOpenGlES3().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlES3Type() )
		{
		case GlES3TextureStorageType::e1D:
			l_storage.GetOpenGlES3().TexImage1D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlES3TextureStorageType::e2D:
			l_storage.GetOpenGlES3().TexImage2D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlES3TextureStorageType::e2DMS:
			l_storage.GetOpenGlES3().TexImage2DMultisample( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size, true );
			break;

		case GlES3TextureStorageType::e2DArray:
			l_storage.GetOpenGlES3().TexImage3D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlES3TextureStorageType::e3D:
			l_storage.GetOpenGlES3().TexImage3D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlES3TextureStorageType::eCubeMap:
			l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFacePosX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFaceNegX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFacePosY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFaceNegY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFacePosZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFaceNegZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlES3TextureStorageType::eCubeMapArray:
			l_storage.GetOpenGlES3().TexImage3D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() * 6, 0, l_format.Format, l_format.Type, nullptr );
			break;
		}
	}

	GlES3DirectTextureStorageTraits::~GlES3DirectTextureStorageTraits()
	{
	}

	void GlES3DirectTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlES3DirectTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlES3DirectTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * l_return = nullptr;
		auto l_buffer = p_storage.GetOwner()->GetImage( p_index ).GetBuffer();
		CASTOR_ASSERT( !CheckFlag( p_lock, AccessType::eRead ), "OpenGL ES 3.x doesn't support read from textures." );

		if ( CheckFlag( p_lock, AccessType::eWrite ) )
		{
			l_return = l_buffer->ptr();
		}

		return l_return;
	}

	void GlES3DirectTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		if ( p_modified && CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			Fill( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}
	}

	void GlES3DirectTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3DirectTextureStorageTraits > & >( p_storage );
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

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
		auto & storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
		auto size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt format = storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( storage.GetGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.GetOpenGl().TexImage1D( storage.GetGlType(), 0, format.Internal, size.width(), 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::e2D:
			storage.GetOpenGl().TexImage2D( storage.GetGlType(), 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::e2DMS:
			storage.GetOpenGl().TexImage2DMultisample( storage.GetGlType(), 0, format.Internal, size, true );
			break;

		case GlTextureStorageType::e2DArray:
			storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth(), 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::e3D:
			storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth(), 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosX, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegX, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosY, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegY, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosZ, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegZ, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth() * 6, 0, format.Format, format.Type, nullptr );
			break;
		}
	}

	GlDirectTextureStorageTraits::~GlDirectTextureStorageTraits()
	{
	}

	void GlDirectTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlDirectTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlDirectTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * result = nullptr;
		auto buffer = p_storage.GetOwner()->GetImage( p_index ).GetBuffer();

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead )
			 && CheckFlag( p_lock, AccessType::eRead ) )
		{
			auto & storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
			OpenGl::PixelFmt glPixelFmt = storage.GetOpenGl().Get( buffer->format() );
			storage.GetOpenGl().GetTexImage( storage.GetGlType(), 0, glPixelFmt.Format, glPixelFmt.Type, buffer->ptr() );
		}

		if ( CheckFlag( p_lock, AccessType::eRead )
			 || CheckFlag( p_lock, AccessType::eWrite ) )
		{
			result = buffer->ptr();
		}

		return result;
	}

	void GlDirectTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		if ( p_modified && CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			Fill( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}
	}

	void GlDirectTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
		auto size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt format = storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( storage.GetGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.GetOpenGl().TexSubImage1D( storage.GetGlType(), 0, 0, size.width(), format.Format, format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e2D:
			storage.GetOpenGl().TexSubImage2D( storage.GetGlType(), 0, 0, 0, size.width(), size.height(), format.Format, format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e2DMS:
			storage.GetOpenGl().TexSubImage2D( storage.GetGlType(), 0, 0, 0, size.width(), size.height(), format.Format, format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e2DArray:
			storage.GetOpenGl().TexSubImage3D( storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), size.width(), size.height(), p_image.GetIndex() + 1, format.Format, format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e3D:
			storage.GetOpenGl().TexSubImage3D( storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), size.width(), size.height(), p_image.GetIndex() + 1, format.Format, format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.GetOpenGl().TexSubImage2D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() ), 0, 0, 0, size.width(), size.height(), format.Format, format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.GetOpenGl().TexSubImage3D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() % 6 ), 0, 0, 0, p_image.GetIndex(), size.width(), size.height(), p_image.GetIndex() + 1, format.Format, format.Type, p_image.GetBuffer()->const_ptr() );
			break;
		}
	}
}

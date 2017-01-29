#include "Texture/GlES3GpuOnlyTextureStorage.hpp"

#include "Common/OpenGlES3.hpp"
#include "Texture/GlES3Texture.hpp"
#include "Texture/GlES3TextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3GpuOnlyTextureStorageTraits::GlES3GpuOnlyTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3GpuOnlyTextureStorageTraits > & >( p_storage );
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
			l_storage.GetOpenGlES3().TexImage3D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), l_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
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

	GlES3GpuOnlyTextureStorageTraits::~GlES3GpuOnlyTextureStorageTraits()
	{
	}

	void GlES3GpuOnlyTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlES3GpuOnlyTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlES3GpuOnlyTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		FAILURE( "GPU only, you can't lock this storage" );
		return nullptr;
	}

	void GlES3GpuOnlyTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlES3GpuOnlyTextureStorageTraits::Fill( TextureStorage & p_storage, Castor3D::TextureImage const & p_image )
	{
	}
}

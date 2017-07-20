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
		auto & storage = static_cast< GlTextureStorage< GlGpuOnlyTextureStorageTraits > & >( p_storage );
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
			storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetLayersCount(), 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::e3D:
			storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, format.Format, format.Type, nullptr );
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
			storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetLayersCount() * 6, 0, format.Format, format.Type, nullptr );
			break;
		}
	}

	GlGpuOnlyTextureStorageTraits::~GlGpuOnlyTextureStorageTraits()
	{
	}

	void GlGpuOnlyTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlGpuOnlyTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlGpuOnlyTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		FAILURE( "GPU only, you can't lock this storage" );
		return nullptr;
	}

	void GlGpuOnlyTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlGpuOnlyTextureStorageTraits::Fill( TextureStorage & p_storage, Castor3D::TextureImage const & p_image )
	{
	}
}

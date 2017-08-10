#include "Texture/GlGpuOnlyTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlGpuOnlyTextureStorageTraits::GlGpuOnlyTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & storage = static_cast< GlTextureStorage< GlGpuOnlyTextureStorageTraits > & >( p_storage );
		auto size = p_storage.getOwner()->getDimensions();
		OpenGl::PixelFmt format = storage.getOpenGl().get( p_storage.getOwner()->getPixelFormat() );

		switch ( storage.getGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.getOpenGl().TexImage1D( storage.getGlType(), 0, format.Internal, size.getWidth(), 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::e2D:
			storage.getOpenGl().TexImage2D( storage.getGlType(), 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::e2DMS:
			storage.getOpenGl().TexImage2DMultisample( storage.getGlType(), 0, format.Internal, size, true );
			break;

		case GlTextureStorageType::e2DArray:
			storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getLayersCount(), 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::e3D:
			storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight() / p_storage.getOwner()->getDepth(), p_storage.getOwner()->getDepth(), 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosX, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegX, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosY, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegY, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosZ, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegZ, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getLayersCount() * 6, 0, format.Format, format.Type, nullptr );
			break;
		}
	}

	GlGpuOnlyTextureStorageTraits::~GlGpuOnlyTextureStorageTraits()
	{
	}

	void GlGpuOnlyTextureStorageTraits::bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlGpuOnlyTextureStorageTraits::unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlGpuOnlyTextureStorageTraits::lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		FAILURE( "GPU only, you can't lock this storage" );
		return nullptr;
	}

	void GlGpuOnlyTextureStorageTraits::unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlGpuOnlyTextureStorageTraits::fill( TextureStorage & p_storage, castor3d::TextureImage const & p_image )
	{
	}
}

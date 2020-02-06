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
		auto & l_storage = static_cast< GlTextureStorage< GlGpuOnlyTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlType() )
		{
		case GlTextureStorageType::e1D:
			l_storage.GetOpenGl().TexImage1D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlTextureStorageType::e2D:
			l_storage.GetOpenGl().TexImage2D( l_storage.GetGlType(), 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlTextureStorageType::e2DMS:
			l_storage.GetOpenGl().TexImage2DMultisample( l_storage.GetGlType(), 0, l_format.Internal, l_size, true );
			break;

		case GlTextureStorageType::e2DArray:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetLayersCount(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlTextureStorageType::e3D:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height() / p_storage.GetOwner()->GetDepth(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlTextureStorageType::eCubeMap:
			l_storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			break;

		case GlTextureStorageType::eCubeMapArray:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetLayersCount() * 6, 0, l_format.Format, l_format.Type, nullptr );
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

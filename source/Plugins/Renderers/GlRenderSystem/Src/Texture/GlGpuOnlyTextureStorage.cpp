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

	GlGpuOnlyTextureStorageTraits::~GlGpuOnlyTextureStorageTraits()
	{
	}

	bool GlGpuOnlyTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		return true;
	}

	void GlGpuOnlyTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlGpuOnlyTextureStorageTraits::Lock( TextureStorage & p_storage, AccessType p_lock, uint32_t p_index )
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

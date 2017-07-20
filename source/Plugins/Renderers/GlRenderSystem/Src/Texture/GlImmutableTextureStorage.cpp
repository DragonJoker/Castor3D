#include "GlImmutableTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

#ifdef min
#	undef min
#endif

namespace GlRender
{
	namespace
	{
		GLint DoGetMinLevels( Size const & p_size )
		{
			auto minDim = std::min( p_size.width(), p_size.height() );
			auto levels = 1;

			while ( minDim > 2 )
			{
				++levels;
				minDim /= 2;
			}

			return levels;
		}
	}

	GlImmutableTextureStorageTraits::GlImmutableTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & storage = static_cast< GlTextureStorage< GlImmutableTextureStorageTraits > & >( p_storage );
		auto size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt format = storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		auto levels = DoGetMinLevels( size );

		switch ( storage.GetGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.GetOpenGl().TexStorage1D( storage.GetGlType(), levels, format.Internal, size.width() );
			break;

		case GlTextureStorageType::e2D:
			storage.GetOpenGl().TexStorage2D( storage.GetGlType(), levels, format.Internal, size.width(), size.height() );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.GetOpenGl().TexStorage2D( storage.GetGlType(), levels, format.Internal, size.width(), size.height() );
			break;

		case GlTextureStorageType::e2DMS:
			storage.GetOpenGl().TexStorage2DMultisample( storage.GetGlType(), levels, format.Internal, size.width(), size.height(), true );
			break;

		case GlTextureStorageType::e2DArray:
			storage.GetOpenGl().TexStorage3D( storage.GetGlType(), levels, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.GetOpenGl().TexStorage3D( storage.GetGlType(), levels, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case GlTextureStorageType::e3D:
			storage.GetOpenGl().TexStorage3D( storage.GetGlType(), levels, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth() );
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

	void GlImmutableTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlImmutableTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlImmutableTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * result = nullptr;

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead )
			 && CheckFlag( p_lock, AccessType::eRead ) )
		{
			result = p_storage.GetOwner()->GetImage( p_index ).GetBuffer()->ptr();
		}

		return result;
	}

	void GlImmutableTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlImmutableTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & storage = static_cast< GlTextureStorage< GlImmutableTextureStorageTraits > & >( p_storage );
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

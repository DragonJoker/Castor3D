#include "GlImmutableTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace castor3d;
using namespace castor;

#ifdef min
#	undef min
#endif

namespace GlRender
{
	namespace
	{
		GLint doGetMinLevels( Size const & p_size )
		{
			auto minDim = std::min( p_size.getWidth(), p_size.getHeight() );
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
		auto size = p_storage.getOwner()->getDimensions();
		OpenGl::PixelFmt format = storage.getOpenGl().get( p_storage.getOwner()->getPixelFormat() );

		auto levels = doGetMinLevels( size );

		switch ( storage.getGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.getOpenGl().TexStorage1D( storage.getGlType(), levels, format.Internal, size.getWidth() );
			break;

		case GlTextureStorageType::e2D:
			storage.getOpenGl().TexStorage2D( storage.getGlType(), levels, format.Internal, size.getWidth(), size.getHeight() );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.getOpenGl().TexStorage2D( storage.getGlType(), levels, format.Internal, size.getWidth(), size.getHeight() );
			break;

		case GlTextureStorageType::e2DMS:
			storage.getOpenGl().TexStorage2DMultisample( storage.getGlType(), levels, format.Internal, size.getWidth(), size.getHeight(), true );
			break;

		case GlTextureStorageType::e2DArray:
			storage.getOpenGl().TexStorage3D( storage.getGlType(), levels, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth() );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.getOpenGl().TexStorage3D( storage.getGlType(), levels, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth() );
			break;

		case GlTextureStorageType::e3D:
			storage.getOpenGl().TexStorage3D( storage.getGlType(), levels, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth() );
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

	void GlImmutableTextureStorageTraits::bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlImmutableTextureStorageTraits::unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlImmutableTextureStorageTraits::lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * result = nullptr;

		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eRead )
			 && checkFlag( p_lock, AccessType::eRead ) )
		{
			result = p_storage.getOwner()->getImage( p_index ).getBuffer()->ptr();
		}

		return result;
	}

	void GlImmutableTextureStorageTraits::unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlImmutableTextureStorageTraits::fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & storage = static_cast< GlTextureStorage< GlImmutableTextureStorageTraits > & >( p_storage );
		auto size = p_storage.getOwner()->getDimensions();
		OpenGl::PixelFmt format = storage.getOpenGl().get( p_storage.getOwner()->getPixelFormat() );

		switch ( storage.getGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.getOpenGl().TexSubImage1D( storage.getGlType(), 0, 0, size.getWidth(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			break;

		case GlTextureStorageType::e2D:
			storage.getOpenGl().TexSubImage2D( storage.getGlType(), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			break;

		case GlTextureStorageType::e2DMS:
			storage.getOpenGl().TexSubImage2D( storage.getGlType(), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			break;

		case GlTextureStorageType::e2DArray:
			storage.getOpenGl().TexSubImage3D( storage.getGlType(), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), p_image.getIndex() + 1, format.Format, format.Type, p_image.getBuffer()->constPtr() );
			break;

		case GlTextureStorageType::e3D:
			storage.getOpenGl().TexSubImage3D( storage.getGlType(), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), p_image.getIndex() + 1, format.Format, format.Type, p_image.getBuffer()->constPtr() );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.getOpenGl().TexSubImage2D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.getIndex() ), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.getOpenGl().TexSubImage3D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.getIndex() % 6 ), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), p_image.getIndex() + 1, format.Format, format.Type, p_image.getBuffer()->constPtr() );
			break;
		}
	}
}

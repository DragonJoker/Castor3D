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
			auto l_minDim = std::min( p_size.width(), p_size.height() );
			auto l_levels = 1;

			while ( l_minDim > 2 )
			{
				++l_levels;
				l_minDim /= 2;
			}

			return l_levels;
		}
	}

	GlImmutableTextureStorageTraits::GlImmutableTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlImmutableTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		auto l_levels = DoGetMinLevels( l_size );

		switch ( l_storage.GetGlType() )
		{
		case GlTextureStorageType::e1D:
			l_storage.GetOpenGl().TexStorage1D( l_storage.GetGlType(), l_levels, l_format.Internal, l_size.width() );
			break;

		case GlTextureStorageType::e2D:
			l_storage.GetOpenGl().TexStorage2D( l_storage.GetGlType(), l_levels, l_format.Internal, l_size.width(), l_size.height() );
			break;

		case GlTextureStorageType::eCubeMap:
			l_storage.GetOpenGl().TexStorage2D( l_storage.GetGlType(), l_levels, l_format.Internal, l_size.width(), l_size.height() );
			break;

		case GlTextureStorageType::e2DMS:
			l_storage.GetOpenGl().TexStorage2DMultisample( l_storage.GetGlType(), l_levels, l_format.Internal, l_size.width(), l_size.height(), true );
			break;

		case GlTextureStorageType::e2DArray:
			l_storage.GetOpenGl().TexStorage3D( l_storage.GetGlType(), l_levels, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case GlTextureStorageType::eCubeMapArray:
			l_storage.GetOpenGl().TexStorage3D( l_storage.GetGlType(), l_levels, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
			break;

		case GlTextureStorageType::e3D:
			l_storage.GetOpenGl().TexStorage3D( l_storage.GetGlType(), l_levels, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() );
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
		uint8_t * l_result = nullptr;

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead )
			 && CheckFlag( p_lock, AccessType::eRead ) )
		{
			l_result = p_storage.GetOwner()->GetImage( p_index ).GetBuffer()->ptr();
		}

		return l_result;
	}

	void GlImmutableTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
	}

	void GlImmutableTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlImmutableTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlType() )
		{
		case GlTextureStorageType::e1D:
			l_storage.GetOpenGl().TexSubImage1D( l_storage.GetGlType(), 0, 0, l_size.width(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e2D:
			l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e2DMS:
			l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e2DArray:
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::e3D:
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::eCubeMap:
			l_storage.GetOpenGl().TexSubImage2D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() ), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;

		case GlTextureStorageType::eCubeMapArray:
			l_storage.GetOpenGl().TexSubImage3D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() % 6 ), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_image.GetBuffer()->const_ptr() );
			break;
		}
	}
}

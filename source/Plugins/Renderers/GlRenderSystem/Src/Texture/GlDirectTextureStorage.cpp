#include "Texture/GlDirectTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlDirectTextureStorageTraits::GlDirectTextureStorageTraits( TextureStorage & p_storage )
	{
		auto & storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
		auto size = p_storage.getOwner()->getDimensions();
		OpenGl::PixelFmt format = storage.getOpenGl().get( p_storage.getOwner()->getPixelFormat() );

		if ( storage.getGlType() == GlTextureStorageType::e2DArray
			|| storage.getGlType() == GlTextureStorageType::e3D
			|| storage.getGlType() == GlTextureStorageType::eCubeMapArray )
		{
			switch ( storage.getGlType() )
			{
			case GlTextureStorageType::e2DArray:
				storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth(), 0, format.Format, format.Type, nullptr );
				GlDebug_Set( m_allocatedSize, size.getWidth() * size.getHeight() * p_storage.getOwner()->getDepth() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
				break;

			case GlTextureStorageType::e3D:
				storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth(), 0, format.Format, format.Type, nullptr );
				GlDebug_Set( m_allocatedSize, size.getWidth() * size.getHeight() * p_storage.getOwner()->getDepth() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
				break;

			case GlTextureStorageType::eCubeMapArray:
				storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth() * 6, 0, format.Format, format.Type, nullptr );
				GlDebug_Set( m_allocatedSize, size.getWidth() * size.getHeight() * p_storage.getOwner()->getDepth() * 6u * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
				break;
			}
		}
		else
		{
			if ( p_storage.getOwner()->getMipmapCount() == ~( 0u ) )
			{
				switch ( storage.getGlType() )
				{
				case GlTextureStorageType::e1D:
					storage.getOpenGl().TexImage1D( storage.getGlType(), 0, format.Internal, size.getWidth(), 0, format.Format, format.Type, nullptr );
					GlDebug_Set( m_allocatedSize, size.getWidth() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
					break;

				case GlTextureStorageType::e2D:
					storage.getOpenGl().TexImage2D( storage.getGlType(), 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
					GlDebug_Set( m_allocatedSize, size.getWidth() * size.getHeight() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
					break;

				case GlTextureStorageType::e2DMS:
					storage.getOpenGl().TexImage2DMultisample( storage.getGlType(), 0, format.Internal, size, true );
					GlDebug_Set( m_allocatedSize, size.getWidth() * size.getHeight() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
					break;

				case GlTextureStorageType::eCubeMap:
					storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosX, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
					storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegX, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
					storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosY, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
					storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegY, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
					storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFacePosZ, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
					storage.getOpenGl().TexImage2D( GlTextureStorageType::eCubeMapFaceNegZ, 0, format.Internal, size, 0, format.Format, format.Type, nullptr );
					GlDebug_Set( m_allocatedSize, size.getWidth() * size.getHeight() * 6u * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
					break;
				}
			}
			else
			{
				auto levels = p_storage.getOwner()->getMipmapCount();

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
		}
	}

	GlDirectTextureStorageTraits::~GlDirectTextureStorageTraits()
	{
	}

	void GlDirectTextureStorageTraits::bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlDirectTextureStorageTraits::unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlDirectTextureStorageTraits::lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * result = nullptr;
		auto buffer = p_storage.getOwner()->getImage( p_index ).getBuffer();

		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eRead )
			 && checkFlag( p_lock, AccessType::eRead ) )
		{
			auto & storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
			OpenGl::PixelFmt glPixelFmt = storage.getOpenGl().get( buffer->format() );
			storage.getOpenGl().GetTexImage( storage.getGlType(), 0, glPixelFmt.Format, glPixelFmt.Type, buffer->ptr() );
		}

		if ( checkFlag( p_lock, AccessType::eRead )
			 || checkFlag( p_lock, AccessType::eWrite ) )
		{
			result = buffer->ptr();
		}

		return result;
	}

	void GlDirectTextureStorageTraits::unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		if ( p_modified && checkFlag( p_storage.getCPUAccess(), AccessType::eWrite ) )
		{
			fill( p_storage, p_storage.getOwner()->getImage( p_index ) );
		}
	}

	void GlDirectTextureStorageTraits::fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto & storage = static_cast< GlTextureStorage< GlDirectTextureStorageTraits > & >( p_storage );
		auto size = p_storage.getOwner()->getDimensions();
		OpenGl::PixelFmt format = storage.getOpenGl().get( p_storage.getOwner()->getPixelFormat() );

		switch ( storage.getGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.getOpenGl().TexSubImage1D( storage.getGlType(), 0, 0, size.getWidth(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			GlDebug_Check( m_allocatedSize, size.getWidth() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
			break;

		case GlTextureStorageType::e2D:
			storage.getOpenGl().TexSubImage2D( storage.getGlType(), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			GlDebug_Check( m_allocatedSize, size.getWidth() * size.getHeight() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
			break;

		case GlTextureStorageType::e2DMS:
			storage.getOpenGl().TexSubImage2D( storage.getGlType(), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			GlDebug_Check( m_allocatedSize, size.getWidth() * size.getHeight() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
			break;

		case GlTextureStorageType::e2DArray:
			storage.getOpenGl().TexSubImage3D( storage.getGlType(), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), 1, format.Format, format.Type, p_image.getBuffer()->constPtr() );
			GlDebug_Check( m_allocatedSize, size.getWidth() * size.getHeight() * p_storage.getOwner()->getDepth() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
			break;

		case GlTextureStorageType::e3D:
			storage.getOpenGl().TexSubImage3D( storage.getGlType(), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), 1, format.Format, format.Type, p_image.getBuffer()->constPtr() );
			GlDebug_Check( m_allocatedSize, size.getWidth() * size.getHeight() * p_storage.getOwner()->getDepth() * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.getOpenGl().TexSubImage2D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.getIndex() ), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_image.getBuffer()->constPtr() );
			GlDebug_Check( m_allocatedSize, size.getWidth() * size.getHeight() * 6u * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.getOpenGl().TexSubImage3D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.getIndex() % 6 ), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), p_image.getIndex() + 1, format.Format, format.Type, p_image.getBuffer()->constPtr() );
			GlDebug_Check( m_allocatedSize, size.getWidth() * size.getHeight() * p_storage.getOwner()->getDepth() * 6u * PF::getBytesPerPixel( p_storage.getOwner()->getPixelFormat() ) );
			break;
		}
	}
}

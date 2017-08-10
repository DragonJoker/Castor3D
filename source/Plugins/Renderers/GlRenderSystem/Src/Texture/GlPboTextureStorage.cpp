#include "Texture/GlPboTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlDownloadPixelBuffer.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"
#include "Texture/GlUploadPixelBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlPboTextureStorageTraits::GlPboTextureStorageTraits( TextureStorage & p_storage )
		: m_storage{ p_storage }
	{
		bool result = true;
		auto pxbuffer = p_storage.getOwner()->getImage().getBuffer();
		auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );

		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eWrite ) )
		{
			m_uploadBuffer = std::make_unique< GlUploadPixelBuffer >( storage.getOpenGl(), storage.getGlRenderSystem(), pxbuffer->size() );
			result = m_uploadBuffer != nullptr;

			if ( result )
			{
				result = m_uploadBuffer->create();
			}

			if ( result )
			{
				result = m_uploadBuffer->initialise();
			}
		}

		if ( result && checkFlag( p_storage.getCPUAccess(), AccessType::eRead ) )
		{
			m_downloadBuffer  = std::make_unique< GlDownloadPixelBuffer >( storage.getOpenGl(), storage.getGlRenderSystem(), pxbuffer->size() );
			result = m_downloadBuffer != nullptr;

			if ( result )
			{
				result = m_downloadBuffer->create();
			}

			if ( result )
			{
				result = m_downloadBuffer->initialise();
			}
		}

		if ( result )
		{
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
				storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth(), 0, format.Format, format.Type, nullptr );
				break;

			case GlTextureStorageType::e3D:
				storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth(), 0, format.Format, format.Type, nullptr );
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
				storage.getOpenGl().TexImage3D( storage.getGlType(), 0, format.Internal, size.getWidth(), size.getHeight(), p_storage.getOwner()->getDepth() * 6, 0, format.Format, format.Type, nullptr );
				break;
			}
		}
	}

	GlPboTextureStorageTraits::~GlPboTextureStorageTraits()
	{
		if ( checkFlag( m_storage.getCPUAccess(), AccessType::eWrite ) )
		{
			if ( m_uploadBuffer )
			{
				m_uploadBuffer->destroy();
				m_uploadBuffer.reset();
			}
		}

		if ( checkFlag( m_storage.getCPUAccess(), AccessType::eRead ) )
		{
			if ( m_downloadBuffer )
			{
				m_downloadBuffer->destroy();
				m_downloadBuffer.reset();
			}
		}
	}

	void GlPboTextureStorageTraits::bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlPboTextureStorageTraits::unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlPboTextureStorageTraits::lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * result = nullptr;

		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eRead )
			 && checkFlag( p_lock, AccessType::eRead ) )
		{
			doDownloadAsync( p_storage, p_storage.getOwner()->getImage( p_index ) );
		}

		if ( checkFlag( p_lock, AccessType::eRead )
			 || checkFlag( p_lock, AccessType::eWrite ) )
		{
			result = p_storage.getOwner()->getImage( p_index ).getBuffer()->ptr();
		}

		return result;
	}

	void GlPboTextureStorageTraits::unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		if ( p_modified )
		{
			doUploadAsync( p_storage, p_storage.getOwner()->getImage( p_index ) );
		}
	}

	void GlPboTextureStorageTraits::fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		doUploadImage( p_storage, p_image, p_image.getBuffer()->constPtr() );
	}

	void GlPboTextureStorageTraits::doUploadAsync( TextureStorage & p_storage, TextureImage const & p_image )
	{
		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eWrite ) )
		{
			auto buffer = p_image.getBuffer();

			m_uploadBuffer->upload( 0u, buffer->size(), buffer->ptr() );
			m_uploadBuffer->bind();
			doUploadImage( p_storage, p_image, nullptr );
			m_uploadBuffer->unbind();
		}
	}

	void GlPboTextureStorageTraits::doUploadSync( TextureStorage & p_storage, TextureImage const & p_image )
	{
		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eWrite ) )
		{
			doUploadImage( p_storage, p_image, p_image.getBuffer()->constPtr() );
		}
	}

	void GlPboTextureStorageTraits::doDownloadAsync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eRead ) )
		{
			auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
			auto buffer = p_image.getBuffer();
			OpenGl::PixelFmt glPixelFmt = storage.getOpenGl().get( buffer->format() );

			m_downloadBuffer->bind();
			storage.getOpenGl().GetTexImage( storage.getGlType(), 0, glPixelFmt.Format, glPixelFmt.Type, nullptr );
			m_downloadBuffer->download( 0u, buffer->size(), buffer->ptr() );
		}
	}

	void GlPboTextureStorageTraits::doDownloadSync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( checkFlag( p_storage.getCPUAccess(), AccessType::eRead ) )
		{
			auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
			auto buffer = p_image.getBuffer();
			OpenGl::PixelFmt glPixelFmt = storage.getOpenGl().get( buffer->format() );
			storage.getOpenGl().GetTexImage( storage.getGlType(), 0, glPixelFmt.Format, glPixelFmt.Type, buffer->ptr() );
		}
	}

	void GlPboTextureStorageTraits::doUploadImage( TextureStorage & p_storage, TextureImage const & p_image, uint8_t const * p_buffer )
	{
		auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
		auto size = p_storage.getOwner()->getDimensions();
		OpenGl::PixelFmt format = storage.getOpenGl().get( p_storage.getOwner()->getPixelFormat() );

		switch ( storage.getGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.getOpenGl().TexSubImage1D( storage.getGlType(), 0, 0, size.getWidth(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e2D:
			storage.getOpenGl().TexSubImage2D( storage.getGlType(), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e2DMS:
			storage.getOpenGl().TexSubImage2D( storage.getGlType(), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e2DArray:
			storage.getOpenGl().TexSubImage3D( storage.getGlType(), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), p_image.getIndex() + 1, format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e3D:
			storage.getOpenGl().TexSubImage3D( storage.getGlType(), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), p_image.getIndex() + 1, format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.getOpenGl().TexSubImage2D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.getIndex() ), 0, 0, 0, size.getWidth(), size.getHeight(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.getOpenGl().TexSubImage3D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.getIndex() % 6 ), 0, 0, 0, p_image.getIndex(), size.getWidth(), size.getHeight(), p_image.getIndex() + 1, format.Format, format.Type, p_buffer );
			break;
		}
	}
}

#include "Texture/GlPboTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlDownloadPixelBuffer.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"
#include "Texture/GlUploadPixelBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPboTextureStorageTraits::GlPboTextureStorageTraits( TextureStorage & p_storage )
		: m_storage{ p_storage }
	{
		bool result = true;
		auto pxbuffer = p_storage.GetOwner()->GetImage().GetBuffer();
		auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			m_uploadBuffer = std::make_unique< GlUploadPixelBuffer >( storage.GetOpenGl(), storage.GetGlRenderSystem(), pxbuffer->size() );
			result = m_uploadBuffer != nullptr;

			if ( result )
			{
				result = m_uploadBuffer->Create();
			}

			if ( result )
			{
				result = m_uploadBuffer->Initialise();
			}
		}

		if ( result && CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead ) )
		{
			m_downloadBuffer  = std::make_unique< GlDownloadPixelBuffer >( storage.GetOpenGl(), storage.GetGlRenderSystem(), pxbuffer->size() );
			result = m_downloadBuffer != nullptr;

			if ( result )
			{
				result = m_downloadBuffer->Create();
			}

			if ( result )
			{
				result = m_downloadBuffer->Initialise();
			}
		}

		if ( result )
		{
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
				storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth(), 0, format.Format, format.Type, nullptr );
				break;

			case GlTextureStorageType::e3D:
				storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth(), 0, format.Format, format.Type, nullptr );
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
				storage.GetOpenGl().TexImage3D( storage.GetGlType(), 0, format.Internal, size.width(), size.height(), p_storage.GetOwner()->GetDepth() * 6, 0, format.Format, format.Type, nullptr );
				break;
			}
		}
	}

	GlPboTextureStorageTraits::~GlPboTextureStorageTraits()
	{
		if ( CheckFlag( m_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			if ( m_uploadBuffer )
			{
				m_uploadBuffer->Destroy();
				m_uploadBuffer.reset();
			}
		}

		if ( CheckFlag( m_storage.GetCPUAccess(), AccessType::eRead ) )
		{
			if ( m_downloadBuffer )
			{
				m_downloadBuffer->Destroy();
				m_downloadBuffer.reset();
			}
		}
	}

	void GlPboTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlPboTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlPboTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * result = nullptr;

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead )
			 && CheckFlag( p_lock, AccessType::eRead ) )
		{
			DoDownloadAsync( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}

		if ( CheckFlag( p_lock, AccessType::eRead )
			 || CheckFlag( p_lock, AccessType::eWrite ) )
		{
			result = p_storage.GetOwner()->GetImage( p_index ).GetBuffer()->ptr();
		}

		return result;
	}

	void GlPboTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		if ( p_modified )
		{
			DoUploadAsync( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}
	}

	void GlPboTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		DoUploadImage( p_storage, p_image, p_image.GetBuffer()->const_ptr() );
	}

	void GlPboTextureStorageTraits::DoUploadAsync( TextureStorage & p_storage, TextureImage const & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			auto buffer = p_image.GetBuffer();

			m_uploadBuffer->Upload( 0u, buffer->size(), buffer->ptr() );
			m_uploadBuffer->Bind();
			DoUploadImage( p_storage, p_image, nullptr );
			m_uploadBuffer->Unbind();
		}
	}

	void GlPboTextureStorageTraits::DoUploadSync( TextureStorage & p_storage, TextureImage const & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			DoUploadImage( p_storage, p_image, p_image.GetBuffer()->const_ptr() );
		}
	}

	void GlPboTextureStorageTraits::DoDownloadAsync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead ) )
		{
			auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
			auto buffer = p_image.GetBuffer();
			OpenGl::PixelFmt glPixelFmt = storage.GetOpenGl().Get( buffer->format() );

			m_downloadBuffer->Bind();
			storage.GetOpenGl().GetTexImage( storage.GetGlType(), 0, glPixelFmt.Format, glPixelFmt.Type, nullptr );
			m_downloadBuffer->Download( 0u, buffer->size(), buffer->ptr() );
		}
	}

	void GlPboTextureStorageTraits::DoDownloadSync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead ) )
		{
			auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
			auto buffer = p_image.GetBuffer();
			OpenGl::PixelFmt glPixelFmt = storage.GetOpenGl().Get( buffer->format() );
			storage.GetOpenGl().GetTexImage( storage.GetGlType(), 0, glPixelFmt.Format, glPixelFmt.Type, buffer->ptr() );
		}
	}

	void GlPboTextureStorageTraits::DoUploadImage( TextureStorage & p_storage, TextureImage const & p_image, uint8_t const * p_buffer )
	{
		auto & storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
		auto size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt format = storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( storage.GetGlType() )
		{
		case GlTextureStorageType::e1D:
			storage.GetOpenGl().TexSubImage1D( storage.GetGlType(), 0, 0, size.width(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e2D:
			storage.GetOpenGl().TexSubImage2D( storage.GetGlType(), 0, 0, 0, size.width(), size.height(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e2DMS:
			storage.GetOpenGl().TexSubImage2D( storage.GetGlType(), 0, 0, 0, size.width(), size.height(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e2DArray:
			storage.GetOpenGl().TexSubImage3D( storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), size.width(), size.height(), p_image.GetIndex() + 1, format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::e3D:
			storage.GetOpenGl().TexSubImage3D( storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), size.width(), size.height(), p_image.GetIndex() + 1, format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::eCubeMap:
			storage.GetOpenGl().TexSubImage2D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() ), 0, 0, 0, size.width(), size.height(), format.Format, format.Type, p_buffer );
			break;

		case GlTextureStorageType::eCubeMapArray:
			storage.GetOpenGl().TexSubImage3D( GlTextureStorageType( uint32_t( GlTextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() % 6 ), 0, 0, 0, p_image.GetIndex(), size.width(), size.height(), p_image.GetIndex() + 1, format.Format, format.Type, p_buffer );
			break;
		}
	}
}

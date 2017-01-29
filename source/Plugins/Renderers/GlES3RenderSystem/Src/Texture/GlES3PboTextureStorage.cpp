#include "Texture/GlES3PboTextureStorage.hpp"

#include "Common/OpenGlES3.hpp"
#include "Texture/GlES3DownloadPixelBuffer.hpp"
#include "Texture/GlES3Texture.hpp"
#include "Texture/GlES3TextureStorage.hpp"
#include "Texture/GlES3UploadPixelBuffer.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3PboTextureStorageTraits::GlES3PboTextureStorageTraits( TextureStorage & p_storage )
		: m_storage{ p_storage }
	{
		bool l_return = true;
		auto l_pxbuffer = p_storage.GetOwner()->GetImage().GetBuffer();
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3PboTextureStorageTraits > & >( p_storage );

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			m_uploadBuffer = std::make_unique< GlES3UploadPixelBuffer >( l_storage.GetOpenGlES3(), l_storage.GetGlES3RenderSystem(), l_pxbuffer );
			l_return = m_uploadBuffer != nullptr;

			if ( l_return )
			{
				l_return = m_uploadBuffer->Create();
			}

			if ( l_return )
			{
				l_return = m_uploadBuffer->Initialise();
			}
		}

		if ( l_return && CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead ) )
		{
			m_downloadBuffer  = std::make_unique< GlES3DownloadPixelBuffer >( l_storage.GetOpenGlES3(), l_storage.GetGlES3RenderSystem(), l_pxbuffer );
			l_return = m_downloadBuffer != nullptr;

			if ( l_return )
			{
				l_return = m_downloadBuffer->Create();
			}

			if ( l_return )
			{
				l_return = m_downloadBuffer->Initialise();
			}
		}

		if ( l_return )
		{
			auto l_size = p_storage.GetOwner()->GetDimensions();
			OpenGlES3::PixelFmt l_format = l_storage.GetOpenGlES3().Get( p_storage.GetOwner()->GetPixelFormat() );

			switch ( l_storage.GetGlES3Type() )
			{
			case GlES3TextureStorageType::e1D:
				l_storage.GetOpenGlES3().TexImage1D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), 0, l_format.Format, l_format.Type, nullptr );
				break;

			case GlES3TextureStorageType::e2D:
				l_storage.GetOpenGlES3().TexImage2D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
				break;

			case GlES3TextureStorageType::e2DMS:
				l_storage.GetOpenGlES3().TexImage2DMultisample( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size, true );
				break;

			case GlES3TextureStorageType::e2DArray:
				l_storage.GetOpenGlES3().TexImage3D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
				break;

			case GlES3TextureStorageType::e3D:
				l_storage.GetOpenGlES3().TexImage3D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
				break;

			case GlES3TextureStorageType::eCubeMap:
				l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFacePosX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
				l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFaceNegX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
				l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFacePosY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
				l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFaceNegY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
				l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFacePosZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
				l_storage.GetOpenGlES3().TexImage2D( GlES3TextureStorageType::eCubeMapFaceNegZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
				break;

			case GlES3TextureStorageType::eCubeMapArray:
				l_storage.GetOpenGlES3().TexImage3D( l_storage.GetGlES3Type(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() * 6, 0, l_format.Format, l_format.Type, nullptr );
				break;
			}
		}
	}

	GlES3PboTextureStorageTraits::~GlES3PboTextureStorageTraits()
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

	void GlES3PboTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	void GlES3PboTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlES3PboTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		uint8_t * l_return = nullptr;

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead )
			 && CheckFlag( p_lock, AccessType::eRead ) )
		{
			DoDownloadAsync( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}

		if ( CheckFlag( p_lock, AccessType::eRead )
			 || CheckFlag( p_lock, AccessType::eWrite ) )
		{
			l_return = p_storage.GetOwner()->GetImage( p_index ).GetBuffer()->ptr();
		}

		return l_return;
	}

	void GlES3PboTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		if ( p_modified )
		{
			DoUploadAsync( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}
	}

	void GlES3PboTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		DoUploadImage( p_storage, p_image, p_image.GetBuffer()->const_ptr() );
	}

	void GlES3PboTextureStorageTraits::DoUploadAsync( TextureStorage & p_storage, TextureImage const & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			auto l_buffer = p_image.GetBuffer();

			m_uploadBuffer->Upload( 0u, l_buffer->size(), l_buffer->ptr() );
			m_uploadBuffer->Bind();
			DoUploadImage( p_storage, p_image, nullptr );
			m_uploadBuffer->Unbind();
		}
	}

	void GlES3PboTextureStorageTraits::DoUploadSync( TextureStorage & p_storage, TextureImage const & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eWrite ) )
		{
			DoUploadImage( p_storage, p_image, p_image.GetBuffer()->const_ptr() );
		}
	}

	void GlES3PboTextureStorageTraits::DoDownloadAsync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead ) )
		{
			auto & l_storage = static_cast< GlES3TextureStorage< GlES3PboTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_image.GetBuffer();
			OpenGlES3::PixelFmt l_glPixelFmt = l_storage.GetOpenGlES3().Get( l_buffer->format() );

			m_downloadBuffer->Bind();
			l_storage.GetOpenGlES3().GetTexImage( l_storage.GetGlES3Type(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, nullptr );
			m_downloadBuffer->Download( 0u, l_buffer->size(), l_buffer->ptr() );
		}
	}

	void GlES3PboTextureStorageTraits::DoDownloadSync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::eRead ) )
		{
			auto & l_storage = static_cast< GlES3TextureStorage< GlES3PboTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_image.GetBuffer();
			OpenGlES3::PixelFmt l_glPixelFmt = l_storage.GetOpenGlES3().Get( l_buffer->format() );
			l_storage.GetOpenGlES3().GetTexImage( l_storage.GetGlES3Type(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, l_buffer->ptr() );
		}
	}

	void GlES3PboTextureStorageTraits::DoUploadImage( TextureStorage & p_storage, TextureImage const & p_image, uint8_t const * p_buffer )
	{
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3PboTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGlES3::PixelFmt l_format = l_storage.GetOpenGlES3().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlES3Type() )
		{
		case GlES3TextureStorageType::e1D:
			l_storage.GetOpenGlES3().TexSubImage1D( l_storage.GetGlES3Type(), 0, 0, l_size.width(), l_format.Format, l_format.Type, p_buffer );
			break;

		case GlES3TextureStorageType::e2D:
			l_storage.GetOpenGlES3().TexSubImage2D( l_storage.GetGlES3Type(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_buffer );
			break;

		case GlES3TextureStorageType::e2DMS:
			l_storage.GetOpenGlES3().TexSubImage2D( l_storage.GetGlES3Type(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_buffer );
			break;

		case GlES3TextureStorageType::e2DArray:
			l_storage.GetOpenGlES3().TexSubImage3D( l_storage.GetGlES3Type(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_buffer );
			break;

		case GlES3TextureStorageType::e3D:
			l_storage.GetOpenGlES3().TexSubImage3D( l_storage.GetGlES3Type(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_buffer );
			break;

		case GlES3TextureStorageType::eCubeMap:
			l_storage.GetOpenGlES3().TexSubImage2D( GlES3TextureStorageType( uint32_t( GlES3TextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() ), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_buffer );
			break;

		case GlES3TextureStorageType::eCubeMapArray:
			l_storage.GetOpenGlES3().TexSubImage3D( GlES3TextureStorageType( uint32_t( GlES3TextureStorageType::eCubeMapFacePosX ) + p_image.GetIndex() % 6 ), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_buffer );
			break;
		}
	}
}

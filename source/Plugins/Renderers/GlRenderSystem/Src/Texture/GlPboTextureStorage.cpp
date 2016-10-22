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
		bool l_return = true;
		auto l_pxbuffer = p_storage.GetOwner()->GetImage().GetBuffer();
		auto & l_storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Write ) )
		{
			m_uploadBuffer = std::make_unique< GlUploadPixelBuffer >( l_storage.GetOpenGl(), l_storage.GetGlRenderSystem(), l_pxbuffer );
			l_return = m_uploadBuffer->Create();

			if ( l_return && m_uploadBuffer )
			{
				l_return = m_uploadBuffer->Initialise();
			}
		}

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Read ) )
		{
			m_downloadBuffer  = std::make_unique< GlDownloadPixelBuffer >( l_storage.GetOpenGl(), l_storage.GetGlRenderSystem(), l_pxbuffer );
			l_return = m_downloadBuffer->Create();

			if ( l_return && m_downloadBuffer )
			{
				l_return = m_downloadBuffer->Initialise();
			}
		}

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
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_3D:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth(), 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP:
			l_storage.GetOpenGl().TexImage2D( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGX, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGY, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			l_storage.GetOpenGl().TexImage2D( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_NEGZ, 0, l_format.Internal, l_size, 0, l_format.Format, l_format.Type, nullptr );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP_ARRAY:
			l_storage.GetOpenGl().TexImage3D( l_storage.GetGlType(), 0, l_format.Internal, l_size.width(), l_size.height(), p_storage.GetOwner()->GetDepth() * 6, 0, l_format.Format, l_format.Type, nullptr );
			break;
		}
	}

	GlPboTextureStorageTraits::~GlPboTextureStorageTraits()
	{
		if ( CheckFlag( m_storage.GetCPUAccess(), AccessType::Write ) )
		{
			if ( m_uploadBuffer )
			{
				m_uploadBuffer->Destroy();
				m_uploadBuffer.reset();
			}
		}

		if ( CheckFlag( m_storage.GetCPUAccess(), AccessType::Read ) )
		{
			if ( m_downloadBuffer )
			{
				m_downloadBuffer->Destroy();
				m_downloadBuffer.reset();
			}
		}
	}

	bool GlPboTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		return true;
	}

	void GlPboTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
	}

	uint8_t * GlPboTextureStorageTraits::Lock( TextureStorage & p_storage, AccessType p_lock, uint32_t p_index )
	{
		uint8_t * l_return = nullptr;

		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Read )
			 && CheckFlag( p_lock, AccessType::Read ) )
		{
			DoDownloadAsync( p_storage, p_storage.GetOwner()->GetImage( p_index ) );
		}

		if ( CheckFlag( p_lock, AccessType::Read )
			 || CheckFlag( p_lock, AccessType::Write ) )
		{
			l_return = p_storage.GetOwner()->GetImage( p_index ).GetBuffer()->ptr();
		}

		return l_return;
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
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Write ) )
		{
			auto l_buffer = p_image.GetBuffer();

			if ( m_uploadBuffer->Fill( l_buffer->ptr(), l_buffer->size() ) )
			{
				if ( m_uploadBuffer->Bind() )
				{
					DoUploadImage( p_storage, p_image, nullptr );
					m_uploadBuffer->Unbind();
				}
			}
		}
	}

	void GlPboTextureStorageTraits::DoUploadSync( TextureStorage & p_storage, TextureImage const & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Write ) )
		{
			auto l_buffer = p_image.GetBuffer();
			DoUploadImage( p_storage, p_image, l_buffer->const_ptr() );
		}
	}

	void GlPboTextureStorageTraits::DoDownloadAsync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Read ) )
		{
			auto & l_storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_image.GetBuffer();
			OpenGl::PixelFmt l_glPixelFmt = l_storage.GetOpenGl().Get( l_buffer->format() );

			if ( m_downloadBuffer->Bind() )
			{
				l_storage.GetOpenGl().GetTexImage( l_storage.GetGlType(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, 0 );
				uint8_t * l_data = m_downloadBuffer->Lock( eGL_LOCK_READ_ONLY );

				if ( l_data )
				{
					memcpy( l_buffer->ptr(), l_data, l_buffer->size() );
					m_downloadBuffer->Unlock();
				}

				m_downloadBuffer->Unbind();
			}
		}
	}

	void GlPboTextureStorageTraits::DoDownloadSync( TextureStorage & p_storage, TextureImage & p_image )
	{
		if ( CheckFlag( p_storage.GetCPUAccess(), AccessType::Read ) )
		{
			auto & l_storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_image.GetBuffer();
			OpenGl::PixelFmt l_glPixelFmt = l_storage.GetOpenGl().Get( l_buffer->format() );
			l_storage.GetOpenGl().GetTexImage( l_storage.GetGlType(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, l_buffer->ptr() );
		}
	}

	void GlPboTextureStorageTraits::DoUploadImage( TextureStorage & p_storage, TextureImage const & p_image, uint8_t const * p_buffer )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlPboTextureStorageTraits > & >( p_storage );
		auto l_size = p_storage.GetOwner()->GetDimensions();
		OpenGl::PixelFmt l_format = l_storage.GetOpenGl().Get( p_storage.GetOwner()->GetPixelFormat() );

		switch ( l_storage.GetGlType() )
		{
		case eGL_TEXTURE_STORAGE_1D:
			l_storage.GetOpenGl().TexSubImage1D( l_storage.GetGlType(), 0, 0, l_size.width(), l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2D:
			l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			l_storage.GetOpenGl().TexSubImage2D( l_storage.GetGlType(), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_2DARRAY:
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_3D:
			l_storage.GetOpenGl().TexSubImage3D( l_storage.GetGlType(), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP:
			l_storage.GetOpenGl().TexSubImage2D( eGL_TEXTURE_STORAGE( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX + p_image.GetIndex() ), 0, 0, 0, l_size.width(), l_size.height(), l_format.Format, l_format.Type, p_buffer );
			break;

		case eGL_TEXTURE_STORAGE_CUBE_MAP_ARRAY:
			l_storage.GetOpenGl().TexSubImage3D( eGL_TEXTURE_STORAGE( eGL_TEXTURE_STORAGE_CUBE_MAP_FACE_POSX + p_image.GetIndex() % 6 ), 0, 0, 0, p_image.GetIndex(), l_size.width(), l_size.height(), p_image.GetIndex() + 1, l_format.Format, l_format.Type, p_buffer );
			break;
		}
	}
}

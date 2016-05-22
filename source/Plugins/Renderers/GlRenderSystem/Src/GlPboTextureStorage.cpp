#include "GlPboTextureStorage.hpp"

#include "GlDownloadPixelBuffer.hpp"
#include "GlTexture.hpp"
#include "GlUploadPixelBuffer.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPboTextureStorage::GlPboTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, TextureStorageType p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: GlTextureStorage{ p_gl, p_renderSystem, p_type, p_image, p_cpuAccess, p_gpuAccess }
	{
		bool l_return = true;
		auto l_pxbuffer = GetOwner()->GetBuffer();

		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			m_uploadBuffer = std::make_unique< GlUploadPixelBuffer >( GetOpenGl(), m_glRenderSystem, l_pxbuffer );
			l_return = m_uploadBuffer->Create();

			if ( l_return && m_uploadBuffer )
			{
				l_return = m_uploadBuffer->Initialise();
			}
		}

		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			m_downloadBuffer  = std::make_unique< GlDownloadPixelBuffer >( GetOpenGl(), m_glRenderSystem, l_pxbuffer );
			l_return = m_downloadBuffer->Create();

			if ( l_return && m_downloadBuffer )
			{
				l_return = m_downloadBuffer->Initialise();
			}
		}

		if ( l_return )
		{
			DoFill( l_pxbuffer->const_ptr(), l_pxbuffer->dimensions(), l_pxbuffer->format() );
		}
	}

	GlPboTextureStorage::~GlPboTextureStorage()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			if ( m_uploadBuffer )
			{
				m_uploadBuffer->Cleanup();
				m_uploadBuffer->Destroy();
				m_uploadBuffer.reset();
			}
		}

		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			if ( m_downloadBuffer )
			{
				m_downloadBuffer->Cleanup();
				m_downloadBuffer->Destroy();
				m_downloadBuffer.reset();
			}
		}
	}

	bool GlPboTextureStorage::Bind( uint32_t p_index )const
	{
		return true;
	}

	void GlPboTextureStorage::Unbind( uint32_t p_index )const
	{
	}

	uint8_t * GlPboTextureStorage::Lock( uint32_t p_lock )
	{
		if ( ( p_lock & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ
				&& ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			DoDownloadAsync();
		}

		return GetOwner()->GetBuffer()->ptr();
	}

	void GlPboTextureStorage::Unlock( bool p_modified )
	{
		if ( p_modified )
		{
			DoUploadAsync();
		}
	}

	void GlPboTextureStorage::DoUploadAsync()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			auto l_buffer = GetOwner()->GetBuffer();

			if ( m_uploadBuffer->Fill( l_buffer->ptr(), l_buffer->size() ) )
			{
				if ( m_uploadBuffer->Bind() )
				{
					DoUploadImage( l_buffer->width(), l_buffer->height(), l_buffer->format(), nullptr );
					m_uploadBuffer->Unbind();
				}
			}
		}
	}

	void GlPboTextureStorage::DoUploadSync()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			auto l_buffer = GetOwner()->GetBuffer();
			DoUploadImage( l_buffer->width(), l_buffer->height(), l_buffer->format(), l_buffer->const_ptr() );
		}
	}

	void GlPboTextureStorage::DoDownloadAsync()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			auto l_buffer = GetOwner()->GetBuffer();
			OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );

			if ( m_downloadBuffer->Bind() )
			{
				GetOpenGl().GetTexImage( m_glType, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, 0 );
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

	void GlPboTextureStorage::DoDownloadSync()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			auto l_buffer = GetOwner()->GetBuffer();
			OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );
			GetOpenGl().GetTexImage( m_glType, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, l_buffer->ptr() );
		}
	}
}

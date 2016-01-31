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
	GlPboTextureStorage::GlPboTextureStorage( OpenGl & p_gl, GlTexture & p_texture, GlRenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: GlTextureStorage( p_gl, p_texture, p_renderSystem, p_cpuAccess, p_gpuAccess )
	{
	}

	GlPboTextureStorage::~GlPboTextureStorage()
	{
	}

	bool GlPboTextureStorage::DoCreate()
	{
		bool l_return = true;
		PxBufferBaseSPtr l_pxbuffer = m_buffer.lock();

		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			m_uploadBuffer = std::make_unique< GlUploadPixelBuffer >( GetOpenGl(), m_glRenderSystem, l_pxbuffer );
			l_return = m_uploadBuffer->Create();
		}

		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			m_downloadBuffer  = std::make_unique< GlDownloadPixelBuffer >( GetOpenGl(), m_glRenderSystem, l_pxbuffer );
			l_return = m_downloadBuffer->Create();
		}

		return l_return;
	}

	void GlPboTextureStorage::DoDestroy()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			if ( m_uploadBuffer )
			{
				m_uploadBuffer->Destroy();
				m_uploadBuffer.reset();
			}
		}

		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			if ( m_downloadBuffer )
			{
				m_downloadBuffer->Destroy();
				m_downloadBuffer.reset();
			}
		}
	}

	bool GlPboTextureStorage::DoInitialise()
	{
		bool l_return = true;

		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			if ( l_return && m_uploadBuffer )
			{
				l_return = m_uploadBuffer->Initialise();
			}
		}

		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			if ( l_return && m_downloadBuffer )
			{
				l_return = m_downloadBuffer->Initialise();
			}
		}

		if ( l_return )
		{
			PxBufferBaseSPtr l_buffer = m_buffer.lock();
			DoFill( l_buffer->const_ptr(), l_buffer->dimensions(), l_buffer->format() );
		}

		return l_return;
	}

	void GlPboTextureStorage::DoCleanup()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			if ( m_uploadBuffer )
			{
				m_uploadBuffer->Cleanup();
			}
		}

		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			if ( m_downloadBuffer )
			{
				m_downloadBuffer->Cleanup();
			}
		}
	}

	void GlPboTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( p_format );

		switch ( GetOwner()->GetGlDimension() )
		{
		case eGL_TEXDIM_1D:
			GetOpenGl().TexImage1D( eGL_TEXDIM_1D, 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D:
			GetOpenGl().TexImage2D( eGL_TEXDIM_2D, 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_3D:
			GetOpenGl().TexImage3D( eGL_TEXDIM_3D, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / m_depth, m_depth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			GetOpenGl().TexImage3D( eGL_TEXDIM_2D_ARRAY, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / m_depth, m_depth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;
		}
	}

	bool GlPboTextureStorage::DoBind( uint32_t p_index )
	{
		return true;
	}

	void GlPboTextureStorage::DoUnbind( uint32_t p_index )
	{
	}

	uint8_t * GlPboTextureStorage::DoLock( uint32_t p_lock )
	{
		if ( ( p_lock & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ
			 && ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			DoDownloadAsync();
		}

		return m_buffer.lock()->ptr();
	}

	void GlPboTextureStorage::DoUnlock( bool p_modified )
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
			PxBufferBaseSPtr l_buffer = m_buffer.lock();

			if ( m_uploadBuffer->Fill( l_buffer->ptr(), l_buffer->size() ) )
			{
				if ( m_uploadBuffer->Bind() )
				{
					DoUploadImage( l_buffer->width(), l_buffer->height(), GetOpenGl().Get( l_buffer->format() ), nullptr );
					m_uploadBuffer->Unbind();
				}
			}
		}
	}

	void GlPboTextureStorage::DoUploadSync()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			PxBufferBaseSPtr l_buffer = m_buffer.lock();
			DoUploadImage( l_buffer->width(), l_buffer->height(), GetOpenGl().Get( l_buffer->format() ), l_buffer->const_ptr() );
		}
	}

	void GlPboTextureStorage::DoDownloadAsync()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			PxBufferBaseSPtr l_buffer = m_buffer.lock();
			OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );
			GetOpenGl().BindTexture( GetOwner()->GetGlDimension(), GetOwner()->GetGlName() );

			if ( m_downloadBuffer->Bind() )
			{
				GetOpenGl().GetTexImage( GetOwner()->GetGlDimension(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, 0 );
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
			PxBufferBaseSPtr l_buffer = m_buffer.lock();
			OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );
			GetOpenGl().BindTexture( GetOwner()->GetGlDimension(), GetOwner()->GetGlName() );
			GetOpenGl().GetTexImage( GetOwner()->GetGlDimension(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, l_buffer->ptr() );
		}
	}

	void GlPboTextureStorage::DoUploadImage( uint32_t p_width, uint32_t p_height, OpenGl::PixelFmt const & p_format, uint8_t const * p_buffer )
	{
		GetOpenGl().BindTexture( GetOwner()->GetGlDimension(), GetOwner()->GetGlName() );

		switch ( GetOwner()->GetGlDimension() )
		{
		case eGL_TEXDIM_1D:
			GetOpenGl().TexSubImage1D( eGL_TEXDIM_1D, 0, 0, p_width, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D:
			GetOpenGl().TexSubImage2D( eGL_TEXDIM_2D, 0, 0, 0, p_width, p_height, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_3D:
			GetOpenGl().TexSubImage3D( eGL_TEXDIM_3D, 0, 0, 0, 0, p_width, p_height / m_depth, m_depth, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			GetOpenGl().TexSubImage3D( eGL_TEXDIM_2D_ARRAY, 0, 0, 0, 0, p_width, p_height / m_depth, m_depth, p_format.Format, p_format.Type, p_buffer );
			break;
		}
	}
}

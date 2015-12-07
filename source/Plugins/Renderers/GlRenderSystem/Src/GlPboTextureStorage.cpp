#include "GlPboTextureStorage.hpp"

#include "GlDownloadPixelBuffer.hpp"
#include "GlUploadPixelBuffer.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlPboTextureStorage::GlPboTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: GlTextureStorage( p_gl, p_renderSystem, p_cpuAccess, p_gpuAccess )
		, m_currentDlPbo( 0 )
		, m_currentUlPbo( 0 )
	{
	}

	GlPboTextureStorage::~GlPboTextureStorage()
	{
	}

	bool GlPboTextureStorage::DoCreate()
	{
		bool l_return = true;
		PxBufferBaseSPtr l_pxbuffer = m_buffer.lock();

		for ( auto && l_buffer : m_uploadBuffers )
		{
			if ( l_return )
			{
				l_buffer = std::make_unique< GlUploadPixelBuffer >( GetOpenGl(), m_glRenderSystem, l_pxbuffer );
				l_return = l_buffer->Create();
			}
		}

		for ( auto && l_buffer : m_downloadBuffers )
		{
			if ( l_return )
			{
				l_buffer = std::make_unique< GlDownloadPixelBuffer >( GetOpenGl(), m_glRenderSystem, l_pxbuffer );
				l_return = l_buffer->Create();
			}
		}

		return l_return;
	}

	void GlPboTextureStorage::DoDestroy()
	{
		for ( auto && l_buffer : m_uploadBuffers )
		{
			l_buffer->Destroy();
			l_buffer.reset();
		}

		for ( auto && l_buffer : m_downloadBuffers )
		{
			l_buffer->Destroy();
			l_buffer.reset();
		}
	}

	bool GlPboTextureStorage::DoInitialise()
	{
		bool l_return = true;

		for ( auto && l_buffer : m_uploadBuffers )
		{
			if ( l_return )
			{
				l_return = l_buffer->Initialise();
			}
		}

		for ( auto && l_buffer : m_downloadBuffers )
		{
			if ( l_return )
			{
				l_return = l_buffer->Initialise();
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
		for ( auto && l_buffer : m_uploadBuffers )
		{
			l_buffer->Cleanup();
		}

		for ( auto && l_buffer : m_downloadBuffers )
		{
			l_buffer->Cleanup();
		}
	}

	void GlPboTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( p_format );

		switch ( m_glDimension )
		{
		case eGL_TEXDIM_1D:
			GetOpenGl().TexImage1D( m_glDimension, 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D:
			GetOpenGl().TexImage2D( m_glDimension, 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_3D:
			GetOpenGl().TexImage3D( m_glDimension, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / m_depth, m_depth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			GetOpenGl().TexImage3D( m_glDimension, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / m_depth, m_depth, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
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
		m_currentUlPbo = ( m_currentUlPbo + 1 ) % 2;
		int l_nextUlPbo = ( m_currentUlPbo + 1 ) % 2;
		GlUploadPixelBuffer & l_bufferOut = *m_uploadBuffers[m_currentUlPbo];
		GlUploadPixelBuffer & l_bufferIn = *m_uploadBuffers[l_nextUlPbo];

		if ( l_bufferOut.Bind() )
		{
			PxBufferBaseSPtr l_buffer = m_buffer.lock();
			DoUploadImage( l_buffer->width(), l_buffer->height(), GetOpenGl().Get( l_buffer->format() ), nullptr );
			l_bufferIn.Fill( l_buffer->ptr(), l_buffer->size() );
			l_bufferOut.Unbind();
		}
	}

	void GlPboTextureStorage::DoUploadSync()
	{
		PxBufferBaseSPtr l_buffer = m_buffer.lock();
		DoUploadImage( l_buffer->width(), l_buffer->height(), GetOpenGl().Get( l_buffer->format() ), l_buffer->const_ptr() );
	}

	void GlPboTextureStorage::DoDownloadAsync()
	{
		PxBufferBaseSPtr l_buffer = m_buffer.lock();
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );
		m_currentDlPbo = ( m_currentDlPbo + 1 ) % 2;
		int l_nextDlPbo = ( m_currentDlPbo + 1 ) % 2;
		GlDownloadPixelBuffer & l_bufferOut = *m_downloadBuffers[m_currentDlPbo];
		GlDownloadPixelBuffer & l_bufferIn = *m_downloadBuffers[l_nextDlPbo];

		if ( l_bufferOut.Bind() )
		{
			GetOpenGl().GetTexImage( m_glDimension, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, 0 );

			if ( l_bufferIn.Bind() )
			{
				void * pData = l_bufferIn.Lock( eGL_LOCK_READ_ONLY );

				if ( pData )
				{
					memcpy( l_buffer->ptr(), pData, l_buffer->size() );
					l_bufferIn.Unlock();
				}

				l_bufferIn.Unbind();
			}

			l_bufferOut.Unbind();
		}
	}

	void GlPboTextureStorage::DoDownloadSync()
	{
		PxBufferBaseSPtr l_buffer = m_buffer.lock();
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );
		GetOpenGl().GetTexImage( m_glDimension, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, l_buffer->ptr() );
	}

	void GlPboTextureStorage::DoUploadImage( uint32_t p_width, uint32_t p_height, OpenGl::PixelFmt const & p_format, uint8_t const * p_buffer )
	{
		switch ( m_glDimension )
		{
		case eGL_TEXDIM_1D:
			GetOpenGl().TexSubImage1D( m_glDimension, 0, 0, p_width, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D:
			GetOpenGl().TexSubImage2D( m_glDimension, 0, 0, 0, p_width, p_height, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_3D:
			GetOpenGl().TexSubImage3D( m_glDimension, 0, 0, 0, 0, p_width, p_height / m_depth, m_depth, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			GetOpenGl().TexSubImage3D( m_glDimension, 0, 0, 0, 0, p_width, p_height / m_depth, m_depth, p_format.Format, p_format.Type, p_buffer );
			break;
		}
	}
}

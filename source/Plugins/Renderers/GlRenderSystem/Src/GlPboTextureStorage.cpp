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
	GlPboTextureStorage::GlPboTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
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

	void GlPboTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( p_format );
		auto l_glDimension = GetOpenGl().Get( m_type );

		switch ( l_glDimension )
		{
		case eGL_TEXDIM_1D:
			GetOpenGl().TexImage1D( eGL_TEXDIM_1D, 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D:
			GetOpenGl().TexImage2D( eGL_TEXDIM_2D, 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_3D:
			GetOpenGl().TexImage3D( eGL_TEXDIM_3D, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / GetOwner()->GetDepth(), GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			GetOpenGl().TexImage3D( eGL_TEXDIM_2D_ARRAY, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / GetOwner()->GetDepth(), GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;
		}
	}

	bool GlPboTextureStorage::Bind( uint32_t p_index )
	{
		return true;
	}

	void GlPboTextureStorage::Unbind( uint32_t p_index )
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
			auto l_buffer = GetOwner()->GetBuffer();
			DoUploadImage( l_buffer->width(), l_buffer->height(), GetOpenGl().Get( l_buffer->format() ), l_buffer->const_ptr() );
		}
	}

	void GlPboTextureStorage::DoDownloadAsync()
	{
		if ( ( m_cpuAccess & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ )
		{
			auto l_buffer = GetOwner()->GetBuffer();
			auto l_glDimension = GetOpenGl().Get( m_type );
			OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );
			GetOpenGl().BindTexture( l_glDimension, GetOwner()->GetGlName() );

			if ( m_downloadBuffer->Bind() )
			{
				GetOpenGl().GetTexImage( l_glDimension, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, 0 );
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
			auto l_glDimension = GetOpenGl().Get( m_type );
			auto l_buffer = GetOwner()->GetBuffer();
			OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( l_buffer->format() );
			GetOpenGl().BindTexture( l_glDimension, GetOwner()->GetGlName() );
			GetOpenGl().GetTexImage( l_glDimension, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, l_buffer->ptr() );
		}
	}

	void GlPboTextureStorage::DoUploadImage( uint32_t p_width, uint32_t p_height, OpenGl::PixelFmt const & p_format, uint8_t const * p_buffer )
	{
		auto l_glDimension = GetOpenGl().Get( m_type );
		GetOpenGl().BindTexture( l_glDimension, GetOwner()->GetGlName() );

		switch ( l_glDimension )
		{
		case eGL_TEXDIM_1D:
			GetOpenGl().TexSubImage1D( eGL_TEXDIM_1D, 0, 0, p_width, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D:
			GetOpenGl().TexSubImage2D( eGL_TEXDIM_2D, 0, 0, 0, p_width, p_height, p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_3D:
			GetOpenGl().TexSubImage3D( eGL_TEXDIM_3D, 0, 0, 0, 0, p_width, p_height / GetOwner()->GetDepth(), GetOwner()->GetDepth(), p_format.Format, p_format.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			GetOpenGl().TexSubImage3D( eGL_TEXDIM_2D_ARRAY, 0, 0, 0, 0, p_width, p_height / GetOwner()->GetDepth(), GetOwner()->GetDepth(), p_format.Format, p_format.Type, p_buffer );
			break;
		}
	}
}

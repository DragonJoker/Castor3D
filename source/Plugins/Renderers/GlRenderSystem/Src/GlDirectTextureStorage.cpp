#include "GlDirectTextureStorage.hpp"

#include "GlTexture.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>
#include <PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDirectTextureStorage::GlDirectTextureStorage( OpenGl & p_gl, GlTexture & p_texture, GlRenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: GlTextureStorage( p_gl, p_texture, p_renderSystem, p_cpuAccess, p_gpuAccess )
	{
	}

	GlDirectTextureStorage::~GlDirectTextureStorage()
	{
	}

	bool GlDirectTextureStorage::DoCreate()
	{
		return true;
	}

	void GlDirectTextureStorage::DoDestroy()
	{
	}

	bool GlDirectTextureStorage::DoInitialise()
	{
		PxBufferBaseSPtr l_buffer = m_buffer.lock();
		DoFill( l_buffer->const_ptr(), l_buffer->dimensions(), l_buffer->format() );
		return true;
	}

	void GlDirectTextureStorage::DoCleanup()
	{
	}

	void GlDirectTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
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

	bool GlDirectTextureStorage::DoBind( uint32_t p_index )
	{
		return true;
	}

	void GlDirectTextureStorage::DoUnbind( uint32_t p_index )
	{
	}

	uint8_t * GlDirectTextureStorage::DoLock( uint32_t p_lock )
	{
		REQUIRE( !m_buffer.expired() );
		uint8_t * l_return = NULL;

		if ( ( m_cpuAccess && p_lock & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ
			 || ( m_cpuAccess & p_lock & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			l_return = m_buffer.lock()->ptr();
		}

		return l_return;
	}

	void GlDirectTextureStorage::DoUnlock( bool p_modified )
	{
		if ( p_modified && ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			PxBufferBaseSPtr l_buffer = m_buffer.lock();
			OpenGl::PixelFmt l_format = GetOpenGl().Get( l_buffer->format() );
			GetOpenGl().BindTexture( GetOwner()->GetGlDimension(), GetOwner()->GetGlName() );

			switch ( GetOwner()->GetGlDimension() )
			{
			case eGL_TEXDIM_1D:
				GetOpenGl().TexSubImage1D( eGL_TEXDIM_1D, 0, 0, l_buffer->width(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXDIM_2D:
				GetOpenGl().TexSubImage2D( eGL_TEXDIM_2D, 0, 0, 0, l_buffer->width(), l_buffer->height(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXDIM_3D:
				GetOpenGl().TexSubImage3D( eGL_TEXDIM_3D, 0, 0, 0, 0, l_buffer->width(), l_buffer->height() / m_depth, m_depth, l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXDIM_2D_ARRAY:
				GetOpenGl().TexSubImage3D( eGL_TEXDIM_2D_ARRAY, 0, 0, 0, 0, l_buffer->width(), l_buffer->height() / m_depth, m_depth, l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;
			}
		}
	}
}

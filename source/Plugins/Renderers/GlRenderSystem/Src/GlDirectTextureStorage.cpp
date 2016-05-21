#include "GlDirectTextureStorage.hpp"

#include "GlTexture.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>
#include <PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDirectTextureStorage::GlDirectTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: GlTextureStorage{ p_gl, p_renderSystem, p_type, p_image, p_cpuAccess, p_gpuAccess }
	{
		auto l_buffer = GetOwner()->GetBuffer();
		DoFill( l_buffer->const_ptr(), l_buffer->dimensions(), l_buffer->format() );
	}

	GlDirectTextureStorage::~GlDirectTextureStorage()
	{
	}

	bool GlDirectTextureStorage::Bind( uint32_t p_index )const
	{
		return true;
	}

	void GlDirectTextureStorage::Unbind( uint32_t p_index )const
	{
	}

	uint8_t * GlDirectTextureStorage::Lock( uint32_t p_lock )
	{
		uint8_t * l_return = nullptr;

		if ( ( m_cpuAccess && p_lock & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ
				|| ( m_cpuAccess & p_lock & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			l_return = GetOwner()->GetBuffer()->ptr();
		}

		return l_return;
	}

	void GlDirectTextureStorage::Unlock( bool p_modified )
	{
		if ( p_modified && ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			auto l_buffer = GetOwner()->GetBuffer();
			auto l_glDimension = GetOpenGl().Get( m_type );
			OpenGl::PixelFmt l_format = GetOpenGl().Get( l_buffer->format() );

			switch ( l_glDimension )
			{
			case eGL_TEXDIM_1D:
				GetOpenGl().TexSubImage1D( l_glDimension, 0, 0, l_buffer->width(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXDIM_2D:
				GetOpenGl().TexSubImage2D( l_glDimension, 0, 0, 0, l_buffer->width(), l_buffer->height(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXDIM_3D:
				GetOpenGl().TexSubImage3D( l_glDimension, 0, 0, 0, 0, l_buffer->width(), l_buffer->height() / GetOwner()->GetDepth(), GetOwner()->GetDepth(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;

			case eGL_TEXDIM_2D_ARRAY:
				GetOpenGl().TexSubImage3D( l_glDimension, 0, 0, 0, 0, l_buffer->width(), l_buffer->height() / GetOwner()->GetDepth(), GetOwner()->GetDepth(), l_format.Format, l_format.Type, l_buffer->const_ptr() );
				break;
			}
		}
	}

	void GlDirectTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( p_format );
		auto l_glDimension = GetOpenGl().Get( m_type );

		switch ( l_glDimension )
		{
		case eGL_TEXDIM_1D:
			GetOpenGl().TexImage1D( l_glDimension, 0, l_glPixelFmt.Internal, p_size.width(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D:
			GetOpenGl().TexImage2D( l_glDimension, 0, l_glPixelFmt.Internal, p_size, 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_3D:
			GetOpenGl().TexImage3D( l_glDimension, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / GetOwner()->GetDepth(), GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;

		case eGL_TEXDIM_2D_ARRAY:
			GetOpenGl().TexImage3D( l_glDimension, 0, l_glPixelFmt.Internal, p_size.width(), p_size.height() / GetOwner()->GetDepth(), GetOwner()->GetDepth(), 0, l_glPixelFmt.Format, l_glPixelFmt.Type, p_buffer );
			break;
		}
	}
}

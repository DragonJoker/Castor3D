#include "GlImmutableTextureStorage.hpp"

#include "GlTexture.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>
#include <PixelBufferBase.hpp>

#include <Texture/TextureImage.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlImmutableTextureStorage::GlImmutableTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, TextureStorageType p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: GlTextureStorage{ p_gl, p_renderSystem, p_type, p_image, p_cpuAccess, p_gpuAccess }
	{
		auto l_buffer = GetOwner()->GetBuffer();
		DoFill( l_buffer->const_ptr(), l_buffer->dimensions(), l_buffer->format() );
	}

	GlImmutableTextureStorage::~GlImmutableTextureStorage()
	{
	}

	bool GlImmutableTextureStorage::Bind( uint32_t p_index )const
	{
		return true;
	}

	void GlImmutableTextureStorage::Unbind( uint32_t p_index )const
	{
	}

	uint8_t * GlImmutableTextureStorage::Lock( uint32_t p_lock )
	{
		uint8_t * l_return = nullptr;

		if ( ( m_cpuAccess && p_lock & eACCESS_TYPE_READ ) == eACCESS_TYPE_READ
				|| ( m_cpuAccess & p_lock & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			l_return = GetOwner()->GetBuffer()->ptr();
		}

		return l_return;
	}

	void GlImmutableTextureStorage::Unlock( bool p_modified )
	{
		if ( p_modified && ( m_cpuAccess & eACCESS_TYPE_WRITE ) == eACCESS_TYPE_WRITE )
		{
			auto l_buffer = GetOwner()->GetBuffer();
			DoUploadImage( l_buffer->dimensions().width(), l_buffer->dimensions().height(), l_buffer->format(), l_buffer->const_ptr() );
		}
	}

	void GlImmutableTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
		OpenGl::PixelFmt l_glPixelFmt = GetOpenGl().Get( p_format );

		switch ( m_glType )
		{
		case eGL_TEXTURE_STORAGE_1D:
			GetOpenGl().TexStorage1D( m_glType, 4, l_glPixelFmt.Internal, p_size.width() );
			break;

		case eGL_TEXTURE_STORAGE_2D:
			GetOpenGl().TexStorage2D( m_glType, 4, l_glPixelFmt.Internal, p_size.width(), p_size.height() );
			break;

		case eGL_TEXTURE_STORAGE_2DMS:
			GetOpenGl().TexStorage2DMultisample( m_glType, 8, l_glPixelFmt.Internal, p_size.width(), p_size.height(), true );
			break;

		case eGL_TEXTURE_STORAGE_3D:
		case eGL_TEXTURE_STORAGE_2DARRAY:
			GetOpenGl().TexStorage3D( m_glType, 4, l_glPixelFmt.Internal, p_size.width(), p_size.height() / GetOwner()->GetDepth(), GetOwner()->GetDepth() );
			break;

		default:
			FAILURE( "Storage type unsupported for immutable storages" );
			CASTOR_EXCEPTION( cuT( "Storage type unsupported for immutable storages" ) );
			break;
		}

		DoUploadImage( p_size.width(), p_size.height(), p_format, p_buffer );
	}
}

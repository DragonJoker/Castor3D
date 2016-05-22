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
	GlDirectTextureStorage::GlDirectTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, TextureStorageType p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
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
			DoUploadImage( l_buffer->dimensions().width(), l_buffer->dimensions().height(), l_buffer->format(), l_buffer->const_ptr() );
		}
	}
}

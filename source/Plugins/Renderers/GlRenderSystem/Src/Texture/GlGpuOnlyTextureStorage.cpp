#include "Texture/GlGpuOnlyTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlDownloadPixelBuffer.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlUploadPixelBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGpuOnlyTextureStorage::GlGpuOnlyTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem, TextureStorageType p_type, TextureImage & p_image, uint8_t p_gpuAccess )
		: GlTextureStorage{ p_gl, p_renderSystem, p_type, p_image, 0, p_gpuAccess }
	{
		auto l_buffer = GetOwner()->GetBuffer();
		DoFill( nullptr, l_buffer->dimensions(), l_buffer->format() );
	}

	GlGpuOnlyTextureStorage::~GlGpuOnlyTextureStorage()
	{
	}

	bool GlGpuOnlyTextureStorage::Bind( uint32_t p_index )const
	{
		return true;
	}

	void GlGpuOnlyTextureStorage::Unbind( uint32_t p_index )const
	{
	}

	uint8_t * GlGpuOnlyTextureStorage::Lock( uint32_t p_lock )
	{
		FAILURE( "GPU only, you can't lock this storage" );
		return nullptr;
	}

	void GlGpuOnlyTextureStorage::Unlock( bool p_modified )
	{
	}
}
